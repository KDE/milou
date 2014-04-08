
set (lib_SRCS
    abstractsource.cpp
    match.cpp
    context.cpp
    preview.cpp
    previewplugin.cpp
    sourcesmodel.cpp
    reversemodel.cpp
    baloorunnable.cpp

    sources/baloosource.cpp
    sources/plasmarunnersource.cpp
    sources/applicationsource.cpp
    sources/calculatorsource.cpp
)

kde4_add_library(milou SHARED ${lib_SRCS})

set_target_properties(milou PROPERTIES VERSION 0.1 SOVERSION 0.1)

target_link_libraries (milou
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${BALOO_LIBRARIES}
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
add_subdirectory(previews)
add_subdirectory(test)
