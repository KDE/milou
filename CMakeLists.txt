project(milou)

find_package(KDE4 REQUIRED)
find_package(Baloo REQUIRED)
find_package(KFileMetaData REQUIRED)

find_package(KdepimLibs)
set_package_properties(KdepimLibs PROPERTIES DESCRIPTION "KDE PIM Libraries"
                       URL "http://www.kde.org"
                       TYPE REQUIRED PURPOSE "Support for Email previews")


include(KDE4Defaults)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${BALOO_INCLUDE_DIR}
  ${KFILEMETADATA_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
)

add_subdirectory(lib)
add_subdirectory(plasmoid)
