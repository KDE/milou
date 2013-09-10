project(milou)

find_package(KDE4 REQUIRED)
find_package(NepomukCore REQUIRED)
find_package(NepomukWidgets REQUIRED)
find_package(Soprano)
find_package(SharedDesktopOntologies)

find_package(KdepimLibs)
set_package_properties(KdepimLibs PROPERTIES DESCRIPTION "KDE PIM Libraries"
                       URL "http://www.kde.org"
                       TYPE REQUIRED PURPOSE "Support for Email previews")


include(SopranoAddOntology)
include(KDE4Defaults)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
)

add_subdirectory(lib)
#add_subdirectory(app)
add_subdirectory(plasmoid)
