set(applet_SRCS
    applet.cpp
    configwidget.cpp
)

add_library(milou_applet MODULE ${applet_SRCS})
target_link_libraries(milou_applet
    ${KDE4_KDEUI_LIBS}
    KF5::Plasma
    Qt5::Declarative
)

install(TARGETS milou_applet DESTINATION ${PLUGIN_INSTALL_DIR})

# QML Files
install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.milou)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR}
        RENAME plasma-applet-milou.desktop)


