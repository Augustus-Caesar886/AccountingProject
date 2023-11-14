#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/Date.h"


#include <string>
using std::string;

TEST(dateTests, testDateNumberConstructor1) {
    DateUnit year = 2000, month = 1, day = 1;
    Date d(year, month, day);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateNumberConstructor2) {
    DateUnit year = 2000, month = 1, day = 2;
    Date d(year, month, day);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateNumberConstructor3) {
    DateUnit year = 873, month = 6, day = 16;
    Date d(year, month, day);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateStringConstructor1) {
    string date = "01/01/2000";
    DateUnit year = 2000, month = 1, day = 1;
    Date d(date);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateStringConstructor2) {
    string date = "01/02/2000";
    DateUnit year = 2000, month = 1, day = 2;
    Date d(date);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateStringConstructor3) {
    string date = "06/16/0873";
    DateUnit year = 873, month = 6, day = 16;
    Date d(date);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateStringConstructor4) {
    string date = "11/16/0873";
    DateUnit year = 873, month = 11, day = 16;
    Date d(date);
    EXPECT_EQ(d.year, year);
    EXPECT_EQ(d.month, month);
    EXPECT_EQ(d.day, day);
}

TEST(dateTests, testDateStringForm1) {
    string date = "11/16/0873";
    Date d(date);
    EXPECT_EQ(date, d.stringForm());
}

TEST(dateTests, testDateStringForm2) {
    string date = "01/06/2873";
    Date d(date);
    EXPECT_EQ(date, d.stringForm());
}

TEST(dateTests, testDateStringForm3) {
    string date = "01/06/0006";
    Date d(date);
    EXPECT_EQ(date, d.stringForm());
}

TEST(dateTests, testDateStringForm4) {
    string date = "01/06/0036";
    Date d(date);
    EXPECT_EQ(date, d.stringForm());
}