add_library(milouvideoplugin MODULE videoplugin.cpp)

target_link_libraries(milouvideoplugin
    milou
    KF5::KIOWidgets
    Qt5::Declarative
    ${PHONON_LIBS}
)

install(
FILES milouvideopreview.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

install(
TARGETS milouvideoplugin
DESTINATION ${PLUGIN_INSTALL_DIR})
