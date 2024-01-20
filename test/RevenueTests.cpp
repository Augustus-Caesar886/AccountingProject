#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/RevenueAccount.h"

TEST(RevenueTests, testConstructor) {
    RevenueAccount sales("sales revenue", 2024, 1000); 

    EXPECT_EQ(sales.getBalanceType(), ValueType::credit);
    EXPECT_EQ(sales.getAccountType(), AccountType::Revenue);
    EXPECT_EQ(sales.getName(), "sales revenue");
    EXPECT_EQ(sales.getYear(), 2024);
    EXPECT_EQ(sales.getBeginningBalance(), 1000);
    EXPECT_EQ(sales.getBalance(), 1000);
}