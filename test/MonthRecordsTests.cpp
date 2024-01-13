#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/MonthRecords.h"

#include <stdexcept>
using std::invalid_argument;

TEST(MonthRecordsTests, testConstructor) {
    MonthRecords exampleMonth(2001, 1, ValueType::credit, 1000); //On January 1, 2001, start with $1000 in Accounts Payable

    EXPECT_EQ(exampleMonth.getYear(), 2001);
    EXPECT_EQ(exampleMonth.getMonth(), 1);
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEndingBalance(), 1000);
    EXPECT_EQ(exampleMonth.getValueType(), ValueType::credit);
}

TEST(MonthRecordsTests, testAddEntry) {
    MonthRecords exampleMonth(2001, 1, ValueType::credit, 1000); //On January 1, 2001, start with $1000 in Accounts Payable
    LedgerModification modification1 = LedgerModification(100, ValueType::debit, Date("01/02/2001"), "Pay $100 to Accounts Payable");
    exampleMonth.addEntry(modification1);
    
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEndingBalance(), 900);
    ASSERT_EQ(exampleMonth.getEntries().size(), 1);
    EXPECT_EQ(exampleMonth.getEntries()[0], modification1);
    
    LedgerModification modification2 = LedgerModification(200, ValueType::credit, Date("01/02/2001"), "Receive $200 worth of goods on credit");
    exampleMonth.addEntry(modification2);
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEndingBalance(), 1100);
    ASSERT_EQ(exampleMonth.getEntries().size(), 2);
    EXPECT_EQ(exampleMonth.getEntries()[0], modification1);
    EXPECT_EQ(exampleMonth.getEntries()[1], modification2);
}

TEST(MonthRecordsTests, testBadMonth) {
    MonthRecords exampleMonth(2001, 1, ValueType::debit, 1000); //On January 1, 2001, start with $1000 in cash
    LedgerModification modification1 = LedgerModification(100, ValueType::debit, Date("02/02/2001"), "Earn $100 in Cash");
    
    EXPECT_THROW({
        exampleMonth.addEntry(modification1);
    }, invalid_argument);

    EXPECT_EQ(exampleMonth.getYear(), 2001);
    EXPECT_EQ(exampleMonth.getMonth(), 1);
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEndingBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEntries().size(), 0);
}

TEST(MonthRecordsTests, testBadYear) {
    MonthRecords exampleMonth(2001, 1, ValueType::debit, 1000); //On January 1, 2001, start with $1000 in cash
    LedgerModification modification1 = LedgerModification(100, ValueType::debit, Date("01/02/2002"), "Earn $100 in Cash");
    
    EXPECT_THROW({
        exampleMonth.addEntry(modification1);
    }, invalid_argument);

    EXPECT_EQ(exampleMonth.getYear(), 2001);
    EXPECT_EQ(exampleMonth.getMonth(), 1);
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEndingBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEntries().size(), 0);
}
