#
# Plain Text
#
add_library(miloutextplugin MODULE textplugin.cpp)

target_link_libraries(miloutextplugin
    milou
)

install(
FILES miloutextpreview.desktop
DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

install(
TARGETS miloutextplugin
DESTINATION ${KDE_INSTALL_PLUGINDIR})

#
# Images
#
#add_library(milouimageplugin MODULE imageplugin.cpp)
#
#target_link_libraries(milouimageplugin
#    milou
#    KF5::KIOWidgets
#    Qt5::Declarative
#)
#
#install(
#FILES milouimagepreview.desktop
#DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
#install(
#TARGETS milouimageplugin
#DESTINATION ${KDE_INSTALL_PLUGINDIR})

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
#     DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
#     install(
#     TARGETS milouemailplugin
#     DESTINATION ${KDE_INSTALL_PLUGINDIR})
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
#DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
#install(
#TARGETS milouokularplugin
#DESTINATION ${KDE_INSTALL_PLUGINDIR})

#
# Audio
#
# add_library(milouaudioplugin MODULE audioplugin.cpp)
#
# target_link_libraries(milouaudioplugin
#     milou
#     KF5::KIOWidgets
#     Qt5::Declarative
#     ${BALOO_CORE_LIBRARY} ${BALOO_FILE_LIBRARY}
# )
#
# install(
# FILES milouaudiopreview.desktop
# DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
# install(
# TARGETS milouaudioplugin
# DESTINATION ${KDE_INSTALL_PLUGINDIR})
#
# #
# # General Files
# #
# add_library(miloufileplugin MODULE fileplugin.cpp)
#
# target_link_libraries(miloufileplugin
#     milou
#     KF5::KIOWidgets
#     Qt5::Declarative
#     ${BALOO_CORE_LIBRARY} ${BALOO_FILE_LIBRARY}
# )
#
# install(
# FILES miloufilepreview.desktop
# DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
# install(
# TARGETS miloufileplugin
# DESTINATION ${KDE_INSTALL_PLUGINDIR})
#
# #
# # Applications
# #
# add_library(milouapplicationplugin MODULE applicationplugin.cpp)
#
# target_link_libraries(milouapplicationplugin
#     milou
#     KF5::KIOWidgets
#     Qt5::Declarative
# )
#
# install(
# FILES milouapplicationpreview.desktop
# DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
# install(
# TARGETS milouapplicationplugin
# DESTINATION ${KDE_INSTALL_PLUGINDIR})
#
# #
# # Bookmarks
# #
# add_library(miloubookmarkplugin MODULE bookmarkplugin.cpp)
#
# target_link_libraries(miloubookmarkplugin
#     milou
#     KF5::KIOWidgets
#     Qt5::WebKitWidgets
# )
#
# install(
# FILES miloubookmarkpreview.desktop
# DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
# install(
# TARGETS miloubookmarkplugin
# DESTINATION ${KDE_INSTALL_PLUGINDIR})
