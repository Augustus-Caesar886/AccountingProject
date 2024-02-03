#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/Journal.h"
#include "../header/AccountLibrary.h"

TEST(JournalTests, testConstructor) {
    Journal journal(2024);

    EXPECT_EQ(journal.getYear(), 2024);
    EXPECT_EQ(journal.getEntries().size(), 0);
}

TEST(JournalTests, testAddEntry) {
    Journal journal(2024);
    AccountLibrary accounts(2024);
    accounts.addAccount("Cash", AccountType::Asset, 1000);
    accounts.addAccount("Accounts Receivable", AccountType::Asset, 500);

    JournalEntry entry(Date("01/01/2024"), "Collect $500 from Accounts Receivable");
    entry.addModification(JournalModification(500, ValueType::debit, entry.getDate(), entry.getDescription(), &accounts.getAccount("Cash")));

    EXPECT_FALSE(journal.journalize(entry));
    EXPECT_EQ(journal.getEntries().size(), 0);

    entry.addModification(JournalModification(500, ValueType::credit, entry.getDate(), entry.getDescription(), &accounts.getAccount("Accounts Receivable")));

    EXPECT_TRUE(journal.journalize(entry));
    ASSERT_EQ(journal.getEntries().size(), 1);
    ASSERT_EQ(journal.getEntries().front().getModifications().size(), 2);
    EXPECT_EQ(journal.getEntries().front().getModifications()[0].getAffectedAccount()->getName(), "Cash");
    EXPECT_EQ(journal.getEntries().front().getModifications()[1].getAffectedAccount()->getName(), "Accounts Receivable");
}