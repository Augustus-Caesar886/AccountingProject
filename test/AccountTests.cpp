#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/Account.h"

#include <string>
using std::string;

#include <stdexcept>
using std::invalid_argument;

class AccountTestsDriver : public Account {
    public:
        AccountTestsDriver(const string& name, ValueType valueType, AccountType accountType, DateUnit year, double beginningBalance = 0) : Account(name, valueType, accountType, year, beginningBalance) {}
        const MonthRecords& getMonthRecords(DateUnit month) const { return records.getQuarterRecords()[(month-1) / 3].getMonthRecords()[(month-1) % 3]; }
};

TEST(AccountTests, testConstructor) {
    AccountTestsDriver account1("Cash", ValueType::debit, AccountType::Asset, 2001, 1000); //Start 2001 with $1000 cash
    EXPECT_EQ(account1.getName(), "Cash");
    EXPECT_EQ(account1.getBalance(), 1000);
    EXPECT_EQ(account1.getBeginningBalance(), 1000);
    EXPECT_EQ(account1.getBalanceType(), ValueType::debit);
    EXPECT_EQ(account1.getAccountType(), AccountType::Asset);
    EXPECT_EQ(account1.getEntries().size(), 0);
    EXPECT_EQ(account1.getQuartersEntries(1).size(), 0);
    EXPECT_EQ(account1.getMonthsEntries(1).size(), 0);


    AccountTestsDriver account2("Accounts Payable", ValueType::credit, AccountType::Liability, 2001, 1000); //Start 2001 with $1000 owed to Accounts Payable
    EXPECT_EQ(account2.getName(), "Accounts Payable");
    EXPECT_EQ(account2.getBalance(), 1000);
    EXPECT_EQ(account2.getBeginningBalance(), 1000);
    EXPECT_EQ(account2.getBalanceType(), ValueType::credit);
    EXPECT_EQ(account2.getAccountType(), AccountType::Liability);
    EXPECT_EQ(account2.getEntries().size(), 0);
    EXPECT_EQ(account2.getQuartersEntries(1).size(), 0);
    EXPECT_EQ(account2.getMonthsEntries(1).size(), 0);
}

TEST(AccountTests, testAddEntry) {
    AccountTestsDriver cash("Cash", ValueType::debit, AccountType::Asset, 2001, 1000); //Start 2001 with $1000 Cash
    AccountTestsDriver ap("Accounts Payable", ValueType::credit, AccountType::Liability, 2001, 100); //Start 2001 with $100 owed to Accounts Payable
    AccountTestsDriver supplies("Supplies", ValueType::debit, AccountType::Asset, 2001, 200); //Start 2001 with $200 in supplies
    AccountTestsDriver revenue("Sales Revenue", ValueType::credit, AccountType::Revenue, 2001); //Start 2001 with $0 in revenue

    string transaction1description = "Pay off $100 of Accounts Payable";
    LedgerModification APModification1(100, ValueType::debit, Date("01/01/2001"), transaction1description);
    LedgerModification cashModification1(100, ValueType::credit, Date("01/01/2001"), transaction1description);

    ap.addEntry(APModification1);
    cash.addEntry(cashModification1);

    EXPECT_EQ(cash.getBalance(), 900);
    EXPECT_EQ(ap.getBalance(), 0);
    EXPECT_EQ(cash.getBeginningBalance(), 1000);
    EXPECT_EQ(ap.getBeginningBalance(), 100);
    
    string transaction2description = "Receive $500 worth of supplies";
    LedgerModification suppliesModification1(500, ValueType::debit, Date("02/01/2001"), transaction2description);
    LedgerModification APModification2(500, ValueType::credit, Date("02/01/2001"), transaction2description);

    supplies.addEntry(suppliesModification1);
    ap.addEntry(APModification2);

    EXPECT_EQ(supplies.getBalance(), 700);
    EXPECT_EQ(ap.getBalance(), 500);
    EXPECT_EQ(supplies.getBeginningBalance(), 200);
    EXPECT_EQ(ap.getBeginningBalance(), 100);

    string transaction3description = "Earn $300 from sales";
    LedgerModification cashModification2(300, ValueType::debit, Date("03/01/2001"), transaction3description);
    LedgerModification revenueModification1(300, ValueType::credit, Date("03/01/2001"), transaction3description);

    cash.addEntry(cashModification2);
    revenue.addEntry(revenueModification1);

    EXPECT_EQ(cash.getBalance(), 1200);
    EXPECT_EQ(revenue.getBalance(), 300);
    EXPECT_EQ(cash.getBeginningBalance(), 1000);
    EXPECT_EQ(revenue.getBeginningBalance(), 0);

    AccountTestsDriver suppliesExpense("Supplies Expense", ValueType::debit, AccountType::Expense, 2001); //Start 2001 with $0 supplies expense

    string transaction4description = "Record $50 of supplies expense";
    LedgerModification expenseModification1(50, ValueType::debit, Date("04/01/2001"), transaction4description);
    LedgerModification suppliesModification2(50, ValueType::credit, Date("04/01/2001"), transaction4description);

    suppliesExpense.addEntry(expenseModification1);
    supplies.addEntry(suppliesModification2);

    EXPECT_EQ(suppliesExpense.getBalance(), 50);
    EXPECT_EQ(supplies.getBalance(), 650);
    EXPECT_EQ(suppliesExpense.getBeginningBalance(), 0);
    EXPECT_EQ(supplies.getBeginningBalance(), 200);

    //Test internal integrity of detailed records
    //Cash
    EXPECT_EQ(cash.getMonthRecords(1).getBeginningBalance(), 1000);
    EXPECT_EQ(cash.getMonthRecords(1).getEndingBalance(), 900);
    EXPECT_EQ(cash.getMonthRecords(2).getBeginningBalance(), 900);
    EXPECT_EQ(cash.getMonthRecords(2).getEndingBalance(), 900);
    EXPECT_EQ(cash.getMonthRecords(3).getBeginningBalance(), 900);
    EXPECT_EQ(cash.getMonthRecords(3).getEndingBalance(), 1200);
    EXPECT_EQ(cash.getMonthRecords(4).getBeginningBalance(), 1200);
    EXPECT_EQ(cash.getMonthRecords(4).getEndingBalance(), 1200);
    EXPECT_EQ(cash.getEntries().size(), 2);
    //AP
    EXPECT_EQ(ap.getMonthRecords(1).getBeginningBalance(), 100);
    EXPECT_EQ(ap.getMonthRecords(1).getEndingBalance(), 0);
    EXPECT_EQ(ap.getMonthRecords(2).getBeginningBalance(), 0);
    EXPECT_EQ(ap.getMonthRecords(2).getEndingBalance(), 500);
    EXPECT_EQ(ap.getMonthRecords(3).getBeginningBalance(), 500);
    EXPECT_EQ(ap.getMonthRecords(3).getEndingBalance(), 500);
    EXPECT_EQ(ap.getMonthRecords(4).getBeginningBalance(), 500);
    EXPECT_EQ(ap.getMonthRecords(4).getEndingBalance(), 500);
    EXPECT_EQ(ap.getEntries().size(), 2);
    //Supplies
    EXPECT_EQ(supplies.getMonthRecords(1).getBeginningBalance(), 200);
    EXPECT_EQ(supplies.getMonthRecords(1).getEndingBalance(), 200);
    EXPECT_EQ(supplies.getMonthRecords(2).getBeginningBalance(), 200);
    EXPECT_EQ(supplies.getMonthRecords(2).getEndingBalance(), 700);
    EXPECT_EQ(supplies.getMonthRecords(3).getBeginningBalance(), 700);
    EXPECT_EQ(supplies.getMonthRecords(3).getEndingBalance(), 700);
    EXPECT_EQ(supplies.getMonthRecords(4).getBeginningBalance(), 700);
    EXPECT_EQ(supplies.getMonthRecords(4).getEndingBalance(), 650);
    EXPECT_EQ(supplies.getEntries().size(), 2);
    //Revenue
    EXPECT_EQ(revenue.getMonthRecords(1).getBeginningBalance(), 0);
    EXPECT_EQ(revenue.getMonthRecords(1).getEndingBalance(), 0);
    EXPECT_EQ(revenue.getMonthRecords(2).getBeginningBalance(), 0);
    EXPECT_EQ(revenue.getMonthRecords(2).getEndingBalance(), 0);
    EXPECT_EQ(revenue.getMonthRecords(3).getBeginningBalance(), 0);
    EXPECT_EQ(revenue.getMonthRecords(3).getEndingBalance(), 300);
    EXPECT_EQ(revenue.getMonthRecords(4).getBeginningBalance(), 300);
    EXPECT_EQ(revenue.getMonthRecords(4).getEndingBalance(), 300);
    EXPECT_EQ(revenue.getEntries().size(), 1);
    //Expense
    EXPECT_EQ(suppliesExpense.getMonthRecords(1).getBeginningBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(1).getEndingBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(2).getBeginningBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(2).getEndingBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(3).getBeginningBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(3).getEndingBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(4).getBeginningBalance(), 0);
    EXPECT_EQ(suppliesExpense.getMonthRecords(4).getEndingBalance(), 50);
    EXPECT_EQ(suppliesExpense.getEntries().size(), 1);
}

TEST(AccountTests, testThrow) {
    AccountTestsDriver account("Cash", ValueType::debit, AccountType::Asset, 2001, 1000);

    EXPECT_THROW({
        account.addEntry(LedgerModification(100, ValueType::debit, Date("01/01/2000"), ""));
    }, invalid_argument);
}