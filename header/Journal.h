#ifndef JOURNAL_H
#define JOURNAL_H

#include "Date.h"
#include "JournalEntry.h"

#include <list>
using std::list;

class Journal {
    private:
        DateUnit year;
        list<JournalEntry> entries;
    public:
        Journal(const DateUnit &year) : year(year) {}
        bool journalize(const JournalEntry&);
        const list<JournalEntry> &getEntries() const { return entries; }
        DateUnit getYear() const { return year; }
};

#endif