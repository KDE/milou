
set (lib_SRCS
#     preview.cpp
#     previewplugin.cpp
    sourcesmodel.cpp
)

add_library(milou SHARED ${lib_SRCS})
set_target_properties(milou PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )

set_target_properties(milou PROPERTIES VERSION 0.1 SOVERSION 0.1)

target_link_libraries (milou
    KF5::KIOWidgets
    KF5::Plasma
    KF5::Runner
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR})

generate_export_header(milou BASE_NAME MILOU EXPORT_FILE_NAME milou_export.h)

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(qml)
# add_subdirectory(previews)
add_subdirectory(test)
