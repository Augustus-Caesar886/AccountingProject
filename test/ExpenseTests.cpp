#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/ExpenseAccount.h"

TEST(ExpenseTests, testConstructor) {
    ExpenseAccount se("supplies expense", 2024, 1000); 

    EXPECT_EQ(se.getBalanceType(), ValueType::debit);
    EXPECT_EQ(se.getAccountType(), AccountType::Expense);
    EXPECT_EQ(se.getName(), "supplies expense");
    EXPECT_EQ(se.getYear(), 2024);
    EXPECT_EQ(se.getBeginningBalance(), 1000);
    EXPECT_EQ(se.getBalance(), 1000);
}