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
  ${SOPRANO_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
)

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(plasmoid)
