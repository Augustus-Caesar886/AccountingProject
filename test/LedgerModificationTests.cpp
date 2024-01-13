#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/LedgerModification.h"

TEST(LedgerModificationTests, testEquals) {
    LedgerModification modification(100, ValueType::credit, Date("01/01/2001"), "Starting balance of $100 on notes payable");
    EXPECT_EQ(modification, modification);
}