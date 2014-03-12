#
# Plain Text
#
add_library(miloutextplugin MODULE textplugin.cpp)

target_link_libraries(miloutextplugin
    milou
    KF5::KIOWidgets
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
add_library(milouimageplugin MODULE imageplugin.cpp)

target_link_libraries(milouimageplugin
    milou
    KF5::KIOWidgets
    Qt5::Declarative
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
if(KdepimLibs_FOUND)
#     include_directories(${KDEPIMLIBS_INCLUDE_DIRS})
#
#     add_library(milouemailplugin MODULE emailplugin.cpp)
#
#     target_link_libraries(milouemailplugin
#         milou
#         KF5::KIOWidgets
#         ${KDEPIMLIBS_KMIME_LIBS}
#         ${KDEPIMLIBS_AKONADI_LIBS}
#     )
#
#     install(
#     FILES milouemailpreview.desktop
#     DESTINATION ${SERVICES_INSTALL_DIR})
#
#     install(
#     TARGETS milouemailplugin
#     DESTINATION ${PLUGIN_INSTALL_DIR})
endif()

#
# Okular
#
#include_directories(${OKULAR_INCLUDE_DIRS})
#
#add_library(milouokularplugin MODULE okularplugin.cpp)
#
#target_link_libraries(milouokularplugin
#    milou
#    KF5::KIOWidgets
#    KF5::Parts
#)
#
#install(
#FILES milouokularpreview.desktop
#DESTINATION ${SERVICES_INSTALL_DIR})
#
#install(
#TARGETS milouokularplugin
#DESTINATION ${PLUGIN_INSTALL_DIR})

#
# Audio
#
add_library(milouaudioplugin MODULE audioplugin.cpp)

target_link_libraries(milouaudioplugin
    milou
    KF5::KIOWidgets
    Qt5::Declarative
    ${BALOO_CORE_LIBRARY} ${BALOO_FILE_LIBRARY}
)

install(
FILES milouaudiopreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouaudioplugin
DESTINATION ${PLUGIN_INSTALL_DIR})

#
# General Files
#
add_library(miloufileplugin MODULE fileplugin.cpp)

target_link_libraries(miloufileplugin
    milou
    KF5::KIOWidgets
    Qt5::Declarative
    ${BALOO_CORE_LIBRARY} ${BALOO_FILE_LIBRARY}
)

install(
FILES miloufilepreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS miloufileplugin
DESTINATION ${PLUGIN_INSTALL_DIR})

#
# Applications
#
add_library(milouapplicationplugin MODULE applicationplugin.cpp)

target_link_libraries(milouapplicationplugin
    milou
    KF5::KIOWidgets
    Qt5::Declarative
)

install(
FILES milouapplicationpreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouapplicationplugin
DESTINATION ${PLUGIN_INSTALL_DIR})

#
# Bookmarks
#
add_library(miloubookmarkplugin MODULE bookmarkplugin.cpp)

target_link_libraries(miloubookmarkplugin
    milou
    KF5::KIOWidgets
    Qt5::WebKitWidgets
)

install(
FILES miloubookmarkpreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS miloubookmarkplugin
DESTINATION ${PLUGIN_INSTALL_DIR})
