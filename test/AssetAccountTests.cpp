#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/AssetAccount.h"

TEST(assetTests, testConstructor) {
    AssetAccount cash("cash", 2024, 1000); 

    EXPECT_EQ(cash.getBalanceType(), ValueType::debit);
    EXPECT_EQ(cash.getAccountType(), AccountType::Asset);
    EXPECT_EQ(cash.getName(), "cash");
    EXPECT_EQ(cash.getYear(), 2024);
    EXPECT_EQ(cash.getBeginningBalance(), 1000);
    EXPECT_EQ(cash.getBalance(), 1000);
}