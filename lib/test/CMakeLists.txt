include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(modeltest modeltest.cpp test.cpp)

target_link_libraries(modeltest
  Qt5::Core
  Qt5::Test
  Qt5::Gui

  KF5::KDE4Support
  milou
)

add_executable(widgettest widgettest.cpp)

target_link_libraries(widgettest
  Qt5::Core
  Qt5::Gui
  KF5::KDE4Support
  milou
)
