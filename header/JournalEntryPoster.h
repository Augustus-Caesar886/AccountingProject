#ifndef JOURNAL_ENTRY_POSTER_H
#define JOURNAL_ENTRY_POSTER_H

#include "AccountLibrary.h"
#include "Journal.h"
#include "JournalEntry.h"

class JournalEntryPoster {
    private:
        AccountLibrary* accounts;
        Journal* journal;
    public:
        JournalEntryPoster(Journal* journal, AccountLibrary* accounts) : journal(journal), accounts(accounts) {}
        bool postModification(const JournalEntry&);
};

#endif