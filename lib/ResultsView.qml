/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick
import QtQuick.Templates as T

import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.milou as Milou

ListView {
    id: listView
    property alias queryString: resultModel.queryString
    property alias singleRunner: resultModel.singleRunner
    property alias runnerManager: resultModel.runnerManager
    property var queryField

    property alias singleRunnerMetaData: resultModel.singleRunnerMetaData
    property alias querying: resultModel.querying
    property alias limit: resultModel.limit
    property bool reversed
    signal activated
    signal updateQueryString(string text, int cursorPosition)

    // NOTE this also flips increment/decrementCurrentIndex (Bug 360789)
    verticalLayoutDirection: reversed ? ListView.BottomToTop : ListView.TopToBottom
    keyNavigationWraps: true
    highlight: PlasmaExtras.Highlight {
        pressed: (listView.currentItem as T.AbstractButton)?.down ?? false
    }
    highlightMoveDuration: 0
    activeFocusOnTab: true
    Accessible.role: Accessible.List

    section {
        criteria: ViewSection.FullString
        property: "category"
    }

    // This is used to keep track if the user has pressed enter before
    // the first result has been shown, in the case the first result should
    // be run when the model is populated
    property bool runAutomatically
    property int oldIndex: -1

    model: Milou.ResultsModel {
        id: resultModel
        limit: 15
        onQueryStringChangeRequested: (queryString, pos) => {
            listView.updateQueryString(queryString, pos)
        }
        onQueryStringChanged: () => {
            resetView();
            // Do not run the results automatically, if the query changed since we pressed enter
            // BUG: 459859
            listView.oldIndex = -1
            listView.runAutomatically = false;
        }
        onModelReset: resetView()

        onRowsInserted: {
            // ListView will keep the currentItem the same if new data is added. If the user hasn't interacted
            // with the results, we want the new first entry selected, so store the state to check later
            listView.oldIndex = listView.currentIndex

            if (listView.runAutomatically) {
                // This needs to be delayed as running a result may close the window and clear the query
                // having us reset the model whilst in the middle of processing the insertion.
                // The proxy model chain that comes after us really doesn't like this.
                Qt.callLater(function() {
                    resultModel.run(resultModel.index(0, 0));
                    listView.activated();
                });

                runAutomatically = false;
            }
        }

        function resetView() {
            listView.currentIndex = 0;
        }
    }

    delegate: ResultDelegate {
        id: resultDelegate
        width: listView.width
        reversed: listView.reversed
    }

    onCountChanged: {
        if (currentIndex !== oldIndex && oldIndex === 0) {
            currentIndex = oldIndex
        }
    }

    //
    // vHanda: Ideally this should have gotten handled in the delagte's onReturnPressed
    // code, but the ListView doesn't seem forward keyboard events to the delgate when
    // it is not in activeFocus. Even manually adding Keys.forwardTo: resultDelegate
    // doesn't make any difference!
    Keys.onReturnPressed: event => runCurrentIndex(event);
    Keys.onEnterPressed: event => runCurrentIndex(event);

    function runCurrentIndex(event) {
        if (!currentItem) {
            runAutomatically = true
            return;
        } else {
            // If user presses Shift+Return to invoke an action, invoke the first runner action
            if (event && event.modifiers === Qt.ShiftModifier
                    && currentItem.actions && currentItem.actions.length > 0) {
                runAction(0)
                return
            }

            if (currentItem.activeAction > -1) {
                runAction(currentItem.activeAction)
                return
            }

            if (resultModel.run(resultModel.index(currentIndex, 0))) {
                activated()
            }
            runAutomatically = false
        }
    }

    function runAction(index) {
        if (resultModel.runAction(resultModel.index(currentIndex, 0), index)) {
            activated()
        }
    }

    onActiveFocusChanged: {
        if (!activeFocus && currentIndex == listView.count-1) {
            currentIndex = 0;
        }
    }

    Keys.onTabPressed: {
        if (!currentItem || !currentItem.activateNextAction()) {
            if (reversed) {
                if (currentIndex == 0) {
                    listView.nextItemInFocusChain(false).forceActiveFocus();
                    return;
                }
                decrementCurrentIndex()
            } else {
                if (currentIndex == listView.count-1) {
                    listView.nextItemInFocusChain(true).forceActiveFocus();
                    return;
                }
                incrementCurrentIndex()
            }
        }
    }
    Keys.onBacktabPressed: {
        if (!currentItem || !currentItem.activatePreviousAction()) {
            if (reversed) {
                if (currentIndex == listView.count-1) {
                    listView.nextItemInFocusChain(true).forceActiveFocus();
                    return;
                }
                incrementCurrentIndex()
            } else {
                if (currentIndex == 0) {
                    listView.nextItemInFocusChain(false).forceActiveFocus();
                    return;
                }
                decrementCurrentIndex()
            }

            // activate previous action cannot know whether we want to back tab from an action
            // to the main result or back tab from another search result, so we explicitly highlight
            // the last action here to provide a consistent navigation experience
            if (currentItem) {
                currentItem.activateLastAction()
            }
        }
    }
    Keys.onPressed: event => navigationKeyHandler(event)

    // Moving up/down categories
    function getCategoryName(i) {
        return model.data(model.index(i, 0), Milou.ResultsModel.CategoryRole)
    }

    // Move in any direction.
    // We consume an array of all indexes (this is either spinning up [0,1,2...], or down [4,3,2...]) and iterate it
    // up to 2x using the remainder operator to wrap around. The effective end result is that can visit all indexes,
    // in order, starting from the currentIndex.
    // This helps prevent code duplication for the up/down variants. It's slightly more work to wrap your head around.
    function __move_category(event, indexes) {
        const getIndex = i => indexes[i % indexes.length]

        event.accepted = true
        const originalCategory = getCategoryName(currentIndex)
        let index = currentIndex
        for (let i = indexes.indexOf(currentIndex); i < count * 2; i++) {
            index = getIndex(i)
            const categoryChanged = originalCategory !== getCategoryName(index)
            // when spinning down (i.e. moving upwards visually) we need to consider if there are more items of the same category before the current index
            const nextIndex = getIndex(i + 1)
            const spinningDown = nextIndex < index
            const isFirstInCategory = spinningDown ? (getCategoryName(index) !== getCategoryName(nextIndex)) : true
            if (categoryChanged && isFirstInCategory) {
                break
            }
        }
        currentIndex = index
        queryField?.focus && forceActiveFocus()
    }

    function __move_category_up(event) {
        if (count === 0) {
            return
        }
        __move_category(event, [...Array(count).keys()].reverse())
    }

    function __move_category_down(event) {
        if (count === 0) {
            return
        }
        __move_category(event, [...Array(count).keys()])
    }

    function navigationKeyHandler(e) {
        const ctrl = e.modifiers & Qt.ControlModifier;
        const queryFieldPos = queryField ? queryField.cursorPosition :-1
        const handleHome = queryField ? queryField.cursorPosition === 0 || !queryField.focus : true
        const handleEnd = queryField ? queryField.cursorPosition === queryString.length || !queryField.focus : true

        if (ctrl && e.key === Qt.Key_Up || e.key === Qt.Key_PageUp) {
            queryField?.focus && forceActiveFocus();
            reversed ? __move_category_down(e) : __move_category_up(e)
            e.accepted = true;
        } else if (ctrl && e.key === Qt.Key_Down || e.key === Qt.Key_PageDown) {
            queryField?.focus && forceActiveFocus();
            reversed ? __move_category_up(e) : __move_category_down(e)
            e.accepted = true;
        } else if (e.key === Qt.Key_Up || (ctrl && e.key === Qt.Key_K)) {
            queryField?.focus && forceActiveFocus();
            reversed ? incrementCurrentIndex() : decrementCurrentIndex();
            e.accepted = true;
        } else if (e.key === Qt.Key_Down || (ctrl && e.key === Qt.Key_J)) {
            queryField?.focus && forceActiveFocus();
            reversed ? decrementCurrentIndex() : incrementCurrentIndex();
            e.accepted = true;
        } else if (e.key === Qt.Key_Home && handleHome) {
            queryField?.focus && forceActiveFocus();
            e.accepted = true;
            currentIndex = reversed ? count - 1 : 0
        } else if (e.key === Qt.Key_End && handleEnd) {
            queryField?.focus && forceActiveFocus();
            e.accepted = true;
            currentIndex = reversed ? 0 : count - 1
        } else if (e.text !== "" && queryField?.focus === false) {
            // This prevents unprintable control characters from being inserted
            if (!/[\x00-\x1F\x7F]/.test(e.text)) {
                queryField.text += e.text;
            }
            queryField.cursorPosition = queryField.text.length
            queryField.focus = true;
        }
    }

    boundsBehavior: Flickable.StopAtBounds

    function loadSettings() {
        resultModel.loadSettings()
    }

    function setQueryString(queryString) {
        resultModel.queryString = queryString
        runAutomatically = false
    }

    // Save drag data
    Item {
        id: dragHelper
        Drag.dragType: Drag.Automatic
        Drag.supportedActions: Qt.CopyAction | Qt.LinkAction
        Drag.onDragFinished: {
            Drag.mimeData = {};
            Drag.imageSource = "";
        }
    }
}
