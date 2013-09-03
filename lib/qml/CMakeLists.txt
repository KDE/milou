
set (plugin_SRCS
    qmlplugins.cpp
)

kde4_add_library (milouqmlplugin SHARED ${plugin_SRCS})

target_link_libraries (milouqmlplugin
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    ${NEPOMUK_WIDGETS_LIBRARY}
    milou
)

install (TARGETS milouqmlplugin DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/milou)
install (FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/milou)
