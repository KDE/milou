project(Milou)
set(PROJECT_VERSION "5.2.90")
set(PROJECT_VERSION_MAJOR 5)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.2.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Script Qml Quick Widgets Test)
find_package(KF5 REQUIRED COMPONENTS Runner Plasma I18n Service Declarative)

include(FeatureSummary)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)

# find_package(KdepimLibs)
# set_package_properties(KdepimLibs PROPERTIES DESCRIPTION "KDE PIM Libraries"
#                        URL "http://www.kde.org"
#                        TYPE REQUIRED PURPOSE "Support for Email previews")

add_subdirectory(lib)
add_subdirectory(plasmoid)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
