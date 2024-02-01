#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/AccountLibrary.h"

#include <string>
using std::string;

#include <stdexcept>
using std::invalid_argument;

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
    accounts.addAccount("Equipment", AccountType::Asset, 1000);

    std::cout << "Debug for github environment" << std::endl;

    ASSERT_TRUE(accounts.addAlias("Cash", "Checking"));
    std::cout << "Passed check 1" << std::endl;

    accounts.getAccount("Cash").getName(); 
    std::cout << "retrieved cash account" << std::endl;
    accounts.getAccount("Checking").getName();
    std::cout << "retrieved checking account" << std::endl;


    EXPECT_TRUE(accounts.getAccount("Cash") == accounts.getAccount("Checking"));
    std::cout << "Passed check 2" << std::endl;

    EXPECT_FALSE(accounts.addAlias("Equipment", "Checking"));
    std::cout << "Passed check 3, indicates no segfault in tests" << std::endl;
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

    EXPECT_THROW({
        accounts.getAccount("AP");
    }, invalid_argument);
}

TEST(AccountLibraryTests, testLinkAccount) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);
    accounts.addAccount("Equipment", AccountType::Asset, 1000);
    accounts.linkAccount("Equipment", "Accumulated Depreciation", AccountType::ContraAsset, 300);

    ASSERT_EQ(accounts.findLinked("Cash"), nullptr);
    EXPECT_EQ(accounts.findLinked("Equipment")->getName(), "Accumulated Depreciation");
    EXPECT_EQ(accounts.getAccount("Accumulated Depreciation").getName(), "Accumulated Depreciation");
}

TEST(AccountLibraryTests, testRemoveAlias) {
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);
    accounts.addAlias("Cash", "Checking");

    ASSERT_EQ(accounts.getAccount("Cash"), accounts.getAccount("Checking"));
    
    accounts.removeAlias("checking");
    EXPECT_THROW({
        accounts.getAccount("checking");
    }, invalid_argument);

    EXPECT_EQ(accounts.getAccount("Cash"), accounts.getAccount("Cash"));
}