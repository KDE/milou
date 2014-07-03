project(Milou)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Script Qml Widgets Test)
find_package(KF5 REQUIRED COMPONENTS Runner Plasma I18n Declarative)

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


set(MILOU_VERSION_MAJOR 4)
set(MILOU_VERSION_MINOR 98)
set(MILOU_VERSION_PATCH 0)
set(MILOU_VERSION ${MILOU_VERSION_MAJOR}.${MILOU_VERSION_MINOR}.${MILOU_VERSION_PATCH} )

set(GENERIC_LIB_SOVERSION 5)
set(GENERIC_LIB_VERSION "${MILOU_VERSION_MAJOR}.${MILOU_VERSION_MINOR}.${MILOU_VERSION_PATCH}")

add_subdirectory(lib)
add_subdirectory(plasmoid)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
