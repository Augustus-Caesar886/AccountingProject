#include "../header/JournalEntryPoster.h"

bool JournalEntryPoster::postModification(const JournalEntry& entry) {
    if(not entry.validate()) return false;

    if(not journal->journalize(entry)) return false;

    for(std::list<JournalModification>::iterator it = journal->getEntries().back().getModifications().begin(); it != journal->getEntries().back().getModifications().end(); ++it) {
        it->getAffectedAccount()->addEntry(&(*it));
    }
    return true;
}