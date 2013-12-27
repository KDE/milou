
set (lib_SRCS
    abstractsource.cpp
    match.cpp
    context.cpp
    baloosource.cpp
    baloorunnable.cpp
    plasmarunnersource.cpp
    applicationsource.cpp
    calculatorsource.cpp
    sourcesmodel.cpp
    preview.cpp
    previewplugin.cpp
    reversemodel.cpp
)

kde4_add_library(milou SHARED ${lib_SRCS})

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
