#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/JournalEntryPoster.h"

TEST(JournalEntryPosterTests, testJournalPoster) {
    Journal journal(2024);
    AccountLibrary accounts(2024);
    JournalEntryPoster entryPoster(&journal, &accounts);
    accounts.addAccount("Cash", AccountType::Asset, 5000);
    accounts.addAccount("Paid in Capital, Common Stock", AccountType::StockholdersEquity, 5000);
    accounts.addAlias("Paid in Capital, Common Stock", "PiC");
    accounts.addAccount("Equipment", AccountType::Asset, 0);
    accounts.linkAccount("Equipment", "Accumulated Depreciation on Equipment", AccountType::ContraAsset, 0);
    accounts.addAlias("Accumulated Depreciation on Equipment", "ADoE");
    accounts.addAccount("Notes Payable", AccountType::Liability, 0);
    accounts.addAlias("Notes Payable", "NP");
    accounts.addAccount("Depreciation Expense", AccountType::Expense, 0);
    accounts.addAccount("Retained Earnings", AccountType::StockholdersEquity, 0);
    accounts.addAlias("Retained Earnings", "RE");

    JournalEntry entry1(Date("01/01/2024"), "Receive $3000 worth of equipment");
    entry1.addModification(JournalModification(3000, ValueType::debit, entry1.getDate(), entry1.getDescription(), &accounts.getAccount("Equipment")));

    EXPECT_FALSE(entryPoster.postModification(entry1));
    EXPECT_EQ(journal.getEntries().size(), 0);
    EXPECT_EQ(accounts.getAccount("Equipment").getEntries().size(), 0);
    EXPECT_EQ(accounts.getAccount("Equipment").getBalance(), 0);

    entry1.addModification(JournalModification(1000, ValueType::credit, entry1.getDate(), entry1.getDescription(), &accounts.getAccount("Cash")));

    EXPECT_FALSE(entryPoster.postModification(entry1));
    EXPECT_EQ(journal.getEntries().size(), 0);
    EXPECT_EQ(accounts.getAccount("Equipment").getEntries().size(), 0);
    EXPECT_EQ(accounts.getAccount("Equipment").getBalance(), 0);
    EXPECT_EQ(accounts.getAccount("Cash").getEntries().size(), 0);
    EXPECT_EQ(accounts.getAccount("Cash").getBalance(), 5000);

    entry1.addModification(JournalModification(2000, ValueType::credit, entry1.getDate(), entry1.getDescription(), &accounts.getAccount("np")));

    EXPECT_TRUE(entryPoster.postModification(entry1));
    EXPECT_EQ(journal.getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("Equipment").getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("Equipment").getBalance(), 3000);
    EXPECT_EQ(accounts.getAccount("Cash").getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("Cash").getBalance(), 4000);
    EXPECT_EQ(accounts.getAccount("NP").getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("NP").getBalance(), 2000);

    JournalEntry entry2(Date("04/01/2024"), "Record $150 of Accumulated Depreciation on Equipment");
    entry2.addModification(JournalModification(150, ValueType::debit, entry2.getDate(), entry2.getDescription(), &accounts.getAccount("depreciation expense")));

    EXPECT_FALSE(entryPoster.postModification(entry2));
    EXPECT_EQ(journal.getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getEntries().size(), 0);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getBalance(), 0);

    entry2.addModification(JournalModification(150, ValueType::credit, entry2.getDate(), entry2.getDescription(), &accounts.getAccount("adoe")));
    EXPECT_TRUE(entryPoster.postModification(entry2));
    EXPECT_EQ(journal.getEntries().size(), 2);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getBalance(), 150);
    EXPECT_EQ(accounts.getAccount("Accumulated Depreciation on Equipment").getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("Accumulated Depreciation on Equipment").getBalance(), 150);

    JournalEntry entry3(Date("12/31/2024"), "Record $450 of Accumulated Depreciation on Equipment");
    entry3.addModification(JournalModification(450, ValueType::debit, entry3.getDate(), entry3.getDescription(), &accounts.getAccount("depreciation expense")));

    EXPECT_FALSE(entryPoster.postModification(entry3));
    EXPECT_EQ(journal.getEntries().size(), 2);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getEntries().size(), 1);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getBalance(), 150);

    entry3.addModification(JournalModification(450, ValueType::credit, entry3.getDate(), entry3.getDescription(), &accounts.getAccount("adoe")));
    EXPECT_TRUE(entryPoster.postModification(entry3));
    EXPECT_EQ(journal.getEntries().size(), 3);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getEntries().size(), 2);
    EXPECT_EQ(accounts.getAccount("depreciation expense").getBalance(), 600);
    EXPECT_EQ(accounts.getAccount("Accumulated Depreciation on Equipment").getEntries().size(), 2);
    EXPECT_EQ(accounts.getAccount("Accumulated Depreciation on Equipment").getBalance(), 600);

    JournalEntry cje(Date("12/31/2024"), "CJE");
    cje.addModification(JournalModification(600, ValueType::debit, cje.getDate(), cje.getDescription(), &accounts.getAccount("RE")));
    cje.addModification(JournalModification(600, ValueType::credit, cje.getDate(), cje.getDescription(), &accounts.getAccount("depreciation expense")));
    EXPECT_TRUE(entryPoster.postModification(cje));

    EXPECT_EQ(accounts.getAccount("depreciation expense").getBalance(), 0);
}