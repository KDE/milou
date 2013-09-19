kde4_add_plugin(milouvideoplugin videoplugin.cpp)

target_link_libraries(milouvideoplugin
    milou
    ${KDE4_KIO_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${PHONON_LIBS}
)

install(
FILES milouvideopreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouvideoplugin
DESTINATION ${PLUGIN_INSTALL_DIR})
