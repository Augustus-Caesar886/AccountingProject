#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/YearRecords.h"
#include "../header/LedgerModification.h"
#include "../header/ValueType.h"
#include "../header/Date.h"

#include <stdexcept>
using std::invalid_argument;

#include <vector>
using std::vector;

TEST(YearRecordsTests, testConstructor) {
    YearRecords fiscalYear(2001, ValueType::debit, 1000); //Start 2001 with $1000 cash
    
    EXPECT_EQ(fiscalYear.getYear(), 2001);
    EXPECT_EQ(fiscalYear.getBeginningBalance(), fiscalYear.getEndingBalance());
    EXPECT_EQ(fiscalYear.getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getValueType(), ValueType::debit);
    EXPECT_EQ(fiscalYear.getEntries().size(), 0);
    ASSERT_EQ(fiscalYear.getQuarterRecords().size(), 4);
    for(unsigned i = 0; i < 4; ++i) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getBeginningBalance(), 1000);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getEndingBalance(), 1000);

        for(unsigned j = 0; j < 3; ++j) {
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getBeginningBalance(), 1000);
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getEndingBalance(), 1000);
        }
    }
}

TEST(YearRecordsTests, addEntryThrow) {
    YearRecords fiscalYear(2001, ValueType::debit, 1000); //Start 2001 with $1000 cash

    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(100, ValueType::debit, Date("01/01/2002"), ""));
    }, invalid_argument);
}

void finalTest(const YearRecords &fiscalYear);

TEST(YearRecordsTests, yearRecordsAddEntryComprehensiveTest) {
    YearRecords fiscalYear(2001, ValueType::debit, 1000); //Start 2001 with $1000 cash
    fiscalYear.addEntry(LedgerModification(100, ValueType::debit, Date("01/03/2001"), "Earn $100 cash"));

    EXPECT_EQ(fiscalYear.getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getEndingBalance(), 1100);
    ASSERT_EQ(fiscalYear.getEntries().size(), 1);
    ASSERT_EQ(fiscalYear.getQuarterRecords().size(), 4);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getEndingBalance(), 1100);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getEndingBalance(), 1100);
    for(unsigned j = 1; j < 3; ++j) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[j].getBeginningBalance(), 1100);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[j].getEndingBalance(), 1100);
    }
    for(unsigned i = 1; i < 3; ++i) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getBeginningBalance(), 1100);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getEndingBalance(), 1100);

        for(unsigned j = 0; j < 3; ++j) {
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getBeginningBalance(), 1100);
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getEndingBalance(), 1100);
        }
    }

    fiscalYear.addEntry(LedgerModification(200, ValueType::debit, Date("01/04/2001"), "Earn $200 cash"));

    EXPECT_EQ(fiscalYear.getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getEndingBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getEndingBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getEndingBalance(), 1300);
    for(unsigned j = 1; j < 3; ++j) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[j].getBeginningBalance(), 1300);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[j].getEndingBalance(), 1300);
    }
    for(unsigned i = 1; i < 4; ++i) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getBeginningBalance(), 1300);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getEndingBalance(), 1300);

        for(unsigned j = 0; j < 3; ++j) {
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getBeginningBalance(), 1300);
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getEndingBalance(), 1300);
        }
    }

    fiscalYear.addEntry(LedgerModification(200, ValueType::debit, Date("02/04/2001"), "Earn $200 cash"));

    EXPECT_EQ(fiscalYear.getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getEndingBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[1].getBeginningBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[1].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[2].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[2].getEndingBalance(), 1500);
    for(unsigned i = 1; i < 4; ++i) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getBeginningBalance(), 1500);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getEndingBalance(), 1500);

        for(unsigned j = 0; j < 3; ++j) {
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getBeginningBalance(), 1500);
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getEndingBalance(), 1500);
        }
    }

    fiscalYear.addEntry(LedgerModification(500, ValueType::debit, Date("05/04/2001"), "Earn $500 cash"));

    EXPECT_EQ(fiscalYear.getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getEndingBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getEndingBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[1].getBeginningBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[1].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[2].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[2].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getEndingBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[0].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[0].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[1].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[1].getEndingBalance(), 2000);
    for(unsigned i = 2; i < 4; ++i) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getBeginningBalance(), 2000);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getEndingBalance(), 2000);

        for(unsigned j = 0; j < 3; ++j) {
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getBeginningBalance(), 2000);
            EXPECT_EQ(fiscalYear.getQuarterRecords()[i].getMonthRecords()[j].getEndingBalance(), 2000);
        }
    }

    fiscalYear.addEntry(LedgerModification(100, ValueType::credit, Date("12/31/2001"), "Pay $100 cash"));
    finalTest(fiscalYear);

    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(10, ValueType::debit, Date("01/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(20, ValueType::debit, Date("04/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(30, ValueType::debit, Date("06/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(40, ValueType::debit, Date("07/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(50, ValueType::debit, Date("08/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(60, ValueType::debit, Date("09/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(70, ValueType::debit, Date("10/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
    EXPECT_THROW({
        fiscalYear.addEntry(LedgerModification(80, ValueType::debit, Date("11/07/2001"), ""));
    }, invalid_argument);
    finalTest(fiscalYear);
}

void finalTest(const YearRecords& fiscalYear) {
    EXPECT_EQ(fiscalYear.getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getEndingBalance(), 1900);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[0].getEndingBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[1].getBeginningBalance(), 1300);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[1].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[2].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[0].getMonthRecords()[2].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getEndingBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[0].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[0].getEndingBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[1].getBeginningBalance(), 1500);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[1].getMonthRecords()[1].getEndingBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[2].getBeginningBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[2].getEndingBalance(), 2000);
    for(unsigned i = 0; i < 2; ++i) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[2].getMonthRecords()[i].getBeginningBalance(), 2000);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[2].getMonthRecords()[i].getEndingBalance(), 2000);
    }
    EXPECT_EQ(fiscalYear.getQuarterRecords()[3].getBeginningBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[3].getEndingBalance(), 1900);
    for(unsigned j = 0; j < 2; ++j) {
        EXPECT_EQ(fiscalYear.getQuarterRecords()[3].getMonthRecords()[j].getBeginningBalance(), 2000);
        EXPECT_EQ(fiscalYear.getQuarterRecords()[3].getMonthRecords()[j].getEndingBalance(), 2000);
    }
    EXPECT_EQ(fiscalYear.getQuarterRecords()[3].getMonthRecords()[2].getBeginningBalance(), 2000);
    EXPECT_EQ(fiscalYear.getQuarterRecords()[3].getMonthRecords()[2].getEndingBalance(), 1900);
    
}