
set (plugin_SRCS
    qmlplugins.cpp
)

add_library (milouqmlplugin SHARED ${plugin_SRCS})

target_link_libraries (milouqmlplugin
    ${QT_QTSCRIPT_LIBRARY}
    Qt5::Declarative
    
    ${KDE4_KDEUI_LIBS}
    milou
)

install (TARGETS milouqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/milou)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/milou)
