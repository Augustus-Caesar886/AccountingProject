#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/JournalModification.h"
#include "../header/AssetAccount.h"

TEST(JournalModificationTests, testConstructor) {
    AssetAccount cash("Cash", 2024, 1000);
    JournalModification modification = JournalModification(100, ValueType::credit, Date("01/01/2024"), "Pay $100 to Accounts Payable", &cash);

    EXPECT_EQ(modification.getAffectedAccount(), &cash);
    EXPECT_EQ(modification.get().first, 100);
    EXPECT_EQ(modification.get().second, ValueType::credit);
}