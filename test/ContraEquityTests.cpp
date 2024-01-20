#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/ContraEquityAccount.h"

TEST(ContraEquityTests, testConstructor) {
    ContraEquityAccount ts("treasury stock", 2024, 1000); 

    EXPECT_EQ(ts.getBalanceType(), ValueType::debit);
    EXPECT_EQ(ts.getAccountType(), AccountType::ContraEquity);
    EXPECT_EQ(ts.getName(), "treasury stock");
    EXPECT_EQ(ts.getYear(), 2024);
    EXPECT_EQ(ts.getBeginningBalance(), 1000);
    EXPECT_EQ(ts.getBalance(), 1000);
}