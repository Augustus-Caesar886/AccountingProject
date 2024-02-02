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
    src/Account.cpp
    src/AssetAccount.cpp
    src/LiabilityAccount.cpp
    src/StockholdersEquityAccount.cpp
    src/RevenueAccount.cpp
    src/ExpenseAccount.cpp
    src/GainAccount.cpp
    src/LossAccount.cpp
    src/DividendsAccount.cpp
    src/ContraAssetAccount.cpp
    src/ContraLiabilityAccount.cpp
    src/ContraEquityAccount.cpp
    src/ContraRevenueAccount.cpp
    src/ContraExpenseAccount.cpp
    src/AccountLibrary.cpp
    src/JournalEntry.cpp
)

