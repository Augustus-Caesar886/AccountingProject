#ifndef JOURNAL_MODIFICATION_CREATOR_H
#define JOURNAL_MODIFICATION_CREATOR_H

#include "AccountLibrary.h"
#include "Date.h"
#include "JournalModification.h"

#include <string>
using std::string;

class JournalModificationCreator {
    private:
        AccountLibrary* accounts;
        Date day;
        string description;
    public:
        JournalModificationCreator(AccountLibrary* accounts, const Date& date, const string& description) : accounts(accounts), day(date), description(description) {}
        JournalModification getJournalModification(const string& modification) const;
};

#endif