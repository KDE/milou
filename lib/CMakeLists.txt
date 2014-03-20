
set (lib_SRCS
    abstractsource.cpp
    match.cpp
    context.cpp
#     preview.cpp
#     previewplugin.cpp
    sourcesmodel.cpp
    reversemodel.cpp
    baloorunnable.cpp

    sources/baloosource.cpp
    sources/plasmarunnersource.cpp
    sources/applicationsource.cpp
    sources/calculatorsource.cpp
)

add_library(milou SHARED ${lib_SRCS})

target_link_libraries (milou
    Qt5::Script #this is used for the calculator

    KF5::KIOWidgets
    KF5::Plasma
    KF5::Runner
    KF5::KDE4Support
    ${BALOO_CORE_LIBRARY}
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
# add_subdirectory(previews)
add_subdirectory(test)
