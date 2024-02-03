#include "../header/JournalEntryPoster.h"

bool JournalEntryPoster::postModification(const JournalEntry& entry) {
    if(not entry.validate()) return false;

    journal->journalize(entry);

    for(auto it : entry.getModifications()) {
        it.getAffectedAccount()->addEntry(&it);
    }
    return true;
}