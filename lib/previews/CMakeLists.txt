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

#
# Emails
#
# FIXME: Why does it not work without this?
find_package(KdepimLibs)
if(KdepimLibs_FOUND)
    include_directories(${KDEPIMLIBS_INCLUDE_DIRS})

    kde4_add_plugin(milouemailplugin emailplugin.cpp)

    target_link_libraries(milouemailplugin
        milou
        ${KDE4_KIO_LIBS}
        ${KDEPIMLIBS_KMIME_LIBS}
        ${KDEPIMLIBS_AKONADI_LIBS}
    )

    install(
    FILES milouemailpreview.desktop
    DESTINATION ${SERVICES_INSTALL_DIR})

    install(
    TARGETS milouemailplugin
    DESTINATION ${PLUGIN_INSTALL_DIR})
endif(KdepimLibs_FOUND)

#
# Okular
#
include_directories(${OKULAR_INCLUDE_DIRS})

kde4_add_plugin(milouokularplugin okularplugin.cpp)

target_link_libraries(milouokularplugin
    milou
    ${KDE4_KIO_LIBS}
    ${KDE4_KPARTS_LIBS}
)

install(
FILES milouokularpreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouokularplugin
DESTINATION ${PLUGIN_INSTALL_DIR})

#
# Audio
#
kde4_add_plugin(milouaudioplugin audioplugin.cpp ../nepomuk/queryrunnable.cpp)

target_link_libraries(milouaudioplugin
    milou
    ${KDE4_KIO_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${NEPOMUK_CORE_LIBRARY}
)

install(
FILES milouaudiopreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouaudioplugin
DESTINATION ${PLUGIN_INSTALL_DIR})

add_subdirectory(videos)
