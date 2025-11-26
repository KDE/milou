add_definitions(-DTRANSLATION_DOMAIN=\"milou\")

ecm_add_qml_module(milou URI "org.kde.milou" GENERATE_PLUGIN_SOURCE)

target_sources(milou PRIVATE
    resultsmodel.cpp
    mousehelper.cpp
    resultsmodel.h
    mousehelper.h
)

ecm_target_qml_sources(milou SOURCES
    ResultDelegate.qml
    ResultsView.qml
)

target_link_libraries (milou PRIVATE Qt::Gui Qt::Qml KF6::ConfigCore KF6::Runner)

ecm_finalize_qml_module(milou DESTINATION ${KDE_INSTALL_QMLDIR})
