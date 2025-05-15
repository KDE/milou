cmake_minimum_required(VERSION 3.16)

project(Milou)
set(PROJECT_VERSION "6.3.90")

set(PROJECT_DEP_VERSION "6.3.90")
set(QT_MIN_VERSION "6.8.0")
set(KF6_MIN_VERSION "6.14.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(GenerateExportHeader)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMQmlModule)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Widgets)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS I18n Runner Svg Config)
find_package(Plasma ${PROJECT_DEP_VERSION} REQUIRED)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)

add_definitions(-DQT_NO_FOREACH -DQT_NO_KEYWORDS)

add_subdirectory(lib)
add_subdirectory(plasmoid)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
