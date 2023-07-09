# SPDX-FileCopyrightText: 2023 Alexander Lohnau <alexander.lohnau@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(milouqmlplugin URI "org.kde.milou" CLASSNAME MilouQmlPlugin)
target_sources(milouqmlplugin PRIVATE plugin.cpp)
target_link_libraries (milouqmlplugin Qt::Qml milou)
ecm_target_qml_sources(milouqmlplugin SOURCES
    ResultDelegate.qml ResultsView.qml
    ResultsListViewDelegate.qml ResultsListView.qml
    VERSION 0.1
)
ecm_finalize_qml_module(milouqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR})
