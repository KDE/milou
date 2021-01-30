add_definitions(-DTRANSLATION_DOMAIN=\"milou\")

if(NOT CMAKE_VERSION VERSION_LESS "3.10.0")
    # CMake 3.9+ warns about automoc on files without Q_OBJECT, and doesn't know about other macros.
    # 3.10+ lets us provide more macro names that require automoc.
    list(APPEND CMAKE_AUTOMOC_MACRO_NAMES "MILOU_EXPORT_PREVIEW")
endif()

set (lib_SRCS
    preview.cpp
    previewplugin.cpp
    resultsmodel.cpp
    runnerresultsmodel.cpp
    sourcesmodel.cpp
    draghelper.cpp
    mousehelper.cpp
)

add_library(milou SHARED ${lib_SRCS})
set_target_properties(milou PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR} )

target_link_libraries (milou
    Qt::Qml
    Qt::Quick
    Qt::Widgets # for QAction...
    KF5::ItemModels
    KF5::Service
    KF5::Plasma
    KF5::Runner
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})

generate_export_header(milou BASE_NAME MILOU EXPORT_FILE_NAME milou_export.h)

install(TARGETS milou EXPORT MilouLibraryTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(qml)
add_subdirectory(previews)
if (BUILD_TESTING)
    add_subdirectory(test)
endif()
