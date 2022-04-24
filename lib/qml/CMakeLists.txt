
set (plugin_SRCS
    qmlplugins.cpp
    qmlplugins.h
)

add_library (milouqmlplugin SHARED ${plugin_SRCS})

target_link_libraries (milouqmlplugin
    Qt::Qml

    milou
)

install (TARGETS milouqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/milou)
install (FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/milou)
install (FILES ResultDelegate.qml ResultsView.qml globals.js
               ResultsListViewDelegate.qml ResultsListView.qml
         DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/milou)
