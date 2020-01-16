cmake_minimum_required(VERSION 3.0)

project(Milou)
set(PROJECT_VERSION "5.17.90")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.12.0")
set(KF5_MIN_VERSION "5.66.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Widgets Test)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n Declarative ItemModels Service Plasma Runner)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(GenerateExportHeader)
include(KDEClangFormat)
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
endif()
# find_package(KdepimLibs)
# set_package_properties(KdepimLibs PROPERTIES DESCRIPTION "KDE PIM Libraries"
#                        URL "http://www.kde.org"
#                        TYPE REQUIRED PURPOSE "Support for Email previews")

add_subdirectory(lib)
add_subdirectory(plasmoid)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
