#include "../header/YearRecords.h"
#include "../header/JournalModification.h"

#include <stdexcept>
using std::invalid_argument;

using std::to_string;

YearRecords::YearRecords(DateUnit year, ValueType valueType, double beginningBalance) : AccountRecords(year, valueType, beginningBalance) {
    for(unsigned i = 1; i <= 4; ++i) {
        quarters.push_back(QuarterRecords(year, i, valueType, beginningBalance));
    }
}

void YearRecords::addEntry(JournalModification* entry) {
    if(entry->getDate().year != year) throw invalid_argument("Incompatible year");
    for(unsigned i = (entry->getDate().month-1) / 3 + 1; i < 4; ++i) {
        if(quarters[i].getEntries().size() != 0) throw invalid_argument("Entry dated " + entry->getDate().stringForm() + " is invalid for year " + to_string(year));
    }

    quarters[(entry->getDate().month-1) / 3].addEntry(entry);
    AccountRecords::addEntry(entry);
    entries.push_back(entry);

    for(unsigned i = (entry->getDate().month-1) / 3 + 1; i < 4; ++i) { //Prepare future records
        quarters[i].adjustPeriodBalances(quarters[(entry->getDate().month-1) / 3].getEndingBalance());
    }
}