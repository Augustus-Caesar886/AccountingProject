#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/StockholdersEquityAccount.h"

TEST(StockholdersEquityTests, testConstructor) {
    StockholdersEquityAccount pic("paid in cpicital", 2024, 1000); 

    EXPECT_EQ(pic.getBalanceType(), ValueType::credit);
    EXPECT_EQ(pic.getAccountType(), AccountType::StockholdersEquity);
    EXPECT_EQ(pic.getName(), "paid in cpicital");
    EXPECT_EQ(pic.getYear(), 2024);
    EXPECT_EQ(pic.getBeginningBalance(), 1000);
    EXPECT_EQ(pic.getBalance(), 1000);
}