#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/JournalEntryCreator.h"
#include "../header/JournalModificationCreator.h"

#include <stdexcept>
using std::runtime_error;

TEST(JournalEntryCreatorTests, testJournalEntryCreator) {
    JournalEntryCreator entryCreator(Date("01/01/2024"), "Collect $500 cash from Accounts Receivable");
    AccountLibrary accounts(2024);

    accounts.addAccount("Cash", Asset, 500);
    accounts.addAccount("Accounts Receivable", Asset, 500);

    JournalModificationCreator modificationCreator(&accounts, Date("01/01/2024"), "Collect $500 cash from Accounts Receivable");
    entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. cash, 500"));

    EXPECT_THROW({
        entryCreator.create();
    }, runtime_error);

    entryCreator.addJournalModification(modificationCreator.getJournalModification("cr. accounts receivable, 500"));

    JournalEntry created = entryCreator.create();

    EXPECT_EQ(created.getDate(), Date("01/01/2024"));
    EXPECT_EQ(created.getDescription(), "Collect $500 cash from Accounts Receivable");
    ASSERT_EQ(created.getModifications().size(), 2);
    EXPECT_EQ(created.getModifications()[0].get().first, 500);
    EXPECT_EQ(created.getModifications()[0].get().second, debit);
    EXPECT_EQ(created.getModifications()[1].get().first, 500);
    EXPECT_EQ(created.getModifications()[1].get().second, credit);
}