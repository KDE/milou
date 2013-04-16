project(nepomuk-finder)

find_package(KDE4 REQUIRED)
find_package(NepomukCore REQUIRED)
find_package(NepomukWidgets REQUIRED)
find_package(Soprano)
find_package(SharedDesktopOntologies)

include(SopranoAddOntology)
include(KDE4Defaults)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${CMAKE_SOURCE_DIR}
  ${SOPRANO_INCLUDE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
  )

set( SRCS
  main.cpp
)

kde4_add_executable(nepomuk-finder ${SRCS})

target_link_libraries( nepomuk-finder
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  ${NEPOMUK_WIDGETS_LIBRARY}
  ${SOPRANO_LIBRARIES}
  )

install(TARGETS nepomuk-finder
        DESTINATION ${BIN_INSTALL_DIR})
