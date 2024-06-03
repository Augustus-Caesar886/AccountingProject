#include "../header/ProgramManager.h"
#include "../header/JournalEntryCreator.h"
#include "../header/JournalModificationCreator.h"

void ProgramManager::postClosingEntry() {
    double totalRevenues = 0;
    double totalExpenses = 0;
    double totalDividends = 0;

    Date day = Date("12/31/2024");
    string description = "CJE";
    JournalEntryCreator entryCreator(day, description);
    JournalModificationCreator modificationCreator(&accounts, day, description);
    
    for (auto it : accounts.getRevenues()) {
        totalRevenues += it.getBalance();
        if(accounts.findLinked(it.getName())) {
            totalRevenues -= accounts.findLinked(it.getName())->getBalance();
        }
    }

    for (auto it : accounts.getExpenses()) {
        totalExpenses += it.getBalance();
        if(accounts.findLinked(it.getName())) {
            totalExpenses -= accounts.findLinked(it.getName())->getBalance();
        }
    }

    for (auto it : accounts.getDividends()) {
        totalExpenses += it.getBalance();
        if(accounts.findLinked(it.getName())) {
            totalExpenses -= accounts.findLinked(it.getName())->getBalance();
        }
    }

    for(auto it : accounts.getRevenues()) {
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. " + it.getName() + ", " + std::to_string(it.getBalance())));
    }

    for(auto it : accounts.getExpenses()) {
        if(accounts.findLinked(it.getName())) {
            entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. " + accounts.findLinked(it.getName())->getName() + ", " + std::to_string(accounts.findLinked(it.getName())->getBalance())));
        }
    }

    if(totalRevenues - totalExpenses - totalDividends < 0) {
        entryCreator.addJournalModification(modificationCreator.getJournalModification("dr. Retained Earnings, " + std::to_string(-totalRevenues + totalExpenses + totalDividends)));
    }

    for(auto it : accounts.getExpenses()) {
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr. " + it.getName() + ", " + std::to_string(it.getBalance())));
    }

    for(auto it : accounts.getRevenues()) {
        if(accounts.findLinked(it.getName())) {
            entryCreator.addJournalModification(modificationCreator.getJournalModification("cr. " + accounts.findLinked(it.getName())->getName() + ", " + std::to_string(accounts.findLinked(it.getName())->getBalance())));
        }
    }

    for(auto it : accounts.getDividends()) {
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr. " + it.getName() + ", " + std::to_string(it.getBalance())));
    }

    if(totalRevenues - totalExpenses - totalDividends >= 0) {
        entryCreator.addJournalModification(modificationCreator.getJournalModification("cr. Retained Earnings, " + std::to_string(totalRevenues - totalExpenses - totalDividends)));
    }

    postEntry(entryCreator.create());
}