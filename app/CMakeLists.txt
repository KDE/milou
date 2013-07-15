set( APP_SRCS
  mainwindow.cpp
  main.cpp
)

kde4_add_executable(nepomuk-finder ${APP_SRCS})

target_link_libraries( nepomuk-finder
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  ${NEPOMUK_WIDGETS_LIBRARY}
  ${SOPRANO_LIBRARIES}
  nepomukfinder
  )

install(TARGETS nepomuk-finder
        DESTINATION ${BIN_INSTALL_DIR})
