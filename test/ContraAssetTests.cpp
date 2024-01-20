#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/ContraAssetAccount.h"

TEST(ContraAssetTests, testConstructor) {
    ContraAssetAccount ad("accumulated depreciation", 2024, 1000); 

    EXPECT_EQ(ad.getBalanceType(), ValueType::credit);
    EXPECT_EQ(ad.getAccountType(), AccountType::ContraAsset);
    EXPECT_EQ(ad.getName(), "accumulated depreciation");
    EXPECT_EQ(ad.getYear(), 2024);
    EXPECT_EQ(ad.getBeginningBalance(), 1000);
    EXPECT_EQ(ad.getBalance(), 1000);
}