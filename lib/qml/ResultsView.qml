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
import org.kde.kirigami 2.20 as Kirigami
import org.kde.milou 0.3 as Milou

ListView {
    id: listView
    property alias queryString: resultModel.queryString
    property alias singleRunner: resultModel.singleRunner
    property alias runnerManager: resultModel.runnerManager

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

    // This is used to disable mouse selection if the user interacts only with keyboard
    property bool moved: false
    property point savedMousePosition: Milou.MouseHelper.globalMousePosition()
    function mouseMovedGlobally() {
        return savedMousePosition != Milou.MouseHelper.globalMousePosition();
    }

    model: Milou.ResultsModel {
        id: resultModel
        limit: 15
        onQueryStringChangeRequested: {
            listView.updateQueryString(queryString, pos)
        }
        onQueryStringChanged: () => {
            resetView();
            // Do not run the results automatically, if the query changed since we pressed enter
            // BUG: 459859
            runAutomatically = false;
        }
        onModelReset: resetView()

        onRowsInserted: {
            // Keep the selection at the top as items inserted to the beginning will shift it downwards
            // ListView will update its view after this signal is processed and then our callLater will set it back
            if (listView.currentIndex === 0) {
                Qt.callLater(function() {
                    listView.currentIndex = 0;
                });
            }

            if (runAutomatically) {
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
            listView.moved = false;
            listView.savedMousePosition = Milou.MouseHelper.globalMousePosition();
        }
    }

    delegate: ResultDelegate {
        id: resultDelegate
        width: listView.width
        reversed: listView.reversed
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
                    && currentItem.additionalActions && currentItem.additionalActions.length > 0) {
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
    Keys.onPressed: event => navigationKeyHandler(event, true)

    // Moving up/down categories
    function getCategoryName(i) {
        return results.model.data(results.model.index(i, 0), Milou.ResultsModel.CategoryRole)
    }
    function __move_category_up(event) {
        event.accepted = true;
        const originalCategory = getCategoryName(results.currentIndex);
        const originalIdx = results.currentIndex;;
        let idx = results.currentIndex;
        while (originalCategory === getCategoryName(idx)
            || getCategoryName(idx) === getCategoryName(idx - 1)
        ) {
            idx--;
            if (idx < 0) { // IF we are at the top and want to go to the previous category, we have to check from the bottom
                idx = results.count -1;
            }
            if (idx === originalIdx) {
                return; // Avoid endless loop if we only have one category
            }
        } 
        results.currentIndex = idx;
        queryField.focus && results.forceActiveFocus();
    }

    function __move_category_down(event) {
        event.accepted = true;
        const originalCategory = getCategoryName(results.currentIndex);
        let idx = results.currentIndex;
        while (originalCategory === getCategoryName(idx)) {
            idx++;
            if (idx === results.count) {
                idx = 0; // The first item is always the first item if it's category'
                break;
            }
        } 
        results.currentIndex = idx;
        queryField.focus && results.forceActiveFocus();
    }


    function navigationKeyHandler(e, handleHomeAndEnd = false) {
        const ctrl = e.modifiers & Qt.ControlModifier;
        if (ctrl && e.key === Qt.Key_Up) {
            reversed ? __move_category_down(e) : __move_category_up(e)
            e.accepted = true;
        } else if (ctrl && e.key === Qt.Key_Down) {
            reversed ? __move_category_up(e) : __move_category_down(e)
            e.accepted = true;
        } else if (e.key === Qt.Key_Up || (ctrl && e.key === Qt.Key_K)) {
            reversed ? incrementCurrentIndex() : decrementCurrentIndex();
            e.accepted = true;
        } else if (e.key === Qt.Key_Down || (ctrl && e.key === Qt.Key_J)) {
            reversed ? decrementCurrentIndex() : incrementCurrentIndex();
            e.accepted = true;
        } else if ((e.key === Qt.Key_Home && handleHomeAndEnd)|| e.key === Qt.Key_PageUp) {
            e.accepted = true;
            results.currentIndex = reversed ? results.count - 1 : 0
        } else if ((e.key === Qt.Key_End && handleHomeAndEnd) || e.key === Qt.Key_PageDown) {
            e.accepted = true;
            results.currentIndex = reversed ? 0 : results.count - 1
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
