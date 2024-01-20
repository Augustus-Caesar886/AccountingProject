#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/LiabilityAccount.h"

TEST(LiabilityTests, testConstructor) {
    LiabilityAccount ap("accounts payable", 2024, 1000); 

    EXPECT_EQ(ap.getBalanceType(), ValueType::credit);
    EXPECT_EQ(ap.getAccountType(), AccountType::Liability);
    EXPECT_EQ(ap.getName(), "accounts payable");
    EXPECT_EQ(ap.getYear(), 2024);
    EXPECT_EQ(ap.getBeginningBalance(), 1000);
    EXPECT_EQ(ap.getBalance(), 1000);
}