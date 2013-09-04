
set (lib_SRCS
    resultsmodel.cpp
    resultlistmodel.cpp

    abstractsource.cpp
    match.cpp
    nepomuksource.cpp
    plasmarunnersource.cpp
    applicationsource.cpp
    sourcesmodel.cpp

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

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
