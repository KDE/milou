set(REVERSE_TEST_SRCS reversemodeltest.cpp)

ecm_add_test(${REVERSE_TEST_SRCS}
    TEST_NAME "reversemodeltest"
    LINK_LIBRARIES Qt5::Test milou
)
