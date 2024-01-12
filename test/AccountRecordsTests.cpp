#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/AccountRecords.h"
#include "../header/LedgerModification.h"
#include "../header/Date.h"

#include <vector>
using std::vector;

#include <stdexcept>
using std::invalid_argument;

class AccountRecordsTestDriver : public AccountRecords {
    private:
        vector<LedgerModification> modifications;
    public:
        AccountRecordsTestDriver(DateUnit year, ValueType vt, double amt) : AccountRecords(year, vt, amt) {}

        vector<LedgerModification> getEntries() { return modifications; }
};

TEST(AccountRecordsTests, testConstructor) {
    AccountRecordsTestDriver exampleMonth(2000, ValueType::debit, 1000); //Ex. start with 1000 cash in a debit account
    
    EXPECT_EQ(exampleMonth.getYear(), 2000);
    EXPECT_EQ(exampleMonth.getBeginningBalance(), exampleMonth.getEndingBalance());
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getValueType(), ValueType::debit);
}

TEST(AccountRecordsTests, testAddEntry) {
    AccountRecordsTestDriver exampleMonth(2000, ValueType::debit, 1000); //Ex. start with 1000 cash in a debit account
    
    exampleMonth.addEntry(LedgerModification(100, ValueType::credit, Date("01/02/2000"), "Pay with $100 cash"));

    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getEndingBalance(), 900);
    EXPECT_EQ(exampleMonth.getYear(), 2000);
}

TEST(AccountRecordsTests, testThrowBadYear) {
    AccountRecordsTestDriver exampleMonth(2000, ValueType::debit, 1000); //Ex. start with 1000 cash in a debit account
    
    EXPECT_THROW({
        exampleMonth.addEntry(LedgerModification(100, ValueType::credit, Date("01/02/2001"), "Pay with $100 cash"));
    }, invalid_argument);
}