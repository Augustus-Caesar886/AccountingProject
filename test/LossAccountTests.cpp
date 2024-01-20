#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/LossAccount.h"

TEST(LossTests, testConstructor) {
    LossAccount los("loss on sale of equipment", 2024, 1000); 

    EXPECT_EQ(los.getBalanceType(), ValueType::debit);
    EXPECT_EQ(los.getAccountType(), AccountType::LOSS);
    EXPECT_EQ(los.getName(), "loss on sale of equipment");
    EXPECT_EQ(los.getYear(), 2024);
    EXPECT_EQ(los.getBeginningBalance(), 1000);
    EXPECT_EQ(los.getBalance(), 1000);
}