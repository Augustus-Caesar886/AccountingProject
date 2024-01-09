CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
ADD_SUBDIRECTORY(googletest)

ADD_EXECUTABLE(AccountingTests
    DateTests.cpp
    ../src/Date.cpp
    PeriodTests.cpp
    ../src/Period.cpp
)

target_link_libraries(AccountingTests gmock gtest gtest_main)