add_definitions(-DTRANSLATION_DOMAIN=\"milou\")
add_library(milou STATIC)

set_property(TARGET milou PROPERTY POSITION_INDEPENDENT_CODE ON)
target_sources(milou PRIVATE
    resultsmodel.cpp
    mousehelper.cpp
    resultsmodel.h
    mousehelper.h
)

target_link_libraries(milou
    Qt::Qml
    Qt::Quick
    Qt::Widgets
    KF6::Runner
    KF6::ConfigCore
)

add_subdirectory(qml)
