
set (lib_SRCS
#     preview.cpp
#     previewplugin.cpp
    sourcesmodel.cpp
)

add_library(milou SHARED ${lib_SRCS})

target_link_libraries (milou
    KF5::KIOWidgets
    KF5::Plasma
    KF5::Runner
    ${BALOO_CORE_LIBRARY}
)

install(
  FILES miloupreviewplugin.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR})

generate_export_header(milou BASE_NAME MILOU EXPORT_FILE_NAME milou_export.h)

install(TARGETS milou EXPORT MilouLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qml)
# add_subdirectory(previews)
add_subdirectory(test)
