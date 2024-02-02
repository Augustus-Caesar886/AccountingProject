#include "../header/Journal.h"

bool Journal::journalize(const JournalEntry& entry) {
    if(not entry.validate()) return false;
    if(entry.getDate().year != year) return false;

    entries.push_back(entry);
    return true;

    return false;
}