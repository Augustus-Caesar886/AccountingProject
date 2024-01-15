#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/QuarterRecords.h"

#include <stdexcept>
using std::invalid_argument;

#include <vector>
using std::vector;

TEST(QuarterRecordsTests, testConstructorQuarter1) {
    QuarterRecords q1(2001, 1, ValueType::debit, 1000); //Start Q1 with $1000 cash
    
    EXPECT_EQ(q1.getQuarter(), 1);
    EXPECT_EQ(q1.getEntries().size(), 0);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q1.getMonthRecords()[i].getMonth(), i+1);
    }
}

TEST(QuarterRecordsTests, testConstructorQuarter2) {
    QuarterRecords q2(2001, 2, ValueType::debit, 1000); //Start Q2 with $1000 cash
    
    EXPECT_EQ(q2.getQuarter(), 2);
    EXPECT_EQ(q2.getEntries().size(), 0);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q2.getMonthRecords()[i].getMonth(), 3 + i+1);
    }
}

TEST(QuarterRecordsTests, testConstructorQuarter3) {
    QuarterRecords q3(2001, 3, ValueType::debit, 1000); //Start Q3 with $1000 cash
    
    EXPECT_EQ(q3.getQuarter(), 3);
    EXPECT_EQ(q3.getEntries().size(), 0);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q3.getMonthRecords()[i].getMonth(), 6 + i+1);
    }
}

TEST(QuarterRecordsTests, testConstructorQuarter4) {
    QuarterRecords q4(2001, 4, ValueType::debit, 1000); //Start Q4 with $1000 cash
    
    EXPECT_EQ(q4.getQuarter(), 4);
    EXPECT_EQ(q4.getEntries().size(), 0);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q4.getMonthRecords()[i].getMonth(), 9 + i+1);
    }
}

TEST(QuarterRecordsTests, testConstructorThrow) {

    EXPECT_THROW({
        QuarterRecords q4(2001, 5, ValueType::debit, 1000);
    }, invalid_argument);
    EXPECT_THROW({
        QuarterRecords q4(2001, 0, ValueType::debit, 1000);
    }, invalid_argument);
}

TEST(QuarterRecordsTests, testAddEntry) {
    QuarterRecords q3(2001, 3, ValueType::debit, 1000); //Start Q3 with $1000 cash
    LedgerModification modification1 = LedgerModification(100, ValueType::credit, Date("07/17/2001"), "Pay with $100 cash");
    q3.addEntry(modification1);

    EXPECT_EQ(q3.getBeginningBalance(), 1000);
    EXPECT_EQ(q3.getEndingBalance(), 900);
    ASSERT_EQ(q3.getMonthRecords()[0].getEntries().size(), 1);
    EXPECT_EQ(q3.getMonthRecords()[0].getEntries()[0], modification1);
    EXPECT_EQ(q3.getEntries()[0], modification1);
    EXPECT_EQ(q3.getMonthRecords()[1].getEntries().size(), 0);
    EXPECT_EQ(q3.getMonthRecords()[2].getEntries().size(), 0);

    LedgerModification modification2 = LedgerModification(100, ValueType::debit, Date("08/17/2001"), "Earn $100 cash");
    q3.addEntry(modification2);

    EXPECT_EQ(q3.getBeginningBalance(), 1000);
    EXPECT_EQ(q3.getEndingBalance(), 1000);
    EXPECT_EQ(q3.getMonthRecords()[0].getEndingBalance(), 900);
    EXPECT_EQ(q3.getMonthRecords()[1].getBeginningBalance(), 900);
    ASSERT_EQ(q3.getMonthRecords()[0].getEntries().size(), 1);
    EXPECT_EQ(q3.getMonthRecords()[0].getEntries()[0], modification1);
    EXPECT_EQ(q3.getEntries()[0], modification1);
    ASSERT_EQ(q3.getMonthRecords()[1].getEntries().size(), 1);
    EXPECT_EQ(q3.getEntries()[1], modification2);
    EXPECT_EQ(q3.getMonthRecords()[2].getEntries().size(), 0);
    EXPECT_EQ(q3.getMonthRecords()[1].getEntries()[0], modification2);

    LedgerModification modification3 = LedgerModification(200, ValueType::debit, Date("08/17/2001"), "Earn $200 cash");
    q3.addEntry(modification3);

    EXPECT_EQ(q3.getBeginningBalance(), 1000);
    EXPECT_EQ(q3.getEndingBalance(), 1200);
    EXPECT_EQ(q3.getMonthRecords()[0].getEndingBalance(), 900);
    EXPECT_EQ(q3.getMonthRecords()[1].getBeginningBalance(), 900);
    ASSERT_EQ(q3.getMonthRecords()[0].getEntries().size(), 1);
    EXPECT_EQ(q3.getMonthRecords()[0].getEntries()[0], modification1);
    EXPECT_EQ(q3.getEntries()[0], modification1);
    ASSERT_EQ(q3.getMonthRecords()[1].getEntries().size(), 2);
    EXPECT_EQ(q3.getMonthRecords()[2].getEntries().size(), 0);
    EXPECT_EQ(q3.getMonthRecords()[1].getEntries()[0], modification2);
    EXPECT_EQ(q3.getEntries()[1], modification2);
    EXPECT_EQ(q3.getMonthRecords()[1].getEntries()[1], modification3);
    EXPECT_EQ(q3.getEntries()[2], modification3);

    LedgerModification modification4 = LedgerModification(100, ValueType::debit, Date("09/17/2001"), "Earn $100 cash");
    q3.addEntry(modification4);

    EXPECT_EQ(q3.getBeginningBalance(), 1000);
    EXPECT_EQ(q3.getEndingBalance(), 1300);
    EXPECT_EQ(q3.getMonthRecords()[0].getEndingBalance(), 900);
    EXPECT_EQ(q3.getMonthRecords()[1].getBeginningBalance(), 900);
    EXPECT_EQ(q3.getMonthRecords()[1].getEndingBalance(), 1200);
    EXPECT_EQ(q3.getMonthRecords()[2].getBeginningBalance(), 1200);
    ASSERT_EQ(q3.getMonthRecords()[0].getEntries().size(), 1);
    EXPECT_EQ(q3.getMonthRecords()[0].getEntries()[0], modification1);
    EXPECT_EQ(q3.getEntries()[0], modification1);
    ASSERT_EQ(q3.getMonthRecords()[1].getEntries().size(), 2);
    EXPECT_EQ(q3.getMonthRecords()[1].getEntries()[0], modification2);
    EXPECT_EQ(q3.getEntries()[1], modification2);
    ASSERT_EQ(q3.getMonthRecords()[2].getEntries().size(), 1);
    EXPECT_EQ(q3.getMonthRecords()[2].getEntries()[0], modification4);
    EXPECT_EQ(q3.getEntries()[3], modification4);
}

TEST(QuarterRecordsTests, testAddEntryThrow) {
    QuarterRecords q1(2001, 1, ValueType::debit, 1000); //Start with $1000 cash

    EXPECT_THROW({
        q1.addEntry(LedgerModification(100, ValueType::credit, Date("04/01/2001"), "Pay $100 in cash"));
    }, invalid_argument);

    EXPECT_EQ(q1.getQuarter(), 1);
    EXPECT_EQ(q1.getEntries().size(), 0);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q1.getMonthRecords()[i].getEntries().size(), 0);
    }

    //Test for Q2
    QuarterRecords q2(2001, 2, ValueType::debit, 1000); //Start with $1000 cash

    EXPECT_THROW({
        q2.addEntry(LedgerModification(100, ValueType::credit, Date("03/01/2001"), "Pay $100 in cash"));
    }, invalid_argument);

    EXPECT_EQ(q2.getQuarter(), 2);
    EXPECT_EQ(q2.getEntries().size(), 0);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q2.getMonthRecords()[i].getEntries().size(), 0);
    }
}

TEST(QuarterRecordsTests, testGetEntriesAndMonthEntries) {
    QuarterRecords q2(2001, 2, ValueType::debit, 1000); //Start with $1000 cash
    LedgerModification aprilModification1(100, ValueType::credit, Date("04/03/2001"), "Pay $100 in cash");
    LedgerModification mayModification1(200, ValueType::credit, Date("05/01/2001"), "Pay $200 in cash");
    LedgerModification mayModification2(300, ValueType::credit, Date("05/02/2001"), "Pay $300 in cash");
    LedgerModification juneModification1(700, ValueType::debit, Date("06/02/2001"), "Earn $700 in cash");
    q2.addEntry(aprilModification1);
    q2.addEntry(mayModification1);
    q2.addEntry(mayModification2);
    q2.addEntry(juneModification1);
    vector<LedgerModification> expected = {aprilModification1, mayModification1, mayModification2, juneModification1};

    EXPECT_EQ(q2.getEndingBalance(), 1100);
    EXPECT_EQ(q2.getMonthRecords()[0].getEntries()[0], aprilModification1);
    EXPECT_EQ(q2.getMonthRecords()[1].getEntries()[0], mayModification1);
    EXPECT_EQ(q2.getMonthRecords()[1].getEntries()[1], mayModification2);
    EXPECT_EQ(q2.getMonthRecords()[2].getEntries()[0], juneModification1);
    EXPECT_EQ(q2.getEntries(), expected);

    EXPECT_EQ(q2.getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(q2.getMonthRecords()[0].getEndingBalance(), 900);
    EXPECT_EQ(q2.getMonthRecords()[1].getBeginningBalance(), 900);
    EXPECT_EQ(q2.getMonthRecords()[1].getEndingBalance(), 400);
    EXPECT_EQ(q2.getMonthRecords()[2].getBeginningBalance(), 400);
    EXPECT_EQ(q2.getMonthRecords()[2].getEndingBalance(), 1100);
}

TEST(QuarterRecordsTests, testProperMonthMaintenance) {
    QuarterRecords q3(2001, 3, ValueType::debit, 1000); //Start with $1000 cash
    q3.addEntry(LedgerModification(100, ValueType::debit, Date("07/17/2001"), "Earn $100 cash"));
    q3.addEntry(LedgerModification(100, ValueType::debit, Date("09/17/2001"), "Earn $100 cash"));
    
    EXPECT_EQ(q3.getMonthRecords()[1].getBeginningBalance(), 1100);
    EXPECT_EQ(q3.getMonthRecords()[1].getEndingBalance(), 1100);
    EXPECT_EQ(q3.getMonthRecords()[2].getBeginningBalance(), 1100);
    EXPECT_EQ(q3.getMonthRecords()[2].getEndingBalance(), 1200);
}

TEST(QuarterRecordsTests, testProperMonthMaintenanceAlternate) {
    QuarterRecords q3(2001, 3, ValueType::debit, 1000); //Start with $1000 cash
    q3.addEntry(LedgerModification(100, ValueType::debit, Date("08/17/2001"), "Earn $100 cash"));
    
    EXPECT_EQ(q3.getMonthRecords()[0].getBeginningBalance(), 1000);
    EXPECT_EQ(q3.getMonthRecords()[0].getEndingBalance(), 1000);
    EXPECT_EQ(q3.getMonthRecords()[1].getBeginningBalance(), 1000);
    EXPECT_EQ(q3.getMonthRecords()[1].getEndingBalance(), 1100);
    EXPECT_EQ(q3.getMonthRecords()[2].getEndingBalance(), 1100);
}

TEST(QuarterRecordsTests, testPeriodAdjustments) {
    QuarterRecords q1(2001, 1, ValueType::debit, 100);
    QuarterRecords q2(2001, 2, ValueType::debit, 100);
    q1.addEntry(LedgerModification(900, ValueType::debit, Date("03/31/2001"), "Earn $900 cash"));
    q2.adjustPeriodBalances(1000); //Set the new BB and EB to 1000 to reflect changes in quarter 1 balances

    EXPECT_EQ(q2.getBeginningBalance(), 1000);
    EXPECT_EQ(q2.getEndingBalance(), 1000);
    for(unsigned i = 0; i < 3; ++i) {
        EXPECT_EQ(q2.getMonthRecords()[i].getBeginningBalance(), 1000);
        EXPECT_EQ(q2.getMonthRecords()[i].getEndingBalance(), 1000);
    }
}

TEST(QuarterRecordsTests, testPeriodAdjustmentsThrow) {
    QuarterRecords q1(2001, 1, ValueType::debit, 100);
    QuarterRecords q2(2001, 2, ValueType::debit, 100);
    q1.addEntry(LedgerModification(900, ValueType::debit, Date("03/31/2001"), "Earn $900 cash"));
    q2.addEntry(LedgerModification(100, ValueType::debit, Date("04/01/2001"), "Earn $100 cash")); //Now that an entry has been added to q2, modifying BB and EB shouldn't be possible
    
    EXPECT_THROW({
        q2.adjustPeriodBalances(1000); 
    }, invalid_argument);

    EXPECT_EQ(q2.getEntries().size(), 1);
    EXPECT_EQ(q2.getBeginningBalance(), 100);
    EXPECT_EQ(q2.getEndingBalance(), 200);
    EXPECT_EQ(q2.getMonthRecords()[0].getBeginningBalance(), 100);
    EXPECT_EQ(q2.getMonthRecords()[0].getEndingBalance(), 200);
    for(unsigned i = 0; i < 2; ++i) {
        EXPECT_EQ(q2.getMonthRecords()[i+1].getBeginningBalance(), 200);
        EXPECT_EQ(q2.getMonthRecords()[i+1].getEndingBalance(), 200);
    }
}