find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(modeltest test.cpp)
ecm_mark_as_test(modeltest)

target_link_libraries(modeltest
  Qt::Core
  Qt::Test
  Qt::Gui
  milou
)

add_executable(widgettest widgettest.cpp)
ecm_mark_as_test(widgettest)

target_link_libraries(widgettest
  Qt::Core
  Qt::Gui
  Qt::Widgets
  milou
)
