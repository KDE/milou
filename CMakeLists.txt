project(Milou)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Widgets Xml Test Sql)
find_package(KF5 REQUIRED NO_MODULE COMPONENTS KDE4Support I18n Declarative)
find_package(Baloo REQUIRED)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

# find_package(KdepimLibs)
# set_package_properties(KdepimLibs PROPERTIES DESCRIPTION "KDE PIM Libraries"
#                        URL "http://www.kde.org"
#                        TYPE REQUIRED PURPOSE "Support for Email previews")

add_subdirectory(lib)
add_subdirectory(plasmoid)
#add_subdirectory(app)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
