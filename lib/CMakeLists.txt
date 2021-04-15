add_definitions(-DTRANSLATION_DOMAIN=\"milou\")

set (lib_SRCS
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
    KF5::KIOWidgets # for KFileItemActions
    KF5::Runner
    KF5::I18n
)

generate_export_header(milou BASE_NAME MILOU EXPORT_FILE_NAME milou_export.h)

install(TARGETS milou EXPORT MilouLibraryTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(qml)
if (BUILD_TESTING)
    add_subdirectory(test)
endif()
