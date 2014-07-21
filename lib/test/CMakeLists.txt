include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(modeltest modeltest.cpp test.cpp)
ecm_mark_as_test(modeltest)

target_link_libraries(modeltest
  Qt5::Core
  Qt5::Test
  Qt5::Gui
  milou
)

add_executable(widgettest widgettest.cpp)
ecm_mark_as_test(widgettest)

target_link_libraries(widgettest
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  milou
)

add_executable(previewtest previewtest.cpp)
ecm_mark_as_test(previewtest)

target_link_libraries(previewtest
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  milou
)
