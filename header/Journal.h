#ifndef JOURNAL_H
#define JOURNAL_H

#include "Date.h"
#include "JournalEntry.h"

#include <vector>
using std::vector;

class Journal {
    private:
        DateUnit year;
        vector<JournalEntry> entries;
    public:
        Journal(const DateUnit &year) : year(year) {}
        bool journalize(const JournalEntry&);
        const vector<JournalEntry> &getEntries() const { return entries; }
        DateUnit getYear() const { return year; }
};

#endif