#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/ContraRevenueAccount.h"

TEST(ContraRevenueTests, testConstructor) {
    ContraRevenueAccount sr("sales returns", 2024, 1000); 

    EXPECT_EQ(sr.getBalanceType(), ValueType::debit);
    EXPECT_EQ(sr.getAccountType(), AccountType::ContraRevenue);
    EXPECT_EQ(sr.getName(), "sales returns");
    EXPECT_EQ(sr.getYear(), 2024);
    EXPECT_EQ(sr.getBeginningBalance(), 1000);
    EXPECT_EQ(sr.getBalance(), 1000);
}