#ifndef MONTH_RECORDS_H
#define MONTH_RECORDS_H

#include "AccountRecords.h"
#include "Date.h"
#include "JournalModification.h"
#include "ValueType.h"

#include <vector>
using std::vector;

class MonthRecords : public AccountRecords {
    private:
        DateUnit month;
        vector<JournalModification*> entries;
    public:
        MonthRecords(DateUnit year, DateUnit month, ValueType valueType, double beginningBalance) : AccountRecords(year, valueType, beginningBalance), month(month) {}
        DateUnit getMonth() const { return month; }
        void addEntry(JournalModification*);
        const vector<JournalModification*> &getEntries() const { return entries; }
};

#endif