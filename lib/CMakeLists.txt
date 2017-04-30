add_definitions(-DTRANSLATION_DOMAIN=\"milou\")

set (lib_SRCS
    preview.cpp
    previewplugin.cpp
    sourcesmodel.cpp
    draghelper.cpp
    mousehelper.cpp
)

add_library(milou SHARED ${lib_SRCS})
set_target_properties(milou PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR} )

target_link_libraries (milou
    Qt5::Qml
    Qt5::Quick
    Qt5::Widgets # for QAction...
    KF5::Service
    KF5::Plasma
    KF5::Runner
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR})

generate_export_header(milou BASE_NAME MILOU EXPORT_FILE_NAME milou_export.h)

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(qml)
add_subdirectory(previews)
add_subdirectory(test)
