#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::InSequence;

#include "../header/ProgramManager.h"
#include "../header/JournalEntryCreator.h"
#include "../header/JournalModificationCreator.h"

using std::string;

TEST(ProgramManagerTests, testConstructor) {
    ProgramManager program(2024);

    EXPECT_EQ(program.getAccountLibrary().getYear(), 2024);
    EXPECT_EQ(program.getJournal().getYear(), 2024);
    EXPECT_EQ(program.getJournal().getEntries().size(), 0);
}

TEST(ProgramManagerTests, programIntegrationTest) {
    ProgramManager program(2024);
    AccountLibrary& accounts = program.getAccountLibrary();

    //Initialize accounts

    accounts.addAccount("Cash", Asset, 1000);
    accounts.addAccount("Supplies", Asset, 400);
    accounts.addAccount("Accounts Receivable", Asset, 210);
    accounts.linkAccount("Accounts Receivable", "Allowance for Doubtful Accounts", ContraAsset, 10);
    accounts.addAccount("Equipment", Asset, 3000);
    accounts.linkAccount("Equipment", "Accumulated Depreciation on Equipment", ContraAsset, 0);
    //Net Assets = 4600

    accounts.addAccount("Accounts Payable", Liability, 300);
    accounts.addAccount("Notes Payable", Liability, 1500);
    accounts.addAccount("Wages Payable", Liability, 500);
    //Net Liabilities = 2300

    accounts.addAccount("Retained Earnings", StockholdersEquity, 0);
    accounts.addAccount("Common Stock", StockholdersEquity, 100);
    accounts.addAccount("Additional Paid in Capital - Common Stock", StockholdersEquity, 2200);
    accounts.addAccount("Sales Revenue", Revenue, 0);
    accounts.addAccount("Supplies Expense", Expense, 0);
    accounts.addAccount("Depreciation Expense", Expense, 0);
    //Net SE = 2300

    //Initialize aliases
    accounts.addAlias("Accounts Receivable", "AR");
    accounts.addAlias("Allowance for Doubtful Accounts", "ADA");
    accounts.addAlias("Accumulated Depreciation on Equipment", "ADE");
    accounts.addAlias("Accounts Payable", "AP");
    accounts.addAlias("Notes Payable", "NP");
    accounts.addAlias("Wages Payable", "WP");
    accounts.addAlias("Retained Earnings", "RE");
    accounts.addAlias("Common Stock", "CS");
    accounts.addAlias("Additional Paid in Capital - Common Stock", "APIC CS");
    accounts.addAlias("Sales Revenue", "Sales");
    accounts.addAlias("Supplies Expense", "SE");
    accounts.addAlias("Depreciation Expense", "Depreciation");

    accounts.addAccount("Dividends", Dividends, 0);

    {
        Date day = Date("01/01/2024");
        string description = "Earn $300 in sales, with $100 on credit";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. cash, 200"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ar, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 300"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 300);
    }

    {
        Date day = Date("01/02/2024");
        string description = "Pay wages owed from last year";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d. WP, 500"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr. cash, 500"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 700);
        EXPECT_EQ(accounts.getAccount("WP").getBalance(), 0);
    }

    {
        Date day = Date("01/02/2024");
        string description = "Collect $200 from Accounts Receivable";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr cash, 200"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. AR, 200"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 900);
        EXPECT_EQ(accounts.getAccount("ar").getBalance(), 110);
    }
}