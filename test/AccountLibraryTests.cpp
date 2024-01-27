#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/AccountLibrary.h"

#include <string>
using std::string;

TEST(AccountLibraryTests, testConstructor) {
    AccountLibrary accounts(2024);

    EXPECT_EQ(accounts.getYear(), 2024);
}

TEST(AccountLibraryTests, testAddAsset) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);

    EXPECT_EQ(accounts.getAccount("Cash").getName(), "Cash");
    EXPECT_EQ(accounts.getAccount("Cash").getYear(), 2024);
    EXPECT_EQ(accounts.getAccount("Cash").getBalanceType(), ValueType::debit);
    EXPECT_EQ(accounts.getAccount("Cash").getAccountType(), AccountType::Asset);
    EXPECT_EQ(accounts.getAccount("Cash").getBalance(), 1000);
    EXPECT_EQ(accounts.getAccount("Cash").getBeginningBalance(), 1000);
}

TEST(AccountLibraryTests, testAddLiability) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Accounts Payable", AccountType::Liability, 1000);

    EXPECT_EQ(accounts.getAccount("Accounts Payable").getName(), "Accounts Payable");
    EXPECT_EQ(accounts.getAccount("Accounts Payable").getYear(), 2024);
    EXPECT_EQ(accounts.getAccount("Accounts Payable").getBalanceType(), ValueType::credit);
    EXPECT_EQ(accounts.getAccount("Accounts Payable").getAccountType(), AccountType::Liability);
    EXPECT_EQ(accounts.getAccount("Accounts Payable").getBalance(), 1000);
    EXPECT_EQ(accounts.getAccount("Accounts Payable").getBeginningBalance(), 1000);
}

TEST(AccountLibraryTests, testAddAlias) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);
    accounts.addAlias("Cash", "Checking");

    EXPECT_EQ(accounts.getAccount("Cash"), accounts.getAccount("Checking"));
}


TEST(AccountLibraryTests, testGetAccount) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);

    EXPECT_EQ(accounts.getAccount("CaSh").getName(), "Cash");
    EXPECT_EQ(accounts.getAccount("CAsh").getYear(), 2024);
    EXPECT_EQ(accounts.getAccount("cASH").getBalanceType(), ValueType::debit);
    EXPECT_EQ(accounts.getAccount("CASh").getAccountType(), AccountType::Asset);
    EXPECT_EQ(accounts.getAccount("cAsH").getBalance(), 1000);
    EXPECT_EQ(accounts.getAccount("CasH").getBeginningBalance(), 1000);
}

TEST(AccountLibraryTests, testLinkAccount) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);
    accounts.addAccount("Equipment", AccountType::Asset, 1000);
    accounts.linkAccount("Equipment", "Accumulated Depreciation", AccountType::ContraAsset, 300);

    ASSERT_EQ(accounts.findLinked("Cash"), nullptr);
    EXPECT_EQ(accounts.findLinked("Equipment")->getName(), "Accumulated Depcreciation");
}