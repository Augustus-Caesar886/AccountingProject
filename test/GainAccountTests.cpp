#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/GainAccount.h"

TEST(GainTests, testConstructor) {
    GainAccount gos("gain on sale of equipment", 2024, 1000); 

    EXPECT_EQ(gos.getBalanceType(), ValueType::credit);
    EXPECT_EQ(gos.getAccountType(), AccountType::GAIN);
    EXPECT_EQ(gos.getName(), "gain on sale of equipment");
    EXPECT_EQ(gos.getYear(), 2024);
    EXPECT_EQ(gos.getBeginningBalance(), 1000);
    EXPECT_EQ(gos.getBalance(), 1000);
}