CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(AccountingProject)

set(CMAKE_CXX_STANDARD 17)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

ADD_EXECUTABLE(AccountingProject 
    src/main.cpp
    src/Date.cpp
    src/Period.cpp
    src/ValueType.cpp
    src/AccountModification.cpp
    src/JournalModification.cpp
    src/LedgerModification.cpp
    src/AccountRecords.cpp
    src/MonthRecords.cpp
    src/QuarterRecords.cpp
    src/YearRecords.cpp
)

