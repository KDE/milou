set( APP_SRCS
  sidebar.cpp
  mainwindow.cpp
  main.cpp
)

kde4_add_executable(milou-app ${APP_SRCS})

target_link_libraries(milou-app
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  ${NEPOMUK_WIDGETS_LIBRARY}
  ${SOPRANO_LIBRARIES}
  milou
  )

install(TARGETS milou-app
        DESTINATION ${BIN_INSTALL_DIR})
