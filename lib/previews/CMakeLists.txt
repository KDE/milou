#
# Plain Text
#
kde4_add_plugin(miloutextplugin textplugin.cpp)

target_link_libraries(miloutextplugin
    milou
    ${KDE4_KIO_LIBS}
)

install(
FILES miloutextpreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS miloutextplugin
DESTINATION ${PLUGIN_INSTALL_DIR})

#
# Images
#
kde4_add_plugin(milouimageplugin imageplugin.cpp)

target_link_libraries(milouimageplugin
    milou
    ${KDE4_KIO_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
)

install(
FILES milouimagepreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouimageplugin
DESTINATION ${PLUGIN_INSTALL_DIR})
