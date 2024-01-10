#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/AccountModification.h"

#include <string>
using std::string;

TEST(AccountModificationTests, testCashToSupplies) {
    string description1 = "Acquired Supplies", description2 = "Paid in Cash";
    Date day("01/01/2001");
    AccountModification debited(1000, ValueType::debit, day, description1);
    AccountModification credited(1000, ValueType::credit, day, description2);
    
    EXPECT_EQ(debited.get().first, 1000);
    EXPECT_EQ(credited.get().first, 1000);
    EXPECT_EQ(debited.get().second, ValueType::debit);
    EXPECT_EQ(credited.get().second, ValueType::credit);
    EXPECT_EQ(debited.getDate(), credited.getDate());
    EXPECT_EQ(debited.getDate(), day);
    EXPECT_EQ(debited.getDescription(), description1);
    EXPECT_EQ(credited.getDescription(), description2);
}