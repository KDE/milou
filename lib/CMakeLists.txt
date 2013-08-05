
set (finder_lib_SRCS
    queryrunnable.cpp
    resultsmodel.cpp
    resultlistmodel.cpp

    abstractsource.cpp
    nepomuksource.cpp
    plasmarunnersource.cpp
    sourcesmodel.cpp
)

kde4_add_library(nepomukfinder SHARED ${finder_lib_SRCS})

target_link_libraries (nepomukfinder
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    ${NEPOMUK_WIDGETS_LIBRARY}
    ${KDE4_PLASMA_LIBS}
)

install(TARGETS nepomukfinder EXPORT NepomukFinderLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
