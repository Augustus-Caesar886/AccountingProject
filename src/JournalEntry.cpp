#include "../header/JournalEntry.h"
#include <stdexcept>
using std::invalid_argument;

void JournalEntry::addModification(const JournalModification& modification) {
    if(modification.getDate() != day) throw invalid_argument("Modification dated " + modification.getDate().stringForm() + " not compatible with entry dated " + day.stringForm());

    if(lastEntryType == ValueType::credit and modification.get().second != ValueType::credit) throw invalid_argument("Attempting to add debit (or invalid) modification after credit modification(s) entered");
    accountsModified.push_back(modification);
    lastEntryType = modification.get().second;
}

bool JournalEntry::validate() const {
    double currValue = 0;
    for(auto it : accountsModified) {
        currValue += (it.get().second == ValueType::debit) ? it.get().first : -it.get().first;
    }
    return currValue == 0;
}