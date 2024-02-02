#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/JournalEntry.h"
#include "../header/Date.h"
#include "../header/AssetAccount.h"

#include <stdexcept>
using std::invalid_argument;

TEST(JournalEntryTests, testConstructor) {
    JournalEntry entry(Date("01/01/2024"));

    EXPECT_TRUE(entry.validate());
    EXPECT_EQ(entry.getModifications().size(), 0);
}

TEST(JournalEntryTests, testAddModification) {
    JournalEntry entry(Date("01/01/2024"));
    AssetAccount cash("Cash", 2024, 1000);
    AssetAccount accountsReceivable("Accounts Receivable", 2024, 500);

    entry.addModification(JournalModification(500, ValueType::debit, Date("01/01/2024"), "Collect $500 from Accounts Receivable", &cash));
    EXPECT_FALSE(entry.validate());

    entry.addModification(JournalModification(500, ValueType::credit, Date("01/01/2024"), "Collect $500 from Accounts Receivable", &accountsReceivable));
    EXPECT_TRUE(entry.validate());

    EXPECT_EQ(entry.getModifications().size(), 2);
    EXPECT_EQ(entry.getModifications()[0].getAffectedAccount(), &cash);
    EXPECT_EQ(entry.getModifications()[1].getAffectedAccount(), &accountsReceivable);
}

TEST(JournalEntryTests, testAddModificationThrow) {
    JournalEntry entry(Date("01/01/2024"));
    AssetAccount cash("Cash", 2024, 1000);
    AssetAccount accountsReceivable("Accounts Receivable", 2024, 500);

    entry.addModification(JournalModification(500, ValueType::debit, Date("01/01/2024"), "Collect $500 from Accounts Receivable", &cash));
    entry.addModification(JournalModification(500, ValueType::credit, Date("01/01/2024"), "Collect $500 from Accounts Receivable", &accountsReceivable));

    EXPECT_THROW({
        entry.addModification(JournalModification(500, ValueType::credit, Date("01/02/2024"), "Collect $500 from Accounts Receivable", &accountsReceivable));
    }, invalid_argument);

    EXPECT_THROW({
        entry.addModification(JournalModification(500, ValueType::debit, Date("01/01/2024"), "Collect $500 from Accounts Receivable", &accountsReceivable));
    }, invalid_argument);
}