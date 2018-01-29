add_library(milouvideoplugin MODULE videoplugin.cpp)

target_link_libraries(milouvideoplugin
    milou
    KF5::KIOWidgets
    Qt5::Declarative
    ${PHONON_LIBS}
)

install(
FILES milouvideopreview.desktop
DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

install(
TARGETS milouvideoplugin
DESTINATION ${KDE_INSTALL_PLUGINDIR})
