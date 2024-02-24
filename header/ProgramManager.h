#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include "AccountLibrary.h"
#include "Journal.h"
#include "JournalEntryPoster.h"

class ProgramManager {
    private:
        AccountLibrary accounts;
        Journal journal;
        JournalEntryPoster entryPoster;
    public:
        ProgramManager(DateUnit year) : accounts(year), journal(year) {}

        AccountLibrary &getAccountLibrary() { return accounts; }
        const AccountLibrary& getAccountLibrary() const { return accounts; }

        const Journal& getJournal() const { return accounts; }

        bool postEntry(const JournalEntry& entry) { return entryPoster.postModification(entry); }
};

#endif