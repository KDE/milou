project(nepomuk-finder)

find_package(KDE4 REQUIRED)
find_package(NepomukCore REQUIRED)
find_package(NepomukWidgets REQUIRED)
find_package(Soprano)
find_package(SharedDesktopOntologies)

include(SopranoAddOntology)
include(KDE4Defaults)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${CMAKE_SOURCE_DIR}
  ${SOPRANO_INCLUDE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
  )

set( SRCS
  finderapplet.cpp
)

kde4_add_plugin(plasma_applet_nepomuk_finder ${SRCS})

target_link_libraries(plasma_applet_nepomuk_finder
                       ${KDE4_PLASMA_LIBS}
                       ${KDE4_KDEUI_LIBS}
                       ${KDE4_KIO_LIBS}
                       ${QT_QTDECLARATIVE_LIBRARY}
                       ${NEPOMUK_CORE_LIBRARY}
                       ${NEPOMUK_WIDGETS_LIBRARY}
                       ${SOPRANO_LIBRARIES}
)

install(DIRECTORY declarative/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.nepomuk.finder)
install(TARGETS plasma_applet_nepomuk_finder DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR}
        RENAME plasma-applet-nepomuk-finder.desktop)

