#ifndef JOURNAL_ENTRY_H
#define JOURNAL_ENTRY_H

#include "JournalModification.h"
#include "Date.h"

#include <vector>
using std::vector;

#include <string>
using std::string;

class JournalEntry {
    private:
        vector<JournalModification> accountsModified;
        Date day;
        string description;
        ValueType lastEntryType;
    public:
        JournalEntry(const Date &day, const string& description) : day(day), description(description), lastEntryType(ValueType::debit) {}
        void addModification(const JournalModification&);
        bool validate() const;
        const Date& getDate() const { return day; }
        const string& getDescription() const { return description; }
        const vector<JournalModification> &getModifications() const { return accountsModified; }
};

#endif