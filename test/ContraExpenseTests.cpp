#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/ContraExpenseAccount.h"

TEST(ContraExpenseTests, testConstructor) {
    ContraExpenseAccount example("example", 2024, 1000); 

    EXPECT_EQ(example.getBalanceType(), ValueType::credit);
    EXPECT_EQ(example.getAccountType(), AccountType::ContraExpense);
    EXPECT_EQ(example.getName(), "example");
    EXPECT_EQ(example.getYear(), 2024);
    EXPECT_EQ(example.getBeginningBalance(), 1000);
    EXPECT_EQ(example.getBalance(), 1000);
}