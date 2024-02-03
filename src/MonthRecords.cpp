#include "../header/MonthRecords.h"

#include <stdexcept>
using std::invalid_argument;

void MonthRecords::addEntry(JournalModification* entry) {
    if(entry->getDate().month != month) throw invalid_argument("Invalid month argument");

    AccountRecords::addEntry(entry);
    entries.push_back(entry);
}