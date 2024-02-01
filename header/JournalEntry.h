#ifndef JOURNAL_ENTRY_H
#define JOURNAL_ENTRY_H

#include "JournalModification.h"
#include "Date.h"

#include <vector>
using std::vector;

class JournalEntry {
    private:
        vector<JournalModification> accountsModified;
        Date day;
    public:
        JournalEntry(const Date &day) : day(day) {}
        void addModification(const JournalModification&);
        bool validate() const;
        const vector<JournalModification> &getModifications() const { return accountsModified; }
};

#endif