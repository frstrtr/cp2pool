include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(DEVCORE_TEST_SOURCE addrStore_test.cpp)


add_executable(devcore_test ${DEVCORE_TEST_SOURCE})
set_target_properties(devcore_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_link_libraries(devcore_test gtest gtest_main)
target_link_libraries(devcore_test devcore)