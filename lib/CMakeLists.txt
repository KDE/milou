
set (lib_SRCS
    abstractsource.cpp
    match.cpp
    nepomuksource.cpp
    plasmarunnersource.cpp
    applicationsource.cpp
    calculatorsource.cpp
    sourcesmodel.cpp
    preview.cpp
    previewplugin.cpp
    reversemodel.cpp

    nepomuk/asyncnepomukresourceretriever.cpp
    nepomuk/queryrunnable.cpp
)

kde4_add_library(milou SHARED ${lib_SRCS})

target_link_libraries (milou
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    ${NEPOMUK_WIDGETS_LIBRARY}
    ${KDE4_PLASMA_LIBS}
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
add_subdirectory(previews)
