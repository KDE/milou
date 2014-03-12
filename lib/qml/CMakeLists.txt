
set (plugin_SRCS
    qmlplugins.cpp
)

add_library (milouqmlplugin SHARED ${plugin_SRCS})

target_link_libraries (milouqmlplugin
    Qt5::Qml
    
    milou
)

install (TARGETS milouqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/milou)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/milou)
