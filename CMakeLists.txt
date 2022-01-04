cmake_minimum_required(VERSION 3.16)

project(Milou)
set(PROJECT_VERSION "5.23.5")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.86")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n Declarative ItemModels Service Plasma Runner)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(GenerateExportHeader)
include(KDEClangFormat)
include(KDEGitCommitHooks)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050c00)

add_definitions(-DQT_NO_FOREACH -DQT_NO_KEYWORDS)

add_subdirectory(lib)
add_subdirectory(plasmoid)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
