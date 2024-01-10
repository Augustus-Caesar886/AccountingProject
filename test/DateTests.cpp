#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/Date.h"


#include <stdexcept>
#include <string>
using std::string;
using std::invalid_argument;

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

TEST(dateTests, testObviousThrow) {
    string date = "ab/cd/efgh";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testSubtleThrow) {
    string date = "01/04/200O"; //Ends with letter 'O' rather than number 0
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testNegativeMonthThrow) {
    string date = "-1/05/2003";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testNegativeDayThrow) {
    string date = "01/-5/2003";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testNegativeYearThrow) {
    string date = "01/05/-203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testAllNegativesThrow) {
    string date = "-1/-5/-203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testLongMonthExcept) {
    string date = "101/05/2203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testLongDayExcept) {
    string date = "01/105/2203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testLongYearExcept) {
    string date = "01/05/12203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testShortMonthExcept) {
    string date = "1/05/2203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testShortDayExcept) {
    string date = "01/5/2203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testShortYearExcept) {
    string date = "01/05/203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testShortFieldsExcept) {
    string date = "1/5/203";
    EXPECT_THROW(Date d(date), invalid_argument);
}

TEST(dateTests, testEquality) {
    string date = "01/05/2003";
    Date d(date);
    EXPECT_EQ(d, d);
}

TEST(dateTests, testNoEquality) {
    string date1 = "01/05/2003", date2 = "01/05/2004";
    Date d1(date1), d2(date2);
    EXPECT_NE(d1, d2);
}