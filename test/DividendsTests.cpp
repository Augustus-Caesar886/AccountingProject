#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/DividendsAccount.h"

TEST(DividendsTests, testConstructor) {
    DividendsAccount dividends("dividends", 2024, 1000); 

    EXPECT_EQ(dividends.getBalanceType(), ValueType::debit);
    EXPECT_EQ(dividends.getAccountType(), AccountType::Dividends);
    EXPECT_EQ(dividends.getName(), "dividends");
    EXPECT_EQ(dividends.getYear(), 2024);
    EXPECT_EQ(dividends.getBeginningBalance(), 1000);
    EXPECT_EQ(dividends.getBalance(), 1000);
}