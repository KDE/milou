/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 * SPDX-FileCopyrightText: 2015-2016 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick 2.15
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 3.0 as PlasmaComponents3

MouseArea {
    id: resultDelegate

    property variant theModel: model
    property bool reversed: false

    readonly property bool isCurrent: ListView.isCurrentItem // cannot properly Connect {} to this
    readonly property bool sectionHasChanged: (reversed && ListView.section != ListView.nextSection)
                                           || (!reversed && ListView.section != ListView.previousSection)

    property int activeAction: -1

    property string typeText: sectionHasChanged ? ListView.section : ""
    property var additionalActions: typeof actions !== "undefined" ? actions : []
    property int categoryWidth: units.gridUnit * 10

    Accessible.role: Accessible.ListItem
    Accessible.name: displayLabel.text
    Accessible.description: {
        var section = ListView.section;
        if (!section) {
            return "";
        }
        var subtext = subtextLabel.text;
        if (subtext.length > 0) {
            return i18nd("milou", "%1, in category %2", subtext, section);
        } else {
            return i18nd("milou", "in category %1", section);
        }
    }

    property bool __pressed: false
    property int __pressX: -1
    property int __pressY: -1

    onIsCurrentChanged: {
        if (!isCurrent) {
            activeAction = -1
        }
    }

    function activateNextAction() {
        if (activeAction === actionsRepeater.count - 1) { // last action, do nothing
            return false
        }
        ++activeAction
        return true
    }

    function activatePreviousAction() {
        if (activeAction < 0) { // no action, do nothing
            return false
        }
        --activeAction
        return true
    }

    function activateLastAction() {
        activeAction = actionsRepeater.count - 1
    }

    width: listItem.implicitWidth
    height: listItem.implicitHeight

    acceptedButtons: Qt.LeftButton
    hoverEnabled: true
    onPressed: mouse => {
        __pressed = true;
        __pressX = mouse.x;
        __pressY = mouse.y;
    }

    onReleased: {
        if (__pressed) {
            listView.currentIndex = model.index
            listView.runCurrentIndex()
        }

        __pressed = false;
        __pressX = -1;
        __pressY = -1;
    }

    onPositionChanged: {
        if (__pressX != -1 && typeof dragHelper !== "undefined" && dragHelper.isDrag(__pressX, __pressY, mouse.x, mouse.y)) {
            var resultsModel = ListView.view.model;
            var mimeData = resultsModel.getMimeData(resultsModel.index(index, 0));
            if (mimeData) {
                dragHelper.startDrag(resultDelegate, mimeData, model.decoration);
                __pressed = false;
                __pressX = -1;
                __pressY = -1;
            }
        }

        if (!listView.moved && listView.mouseMovedGlobally()) {
            listView.moved = true
            listView.currentIndex = index
        }
    }

    onContainsMouseChanged: {
        if (!containsMouse) {
            __pressed = false;
            __pressX = -1;
            __pressY = -1;
        } else {
            // In case we display the history we have a QML ListView which does not have the moved property
            if (!listView.hasOwnProperty("moved") || listView.moved) {
                listView.currentIndex = index
            } else if (listView.mouseMovedGlobally()) {
                listView.moved = true
                listView.currentIndex = index
            }
        }
    }

    PlasmaComponents3.Label {
        id: typeText
        text: resultDelegate.typeText
        color: isCurrent ? Qt.tint(theme.disabledTextColor, Qt.rgba(theme.textColor.r, theme.textColor.g, theme.textColor.b, 0.4)) : theme.disabledTextColor

        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        textFormat: Text.PlainText

        width: resultDelegate.categoryWidth - units.largeSpacing
        anchors {
            left: parent.left
            verticalCenter: listItem.verticalCenter
        }
    }

    PlasmaExtras.ListItem {
        id: listItem

        readonly property int indexModifier: reversed ? 0 : 1

        // fake pressed look
        checked: resultDelegate.pressed
        separatorVisible: resultDelegate.sectionHasChanged
                       && !resultDelegate.isCurrent
                       && (index === 0 || resultDelegate.ListView.view.currentIndex !== (index - indexModifier))

        implicitHeight: labelWrapper.implicitHeight + Kirigami.Units.mediumSpacing * 2

        Item {
            id: labelWrapper
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: resultDelegate.categoryWidth
            }
            implicitHeight: labelLayout.implicitHeight

            RowLayout {
                id: labelLayout

                anchors {
                    left: parent.left
                    right: actionsRow.left
                    rightMargin: units.smallSpacing
                    verticalCenter: parent.verticalCenter
                }

                Kirigami.Icon {
                    id: typePixmap
                    Layout.preferredWidth: units.iconSizes.smallMedium
                    Layout.preferredHeight: units.iconSizes.smallMedium
                    Layout.fillHeight: true
                    source: model.decoration
                    animated: false
                }

                PlasmaComponents3.Label {
                    id: displayLabel
                    text: String(typeof modelData !== "undefined" ? modelData : model.display)

                    elide: Text.ElideMiddle
                    wrapMode: model.multiLine ? Text.WordWrap : Text.NoWrap
                    maximumLineCount: model.multiLine ? Infinity : 1
                    verticalAlignment: Text.AlignVCenter
                    // For multiLine we offer styled text, otherwise we default to plain text
                    textFormat: model.multiLine ? Text.StyledText : Text.PlainText
                    // The extra spacing accounts for the right margin so the text doesn't overlap the actions
                    Layout.maximumWidth: labelWrapper.width - typePixmap.width - actionsRow.width - 2 * Kirigami.Units.smallSpacing

                    PlasmaComponents3.ToolTip {
                        text: displayLabel.text
                        visible: displayLabelHoverHandler.hovered && displayLabel.truncated
                        timeout: -1
                    }

                    HoverHandler {
                        id: displayLabelHoverHandler
                    }
                }

                PlasmaComponents3.Label {
                    id: subtextLabel

                    // SourcesModel returns number of duplicates in this property
                    // ResultsModel just has it as a boolean as you would expect from the name of the property
                    text: model.isDuplicate === true || model.isDuplicate > 1 || resultDelegate.isCurrent ? String(model.subtext || "") : ""

                    // HACK If displayLabel is too long it will shift this label outside boundaries
                    // but still render the text leading to it overlapping the action buttons looking horrible
                    opacity: width > 0 ? 1 : 0

                    color: isCurrent ? Qt.tint(theme.disabledTextColor, Qt.rgba(theme.textColor.r, theme.textColor.g, theme.textColor.b, 0.4)) : theme.disabledTextColor

                    elide: Text.ElideMiddle
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                    verticalAlignment: Text.AlignVCenter
                    textFormat: Text.PlainText

                    Layout.fillWidth: true
                    PlasmaComponents3.ToolTip {
                        text: subtextLabel.text
                        visible: subtextLabelHoverHandler.hovered && subtextLabel.truncated
                        timeout: -1
                    }

                    HoverHandler {
                        id: subtextLabelHoverHandler
                    }
                }
            }

            Row {
                id: actionsRow
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: resultDelegate.isCurrent

                Repeater {
                    id: actionsRepeater
                    model: resultDelegate.additionalActions

                    PlasmaComponents3.ToolButton {
                        width: height
                        height: Kirigami.Units.iconSizes.medium
                        visible: modelData.visible || true
                        enabled: modelData.enabled || true

                        Accessible.role: Accessible.Button
                        Accessible.name: modelData.text
                        checkable: checked
                        checked: resultDelegate.activeAction === index
                        focus: resultDelegate.activeAction === index

                        Kirigami.Icon {
                            anchors.centerIn: parent
                            implicitWidth: units.iconSizes.smallMedium
                            implicitHeight: units.iconSizes.smallMedium
                            // ToolButton cannot cope with QIcon
                            source: modelData.iconSource || ""
                            active: parent.hovered || parent.checked
                        }

                        PlasmaComponents3.ToolTip {
                            text: {
                                var text = modelData.text || ""
                                if (index === 0) { // Shift+Return will invoke first action
                                    text = i18ndc("milou", "placeholder is action e.g. run in terminal, in parenthesis is shortcut", "%1 (Shift+Return)", text)
                                }
                                return text
                            }
                        }

                        onClicked: resultDelegate.ListView.view.runAction(index)
                    }
                }
            }
        }
    }
}
