
include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
)

set (finder_lib_SRCS
    resultsmodel.cpp
)

kde4_add_library(nepomukfinder SHARED ${finder_lib_SRCS})

target_link_libraries (nepomukfinder
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    ${NEPOMUK_WIDGETS_LIBRARY}
)

install(TARGETS nepomukfinder EXPORT NepomukFinderLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
