#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/Date.h"
#include "../header/Period.h"

TEST(periodTests, testProperRecording) {
    Date startDate("01/01/2001");
    Date endDate("01/31/2001");
    Period p(startDate, endDate);

    EXPECT_EQ(p.getStartDate(), startDate);
    EXPECT_EQ(p.getEndDate(), endDate);
    EXPECT_NE(p.getStartDate(), endDate);
    EXPECT_NE(p.getEndDate(), startDate);
}