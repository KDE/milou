/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1

import org.kde.milou 0.1 as Milou

Milou.ResultsView {
    id: listView

    width: 800
    height: 1100

    Component.onCompleted: {
        timer.start()
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        onTriggered: {
            var date = new Date()
            date.setYear(Math.random())
            listView.queryString = Qt.formatDateTime(date, "dddd")
            console.log(listView.queryString)
        }
    }
}
