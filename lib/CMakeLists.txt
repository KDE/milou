
include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
)


set (finder_plugin_SRCS
    resultsmodel.cpp
    qmlplugins.cpp
)

kde4_add_library (nepomukfinderqmlplugin SHARED ${finder_plugin_SRCS})

target_link_libraries (nepomukfinderqmlplugin
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    ${NEPOMUK_WIDGETS_LIBRARY}
)

install (TARGETS nepomukfinderqmlplugin DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/nepomuk)
install (FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/nepomuk)
