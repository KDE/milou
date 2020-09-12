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
