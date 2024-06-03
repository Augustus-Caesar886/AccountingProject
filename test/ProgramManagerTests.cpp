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
    accounts.linkAccount("Accounts Receivable", "Allowance for Doubtful Accounts", ContraAsset, 0);
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
    accounts.addAccount("Bad Debt Expense", Expense, 0);
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
    accounts.addAlias("Bad Debt Expense", "BDE");

    accounts.addAccount("Dividends", Dividends, 0);

    {
        Date day = Date("01/01/2024");
        string description = "Prepare provision for doubtful accounts";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. BDE, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADA, 10"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("BDE").getBalance(), 10);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 200);
    }

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

    {
        Date day = Date("01/31/2024");
        string description = "Record Supplies Expense for January";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr SE, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. supplies, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("SE").getBalance(), 100);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 300);
    }

    {
        Date day = Date("01/31/2024");
        string description = "Record Depreciation Expense for January";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 45);
    }

    {
        Date day = Date("01/31/2024");
        string description = "Write-offs for January totaling $10";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. ada, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ar, 10"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("ada").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getBalance(), 100);
    }

    {
        Date day = Date("02/01/2024");
        string description = "Sales of $270 first day of February, all cash";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 270"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 270"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 1170);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1170);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1170);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
    }

    {
        Date day = Date("02/01/2024");
        string description = "Collection of remaining Accounts Receivable balances";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ar, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("ar").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
    }

    {
        Date day = Date("02/29/2024");
        string description = "Record Supplies Expense for February";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
    }

    {
        Date day = Date("02/29/2024");
        string description = "Record Depreciation Expense for February";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
    }

    {
        Date day = Date("03/01/2024");
        string description = "Sales of $330 first day of March, $30 on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 300"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. AR, 30"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 330"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 1570);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1570);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1570);
        EXPECT_EQ(accounts.getAccount("ar").getBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
    }

    {
        Date day = Date("03/02/2024");
        string description = "Received shipment of $400 in supplies, pay on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d supplies, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ap, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 600);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 600);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 600);
        EXPECT_EQ(accounts.getAccount("ap").getBalance(), 700);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getQuarterRecords()[0].getEndingBalance(), 700);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 700);
    }

    {
        Date day = Date("03/03/2024");
        string description = "Receive $1000 from common stock sales, at $10 total par";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d cash, 1000"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cs, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr apic cs, 990"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 2570);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 2570);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 2570);
        EXPECT_EQ(accounts.getAccount("cs").getBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("apic cs").getBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getEndingBalance(), 3190);
    }

    {
        Date day = Date("03/15/2024");
        string description = "Pay off AP balance";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ap, 700"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cash, 700"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
    }

    {
        Date day = Date("03/31/2024");
        string description = "Record Supplies Expense for March";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 120"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 120"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
    }

    {
        Date day = Date("03/31/2024");
        string description = "Record Depreciation Expense for March";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
    }

    {
        Date day = Date("04/01/2024");
        string description = "Earn $300 in sales, with $100 on credit";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. cash, 200"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ar, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 300"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 2070);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 2070);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2070);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 130);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
    }

    {
        Date day = Date("04/02/2024");
        string description = "Collect $130 from Accounts Receivable";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr cash, 130"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. AR, 130"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
    }

    {
        Date day = Date("04/30/2024");
        string description = "Record Supplies Expense for April";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr SE, 130"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. supplies, 130"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
    }

    {
        Date day = Date("04/30/2024");
        string description = "Record Depreciation Expense for April";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
    }

    {
        Date day = Date("05/01/2024");
        string description = "Sales of $270 first day of May, all cash";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 270"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 270"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
    }

    {
        Date day = Date("05/30/2024");
        string description = "Record Supplies Expense for May";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
    }

    {
        Date day = Date("05/30/2024");
        string description = "Record Depreciation Expense for May";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
    }

    {
        Date day = Date("06/01/2024");
        string description = "Sales of $330 first day of June, $30 on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 300"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. AR, 30"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 330"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 2770);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 2770);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 2770);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
    }

    {
        Date day = Date("06/02/2024");
        string description = "Received shipment of $400 in supplies, pay on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d supplies, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ap, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 650);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 650);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 650);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 400);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getEndingBalance(), 400);
    }

    {
        Date day = Date("06/03/2024");
        string description = "Receive $1000 from common stock sales, at $10 total par";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d cash, 1000"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cs, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr apic cs, 990"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3770);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3770);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3770);
        EXPECT_EQ(accounts.getAccount("cs").getBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(4).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(4).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(5).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(5).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(6).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(6).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("apic cs").getBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(4).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(4).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(5).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(5).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(6).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(6).getEndingBalance(), 4180);
    }

    {
        Date day = Date("06/15/2024");
        string description = "Pay off AP balance";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ap, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cash, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3370);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3370);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3370);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getEndingBalance(), 0);
    }

    {
        Date day = Date("06/15/2024");
        string description = "Pay dividends to stockholders";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d dividends, 40"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cash, 40"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("dividends").getBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[1].getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(1).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(2).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(6).getEndingBalance(), 40);
    }

    {
        Date day = Date("06/31/2024");
        string description = "Record Supplies Expense for June";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 140"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 140"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
    }

    {
        Date day = Date("06/31/2024");
        string description = "Record Depreciation Expense for June";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
    }
    
    
    {
        Date day = Date("07/01/2024");
        string description = "Earn $300 in sales, with $100 on credit";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. cash, 200"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ar, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 300"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3530);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 3530);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3530);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 130);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
    }
    
    {
        Date day = Date("07/02/2024");
        string description = "Collect $30 from Accounts Receivable";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr cash, 30"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. AR, 30"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 100);
    }

    {
        Date day = Date("07/31/2024");
        string description = "Record Supplies Expense for July";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr SE, 110"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. supplies, 110"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
    }

    {
        Date day = Date("07/31/2024");
        string description = "Record Depreciation Expense for July";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
    }


    {
        Date day = Date("08/01/2024");
        string description = "Sales of $270 first day of August, all cash";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 270"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 270"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 3830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3830);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getBeginningBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getEndingBalance(), 2370);
    }

    {
        Date day = Date("08/01/2024");
        string description = "Collection of remaining Accounts Receivable balances";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ar, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getEndingBalance(), 0);
    }

    {
        Date day = Date("08/31/2024");
        string description = "Record Supplies Expense for August";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getBeginningBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
    }

    {
        Date day = Date("08/31/2024");
        string description = "Record Depreciation Expense for August";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getEndingBalance(), 360);
    }

    {
        Date day = Date("09/01/2024");
        string description = "Sales of $330 first day of September, $30 on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 300"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. AR, 30"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 330"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 4230);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4230);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4230);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getBeginningBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getEndingBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getBeginningBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getEndingBalance(), 2700);
    }

    {
        Date day = Date("09/02/2024");
        string description = "Received shipment of $400 in supplies, pay on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d supplies, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ap, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 700);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 700);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getEndingBalance(), 700);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 400);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getEndingBalance(), 400);
    }

    {
        Date day = Date("09/03/2024");
        string description = "Receive $1000 from common stock sales, at $10 total par";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d cash, 1000"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cs, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr apic cs, 990"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 5230);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 5230);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 5230);
        EXPECT_EQ(accounts.getAccount("cs").getBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[1].getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[1].getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[2].getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[2].getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(4).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(4).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(5).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(5).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(6).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(6).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(7).getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(7).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(8).getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(8).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(9).getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(9).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("apic cs").getBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[1].getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[1].getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[2].getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[2].getEndingBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(4).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(4).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(5).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(5).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(6).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(6).getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(7).getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(7).getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(8).getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(8).getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(9).getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(9).getEndingBalance(), 5170);
    }

    {
        Date day = Date("09/15/2024");
        string description = "Pay off AP balance";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ap, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cash, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getEndingBalance(), 0);
    }

    {
        Date day = Date("09/31/2024");
        string description = "Record Supplies Expense for September";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 120"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 120"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        //Replace with up to date tests
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getBeginningBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getEndingBalance(), 580);
    }

    {
        Date day = Date("09/31/2024");
        string description = "Record Depreciation Expense for September";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getEndingBalance(), 405);
    }

    {
        Date day = Date("10/01/2024");
        string description = "Earn $300 in sales, with $100 on credit";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. cash, 200"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ar, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 300"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 5030);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getEndingBalance(), 5030);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getEndingBalance(), 5030);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 130);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[3].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[3].getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(10).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(10).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getEndingBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getBeginningBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getEndingBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getBeginningBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getEndingBalance(), 3000);
    }

    {
        Date day = Date("10/02/2024");
        string description = "Collect $130 from Accounts Receivable";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr cash, 130"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. AR, 130"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getEndingBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getEndingBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[3].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[3].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(10).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(10).getEndingBalance(), 0);
    }

    {
        Date day = Date("10/31/2024");
        string description = "Record Supplies Expense for October";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("Dr SE, 130"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("CR. supplies, 130"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getEndingBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getBeginningBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getEndingBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getEndingBalance(), 450);
    }

    {
        Date day = Date("10/31/2024");
        string description = "Record Depreciation Expense for October";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[3].getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(10).getEndingBalance(), 450);
    }

    {
        Date day = Date("11/01/2024");
        string description = "Sales of $270 first day of November, all cash";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 270"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 270"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getEndingBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getEndingBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getBeginningBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getEndingBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 3270);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getEndingBalance(), 3270);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getBeginningBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getEndingBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getBeginningBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getEndingBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(11).getBeginningBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(11).getEndingBalance(), 3270);
    }

    {
        Date day = Date("11/30/2024");
        string description = "Record Supplies Expense for November";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 100"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 100"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getEndingBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getBeginningBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getEndingBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(11).getBeginningBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(11).getEndingBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(11).getEndingBalance(), 350);
    }

    {
        Date day = Date("11/30/2024");
        string description = "Record Depreciation Expense for November";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 495);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getEndingBalance(), 495);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(11).getEndingBalance(), 495);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 495);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[3].getEndingBalance(), 495);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(11).getEndingBalance(), 495);
    }

    {
        Date day = Date("12/01/2024");
        string description = "Sales of $330 first day of December, $30 on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. cash, 300"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. AR, 30"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr sales, 330"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 5730);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getEndingBalance(), 5730);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getEndingBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getBeginningBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getEndingBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(12).getBeginningBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(12).getEndingBalance(), 5730);
        EXPECT_EQ(accounts.getAccount("ar").getBalance() - accounts.findLinked("ar")->getBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[0].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[1].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[2].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[3].getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getQuarterRecords()[3].getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getBeginningBalance(), 210);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(3).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(6).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(7).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(9).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(10).getBeginningBalance(), 30);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(10).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(11).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(11).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(12).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ar").getRecords().getMonthRecords(12).getEndingBalance(), 30);
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 3600);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getEndingBalance(), 3600);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getBeginningBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getEndingBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getBeginningBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getEndingBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(11).getBeginningBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(11).getEndingBalance(), 3270);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(12).getBeginningBalance(), 3270);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(12).getEndingBalance(), 3600);
    }

    {
        Date day = Date("12/02/2024");
        string description = "Received shipment of $400 in supplies, pay on account";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d supplies, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ap, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 750);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getEndingBalance(), 750);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(11).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(12).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(12).getEndingBalance(), 750);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 400);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[3].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[3].getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(10).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(10).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(11).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(11).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(12).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(12).getEndingBalance(), 400);
    }

    {
        Date day = Date("12/03/2024");
        string description = "Receive $1000 from common stock sales, at $10 total par";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d cash, 1000"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cs, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr apic cs, 990"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 6730);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getEndingBalance(), 6730);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getEndingBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getBeginningBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getEndingBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(12).getBeginningBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(12).getEndingBalance(), 6730);
        EXPECT_EQ(accounts.getAccount("cs").getBalance(), 140);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[1].getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[1].getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[2].getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[2].getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[3].getBeginningBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getQuarterRecords()[3].getEndingBalance(), 140);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(2).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(3).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(4).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(4).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(5).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(5).getEndingBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(6).getBeginningBalance(), 110);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(6).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(7).getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(7).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(8).getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(8).getEndingBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(9).getBeginningBalance(), 120);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(9).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(10).getBeginningBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(10).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(11).getBeginningBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(11).getEndingBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(11).getBeginningBalance(), 130);
        EXPECT_EQ(accounts.getAccount("cs").getRecords().getMonthRecords(12).getEndingBalance(), 140);
        EXPECT_EQ(accounts.getAccount("apic cs").getBalance(), 6160);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[0].getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[1].getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[1].getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[2].getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[2].getEndingBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[3].getBeginningBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getQuarterRecords()[3].getEndingBalance(), 6160);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(1).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(2).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(3).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(4).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(4).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(5).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(5).getEndingBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(6).getBeginningBalance(), 3190);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(6).getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(7).getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(7).getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(8).getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(8).getEndingBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(9).getBeginningBalance(), 4180);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(9).getEndingBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(10).getBeginningBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(10).getEndingBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(11).getBeginningBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(11).getEndingBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(12).getBeginningBalance(), 5170);
        EXPECT_EQ(accounts.getAccount("apic cs").getRecords().getMonthRecords(12).getEndingBalance(), 6160);
    }

    {
        Date day = Date("12/15/2024");
        string description = "Pay off AP balance";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("d ap, 400"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr cash, 400"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("cash").getBalance(), 6330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[0].getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[1].getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[2].getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getQuarterRecords()[3].getEndingBalance(), 6330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getBeginningBalance(), 1000);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(1).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(2).getEndingBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getBeginningBalance(), 1270);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(3).getEndingBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getBeginningBalance(), 1870);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(4).getEndingBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getBeginningBalance(), 2200);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(5).getEndingBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getBeginningBalance(), 2470);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(6).getEndingBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getBeginningBalance(), 3330);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(7).getEndingBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getBeginningBalance(), 3560);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(8).getEndingBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getBeginningBalance(), 3930);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(9).getEndingBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getBeginningBalance(), 4830);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(10).getEndingBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getBeginningBalance(), 5160);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(11).getEndingBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(12).getBeginningBalance(), 5430);
        EXPECT_EQ(accounts.getAccount("cash").getRecords().getMonthRecords(12).getEndingBalance(), 6330);
        EXPECT_EQ(accounts.getAccount("AP").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[2].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[3].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("AP").getRecords().getQuarterRecords()[3].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(2).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(6).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(7).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(9).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(10).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(10).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(11).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(11).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(12).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ap").getRecords().getMonthRecords(12).getEndingBalance(), 0);
    }

    {
        Date day = Date("12/31/2024");
        string description = "Record Supplies Expense for December";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. SE, 140"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies, 140"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("se").getBalance(), 1390);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getEndingBalance(), 1390);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getBeginningBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getEndingBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(11).getBeginningBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(11).getEndingBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(12).getBeginningBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(12).getEndingBalance(), 1390);
        EXPECT_EQ(accounts.getAccount("supplies").getBalance(), 610);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[0].getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[1].getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[2].getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getQuarterRecords()[3].getEndingBalance(), 610);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(3).getEndingBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getBeginningBalance(), 480);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(4).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(5).getEndingBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getBeginningBalance(), 250);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(6).getEndingBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getBeginningBalance(), 510);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(7).getEndingBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getBeginningBalance(), 400);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(8).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(9).getEndingBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getBeginningBalance(), 580);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(11).getEndingBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(12).getBeginningBalance(), 350);
        EXPECT_EQ(accounts.getAccount("supplies").getRecords().getMonthRecords(12).getEndingBalance(), 610);
    }

    {
        Date day = Date("12/31/2024");
        string description = "Record Depreciation Expense for December";
        JournalEntryCreator entryCreator(day, description);
        JournalModificationCreator modificationCreator(&program.getAccountLibrary(), day, description);
        entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. depreciation, 45"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr ADE, 45"));

        ASSERT_TRUE(program.postEntry(entryCreator.create()));

        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 540);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getEndingBalance(), 540);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(11).getEndingBalance(), 495);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(12).getBeginningBalance(), 495);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(12).getEndingBalance(), 540);
        EXPECT_EQ(accounts.getAccount("ADE").getBalance(), 540);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getQuarterRecords()[3].getEndingBalance(), 540);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(11).getEndingBalance(), 495);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(12).getBeginningBalance(), 495);
        EXPECT_EQ(accounts.getAccount("ADE").getRecords().getMonthRecords(12).getEndingBalance(), 540);
    }
    {
        /*entryCreator.addJournalModification(modificationCreator.getJournalModification("DR. sales, 3600"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr supplies expense, 1390"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr depreciation, 540"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr BDE, 10"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr dividends, 40"));
        entryCreator.addJournalModification(modificationCreator.getJournalModification("c. RE, 1620"));*/
        program.postClosingEntry();
        
        //Analyze account changes
        EXPECT_EQ(accounts.getAccount("sales").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[0].getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[1].getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[2].getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getQuarterRecords()[3].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(1).getEndingBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getBeginningBalance(), 300);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(2).getEndingBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getBeginningBalance(), 570);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(3).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(4).getEndingBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getBeginningBalance(), 1200);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(5).getEndingBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getBeginningBalance(), 1470);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(6).getEndingBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getBeginningBalance(), 1800);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(7).getEndingBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getBeginningBalance(), 2100);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(8).getEndingBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getBeginningBalance(), 2370);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(9).getEndingBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getBeginningBalance(), 2700);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(10).getEndingBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(11).getBeginningBalance(), 3000);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(11).getEndingBalance(), 3270);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(12).getBeginningBalance(), 3270);
        EXPECT_EQ(accounts.getAccount("sales").getRecords().getMonthRecords(12).getEndingBalance(), 0);

        EXPECT_EQ(accounts.getAccount("se").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[0].getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[1].getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[2].getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getQuarterRecords()[3].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(1).getEndingBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getBeginningBalance(), 100);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(2).getEndingBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getBeginningBalance(), 200);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(3).getEndingBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getBeginningBalance(), 320);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(4).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(5).getEndingBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getBeginningBalance(), 550);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(6).getEndingBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getBeginningBalance(), 690);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(7).getEndingBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getBeginningBalance(), 800);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(8).getEndingBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getBeginningBalance(), 900);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(9).getEndingBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getBeginningBalance(), 1020);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(10).getEndingBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(11).getBeginningBalance(), 1150);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(11).getEndingBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(12).getBeginningBalance(), 1250);
        EXPECT_EQ(accounts.getAccount("se").getRecords().getMonthRecords(12).getEndingBalance(), 0);

        EXPECT_EQ(accounts.getAccount("depreciation").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[0].getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[1].getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[2].getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getQuarterRecords()[3].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(1).getEndingBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getBeginningBalance(), 45);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(2).getEndingBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getBeginningBalance(), 90);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(3).getEndingBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getBeginningBalance(), 135);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(4).getEndingBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getBeginningBalance(), 180);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(5).getEndingBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getBeginningBalance(), 225);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(6).getEndingBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getBeginningBalance(), 270);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(7).getEndingBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getBeginningBalance(), 315);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(8).getEndingBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getBeginningBalance(), 360);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(9).getEndingBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getBeginningBalance(), 405);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(10).getEndingBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(11).getBeginningBalance(), 450);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(11).getEndingBalance(), 495);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(12).getBeginningBalance(), 495);
        EXPECT_EQ(accounts.getAccount("depreciation").getRecords().getMonthRecords(12).getEndingBalance(), 0);

        EXPECT_EQ(accounts.getAccount("BDE").getBalance(), 0);

        EXPECT_EQ(accounts.getAccount("dividends").getBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[1].getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[2].getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[2].getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[3].getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getQuarterRecords()[3].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(1).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(2).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(6).getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(7).getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(7).getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(8).getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(8).getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(9).getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(9).getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(10).getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(10).getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(11).getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(11).getEndingBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(12).getBeginningBalance(), 40);
        EXPECT_EQ(accounts.getAccount("dividends").getRecords().getMonthRecords(12).getEndingBalance(), 0);

        EXPECT_EQ(accounts.getAccount("RE").getBalance(), 1620);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[0].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[0].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[1].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[1].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[2].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[2].getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[3].getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getQuarterRecords()[3].getEndingBalance(), 1620);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(1).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(1).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(2).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(2).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(3).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(3).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(4).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(4).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(5).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(5).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(6).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(6).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(7).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(7).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(8).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(8).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(9).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(9).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(10).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(10).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(11).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(11).getEndingBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(12).getBeginningBalance(), 0);
        EXPECT_EQ(accounts.getAccount("RE").getRecords().getMonthRecords(12).getEndingBalance(), 1620);
    }
}