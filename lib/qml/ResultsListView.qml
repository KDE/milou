/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2014 Vishesh Handa <vhanda@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.milou 0.1 as Milou

ResultsView {
    id: listView
    delegate: ResultsListViewDelegate {
        id: resultDelegate
        width: listView.width
    }
}
