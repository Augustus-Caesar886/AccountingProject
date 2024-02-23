CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

ADD_EXECUTABLE(AccountingManualTests
    main.cpp
    ../../src/AccountDisplayer.cpp
    ../../src/JournalEntryPoster.cpp
    ../../src/JournalEntryCreator.cpp
    ../../src/JournalModificationCreator.cpp
    ../../src/Account.cpp
    ../../src/JournalEntry.cpp
    ../../src/JournalModification.cpp
    ../../src/Journal.cpp
    ../../src/AccountLibrary.cpp
    ../../src/YearRecords.cpp
    ../../src/QuarterRecords.cpp
    ../../src/MonthRecords.cpp
    ../../src/Period.cpp
    ../../src/AccountRecords.cpp
    ../../src/Date.cpp
)