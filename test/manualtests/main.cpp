#include <iostream>
using std::cout;
using std::endl;
using std::cin;

#include "../../header/AccountDisplayer.h"
#include "../../header/JournalEntryPoster.h"
#include "../../header/JournalEntryCreator.h"
#include "../../header/JournalModificationCreator.h"

int main() {
    Journal journal(2024);
    AccountLibrary accounts(2024);
    JournalEntryPoster entryPoster(&journal, &accounts);

    accounts.addAccount("Cash", Asset, 1000);
    accounts.addAccount("Accounts Receivable", Asset, 500);
    accounts.addAccount("Equipment", Asset, 3000);
    accounts.addAccount("Supplies", Asset, 800);
    accounts.linkAccount("Equipment", "Accumulated Depreciation on Equipment", ContraAsset, 500);
    //Net Assets = 5300 - 500 = $4800

    accounts.addAccount("Accounts Payable", Liability, 600);
    accounts.addAccount("Notes Payable", Liability, 1500);
    //Net Liabilities = $2100

    accounts.addAccount("Sales Revenue", Revenue);
    accounts.addAccount("Supplies Expense", Expense);
    accounts.addAccount("Depreciation Expense", Expense);
    accounts.addAccount("Common Stock", StockholdersEquity, 20);
    accounts.addAccount("Additional Paid in Capital", StockholdersEquity, 1280);
    accounts.addAccount("Retained Earnings", StockholdersEquity, 1400);
    //Net SE = $2700

    accounts.addAlias("Accounts Receivable", "ar");
    accounts.addAlias("Accumulated Depreciation on Equipment", "Acc Dep");
    accounts.addAlias("Accounts Payable", "ap");
    accounts.addAlias("Sales Revenue", "sales");
    accounts.addAlias("Additional Paid in Capital", "apic");
    accounts.addAlias("Supplies Expense", "supplies exp");

    //Make changes
    JournalEntryCreator entry1(Date("01/01/2024"), "Earn $200 in sales, with $150 in cash");
    JournalModificationCreator entry1ModificationCreator(&accounts, Date("01/01/2024"), "Earn $200 in sales, with $150 in cash");
    entry1.addJournalModification(entry1ModificationCreator.getJournalModification("dr cash, 150"));
    entry1.addJournalModification(entry1ModificationCreator.getJournalModification("dr ar, 50"));
    entry1.addJournalModification(entry1ModificationCreator.getJournalModification("cr sales, 200"));
    entryPoster.postModification(entry1.create());

    JournalEntryCreator entry2(Date("01/15/2024"), "Pay off Accounts Payable");
    JournalModificationCreator entry2ModificationCreator(&accounts, Date("01/15/2024"), "Pay off Accounts Payable");
    entry2.addJournalModification(entry2ModificationCreator.getJournalModification("dr ap, 600"));
    entry2.addJournalModification(entry2ModificationCreator.getJournalModification("cr cash, 600"));
    entryPoster.postModification(entry2.create());

    //Test that display works for values in the millions (companies working with greater values can pretend all values are in thousands, millions, etc. so realistically we don't need to reserve much more space for digits)
    JournalEntryCreator entry3(Date("01/20/2024"), "Buy new equipment using millions invested");
    JournalModificationCreator entry3ModificationCreator(&accounts, Date("01/20/2024"), "Buy new equipment using millions invested");
    entry3.addJournalModification(entry3ModificationCreator.getJournalModification("dr equipment, 1000000"));
    entry3.addJournalModification(entry3ModificationCreator.getJournalModification("cr apic, 1000000"));
    entryPoster.postModification(entry3.create());

    //Test negative display for millions
    JournalEntryCreator entry4(Date("01/30/2024"), "Liquidated new equipment for full value");
    JournalModificationCreator entry4ModificationCreator(&accounts, Date("01/30/2024"), "Liquidated new equipment for full value");
    entry4.addJournalModification(entry4ModificationCreator.getJournalModification("d cash, 1000000"));
    entry4.addJournalModification(entry4ModificationCreator.getJournalModification("c equipment, 1000000"));
    entryPoster.postModification(entry4.create());

    JournalEntryCreator entry5(Date("01/31/2024"), "Record supplies expense");
    JournalModificationCreator entry5ModificationCreator(&accounts, Date("01/31/2024"), "Record supplies expense");
    entry5.addJournalModification(entry5ModificationCreator.getJournalModification("d supplies expense, 80"));
    entry5.addJournalModification(entry5ModificationCreator.getJournalModification("c supplies, 80"));
    entryPoster.postModification(entry5.create());

    JournalEntryCreator entry6(Date("02/01/2024"), "Collect $500 from Accounts Receivable");
    JournalModificationCreator entry6ModificationCreator(&accounts, Date("02/01/2024"), "Collect $500 from Accounts Receivable");
    entry6.addJournalModification(entry6ModificationCreator.getJournalModification("d cash, 500"));
    entry6.addJournalModification(entry6ModificationCreator.getJournalModification("c ap, 500"));
    entryPoster.postModification(entry6.create());

    JournalEntryCreator entry7(Date("03/31/2024"), "Record accumulated depreciation");
    JournalModificationCreator entry7ModificationCreator(&accounts, Date("03/31/2024"), "Record accumulated depreciation");
    entry7.addJournalModification(entry7ModificationCreator.getJournalModification("d depreciation expense, 125"));
    entry7.addJournalModification(entry7ModificationCreator.getJournalModification("c acc dep, 125"));
    entryPoster.postModification(entry7.create());

    JournalEntryCreator entry8(Date("04/01/2024"), "Earn $300 in sales");
    JournalModificationCreator entry8ModificationCreator(&accounts, Date("04/01/2024"), "Earn $300 in sales");
    entry8.addJournalModification(entry8ModificationCreator.getJournalModification("d cash, 300"));
    entry8.addJournalModification(entry8ModificationCreator.getJournalModification("c sales, 300"));
    entryPoster.postModification(entry8.create());


    cout << "RUNNING ACCOUNT DISPLAY TESTS" << endl;

    AccountDisplayer displayCash1(&accounts.getAccount("Cash"), Period(Date("01/01/2024"), Date("02/31/2024")));
    displayCash1.display(cout);
   
    AccountDisplayer displayCash2(&accounts.getAccount("Cash"), Period(Date("03/01/2024"), Date("05/31/2024")));
    displayCash2.display(cout);

    AccountDisplayer displayCash(&accounts.getAccount("Cash"), Period(Date("01/01/2024"), Date("12/31/2024")));
    displayCash.display(cout);

    AccountDisplayer displayAR(&accounts.getAccount("ar"), Period(Date("01/01/2024"), Date("12/31/2024")));
    displayAR.display(cout);

    AccountDisplayer displayAccDep(&accounts.getAccount("acc dep"), Period(Date("03/01/2024"), Date("03/31/2024")));
    displayAccDep.display(cout);

    AccountDisplayer displayAP(&accounts.getAccount("ap"), Period(Date("01/01/2024"), Date("12/31/2024")));
    displayAP.display(cout);

    AccountDisplayer displaySales(&accounts.getAccount("sales"), Period(Date("01/01/2024"), Date("12/31/2024")));
    displaySales.display(cout);

    AccountDisplayer displayAPIC(&accounts.getAccount("apic"), Period(Date("01/01/2024"), Date("12/31/2024")));
    displayAPIC.display(cout);
    
    AccountDisplayer displayEquipment(&accounts.getAccount("equipment"), Period(Date("01/01/2024"), Date("12/31/2024")));
    displayEquipment.display(cout);
    cout << "CONCLUDED ACCOUNT DISPLAY TESTS" << endl;

    return 0;
}