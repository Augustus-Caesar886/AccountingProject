#include "../header/AccountRecords.h"

#include <stdexcept>
using std::invalid_argument;


void AccountRecords::addEntry(JournalModification* entry) {
    if(entry->getDate().year != year) throw invalid_argument("Bad year recorded for ledger entry");

    if(entry->get().second == accountType) {
        endingBalance += entry->get().first;
    } else {
        endingBalance -= entry->get().first;
    }
}