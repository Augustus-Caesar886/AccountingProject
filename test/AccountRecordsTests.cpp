#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/AccountRecords.h"
#include "../header/LedgerModification.h"

#include <vector>
using std::vector;

class AccountRecordsTestDriver : public AccountRecords {
    private:
        vector<LedgerModification> modifications;
    public:
        AccountRecordsTestDriver(ValueType vt, double amt) : AccountRecords(vt, amt) {}

        void addEntry(const LedgerModification& modification) {
            modifications.push_back(modification);
            if(modification.get().second == accountType) {
                endingBalance += modification.get().first;
            } else {
                endingBalance -= modification.get().first;
            }
        }

        vector<LedgerModification> getEntries() { return modifications; }
};

TEST(AccountRecordsTests, testConstructor) {
    AccountRecordsTestDriver exampleMonth(ValueType::debit, 1000); //Ex. start with 1000 cash in a debit account
    
    EXPECT_EQ(exampleMonth.getBeginningBalance(), exampleMonth.getEndingBalance());
    EXPECT_EQ(exampleMonth.getBeginningBalance(), 1000);
    EXPECT_EQ(exampleMonth.getValueType(), ValueType::debit);
}