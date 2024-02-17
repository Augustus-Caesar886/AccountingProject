#ifndef JOURNAL_ENTRY_CREATOR_H
#define JOURNAL_ENTRY_CREATOR_H

#include "JournalEntry.h"

class JournalEntryCreator {
    private:
        JournalEntry toCreate;
    public:
        JournalEntryCreator(const Date& day, const string& description) : toCreate(day, description) {}
        void addJournalModification(const JournalModification& modification) { toCreate.addModification(modification); }
        const JournalEntry& create() const;
};

#endif