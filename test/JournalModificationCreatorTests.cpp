#include "../header/JournalModificationCreator.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/JournalModificationCreator.h"
#include "../header/AssetAccount.h"

#include <stdexcept>
using std::invalid_argument;

TEST(JournalModificationCreatorTests, testModificationCreator) {
    AccountLibrary accounts(2024);
    JournalModificationCreator modificationCreator(&accounts, Date("01/01/2024"), "Collect $100 from Accounts Receivable and Sales");
    accounts.addAccount("Cash", AccountType::Asset, 1000);
    accounts.addAccount("Accounts Receivable", AccountType::Asset, 100);
    accounts.addAccount("Sales Revenue", AccountType::Revenue, 0);
    accounts.addAlias("Sales Revenue", "Sales");

    auto modification1 = modificationCreator.getJournalModification("Dr. Cash, 100");
    auto modification2 = modificationCreator.getJournalModification("cr Accounts Receivable, 80");
    auto modification3 = modificationCreator.getJournalModification("c Sales, 20");

    EXPECT_EQ(modification1.get().first, 100);
    EXPECT_EQ(modification1.get().second, ValueType::debit);
    EXPECT_EQ(modification1.getDescription(), "Collect $100 from Accounts Receivable and Sales");
    EXPECT_EQ(modification1.getDate(), Date("01/01/2024"));
    EXPECT_EQ(modification1.getAffectedAccount()->getName(), accounts.getAccount("Cash").getName());

    EXPECT_EQ(modification2.get().first, 80);
    EXPECT_EQ(modification2.get().second, ValueType::credit);
    EXPECT_EQ(modification2.getDescription(), "Collect $100 from Accounts Receivable and Sales");
    EXPECT_EQ(modification2.getDate(), Date("01/01/2024"));
    EXPECT_EQ(modification2.getAffectedAccount()->getName(), accounts.getAccount("accounts receivable").getName());

    EXPECT_EQ(modification3.get().first, 20);
    EXPECT_EQ(modification3.get().second, ValueType::credit);
    EXPECT_EQ(modification3.getDescription(), "Collect $100 from Accounts Receivable and Sales");
    EXPECT_EQ(modification3.getDate(), Date("01/01/2024"));
    EXPECT_EQ(modification3.getAffectedAccount()->getName(), accounts.getAccount("sales revenue").getName());

    EXPECT_THROW({
        auto modification = modificationCreator.getJournalModification("c Sales 20");
    }, invalid_argument);

    EXPECT_THROW({
        auto modification = modificationCreator.getJournalModification("c Sales, a");
    }, invalid_argument);
}