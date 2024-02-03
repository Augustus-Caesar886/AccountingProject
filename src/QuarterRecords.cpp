#include "../header/QuarterRecords.h"

#include <stdexcept>
using std::invalid_argument;

using std::to_string;

QuarterRecords::QuarterRecords(DateUnit year, DateUnit quarter, ValueType valueType, double beginningBalance) : AccountRecords(year, valueType, beginningBalance), quarter(quarter) {
    if(quarter < 1 or quarter > 4) throw invalid_argument("Accounting quarter not within bounds");

    for(unsigned i = 1; i <= 3; ++i) {
        months.push_back(MonthRecords(year, i + 3 * (quarter - 1), valueType, beginningBalance));
    }
}

void QuarterRecords::addEntry(JournalModification* entry) {
    if(entry->getDate().month < (quarter - 1) * 3 + 1 or entry->getDate().month > quarter * 3) throw invalid_argument("Invalid month for Quarter " + to_string(quarter));
    for(unsigned i = entry->getDate().month - 3 * (quarter-1); i < 3; ++i) {
        if(months[i].getEntries().size() != 0) throw invalid_argument("Entry dated " + entry->getDate().stringForm() + " is invalid for quarter " + to_string(quarter));
    }

    AccountRecords::addEntry(entry);
    if(entry->getDate().month - 3 * (quarter-1) - 1 != 0 and months[entry->getDate().month - 3 * (quarter-1) - 1].getEntries().size() == 0) { //Adjust past records
        if(entry->getDate().month - 3 * (quarter-1) - 2 != 0 and months[entry->getDate().month - 3 * (quarter-1) - 2].getEntries().size() == 0) {
            months[entry->getDate().month - 3 * (quarter-1) - 2].setBeginningBalance(months[0].getEndingBalance());
            months[entry->getDate().month - 3 * (quarter-1) - 2].setEndingBalance(months[0].getEndingBalance());
        }
        months[entry->getDate().month - 3 * (quarter-1) - 1].setBeginningBalance(months[entry->getDate().month - 3 * (quarter-1) - 2].getEndingBalance());
        months[entry->getDate().month - 3 * (quarter-1) - 1].setEndingBalance(months[entry->getDate().month - 3 * (quarter-1) - 2].getEndingBalance());
    }
    months[entry->getDate().month - 3 * (quarter-1) - 1].addEntry(entry);
    quarterRecords.push_back(entry);

    for(unsigned i = entry->getDate().month - 3 * (quarter-1); i < 3; ++i) { //Prepare future records
        months[i].setBeginningBalance(months[entry->getDate().month - 3 * (quarter-1) - 1].getEndingBalance());
        months[i].setEndingBalance(months[entry->getDate().month - 3 * (quarter-1) - 1].getEndingBalance());
    }
}

void QuarterRecords::adjustPeriodBalances(double newValue) {
    if(quarterRecords.size() != 0) throw invalid_argument("Attempting to change BB and EB of a quarter with existing ledger entries");

    beginningBalance = endingBalance = newValue;
    for(unsigned i = 0; i < months.size(); ++i) {
        months[i].setBeginningBalance(newValue);
        months[i].setEndingBalance(newValue);
    }
}