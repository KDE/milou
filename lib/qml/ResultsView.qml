/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.milou 0.1 as Milou

import "globals.js" as Globals

ListView {
    id: listView
    property alias queryString: resultModel.queryString
    property alias runner: resultModel.runner
    property bool reversed
    signal activated

    verticalLayoutDirection: reversed ? ListView.BottomToTop : ListView.TopToBottom

    highlight: PlasmaComponents.Highlight {}
    highlightMoveDuration: 0

    // This is used to keep track if the user has pressed enter before
    // the first result has been shown, in the case the first result should
    // be run when the model is populated
    property bool runAutomatically

    model: Milou.SourcesModel {
        id: resultModel
        queryLimit: 20

        // Internally when the query string changes, the model is reset
        // and the results are presented
        onModelReset: {
            listView.currentIndex = 0

            if (runAutomatically) {
                runCurrentIndex();
                runAutomatically = false
            }
        }
    }

    delegate: ResultDelegate {
        id: resultDelegate
        width: listView.width
    }

    //
    // vHanda: Ideally this should have gotten handled in the delagte's onReturnPressed
    // code, but the ListView doesn't seem forward keyboard events to the delgate when
    // it is not in activeFocus. Even manually adding Keys.forwardTo: resultDelegate
    // doesn't make any difference!
    Keys.onReturnPressed: {
        if (!currentIndex) {
            runAutomatically = true
        }
        runCurrentIndex();
    }
    Keys.onEnterPressed: {
        if (!currentIndex) {
            runAutomatically = true
        }
        runCurrentIndex();
    }

    function runCurrentIndex() {
        listView.model.run(currentIndex);
        //clearPreview();
        activated()
    }

    Keys.onTabPressed: incrementCurrentIndex()
    Keys.onBacktabPressed: decrementCurrentIndex()
    Keys.onUpPressed: decrementCurrentIndex();
    Keys.onDownPressed: incrementCurrentIndex();

    boundsBehavior: Flickable.StopAtBounds

    function loadSettings() {
        resultModel.loadSettings()
    }

    function setQueryString(string) {
        resultModel.queryString = string
        runAutomatically = false
    }
}
