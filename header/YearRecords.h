#ifndef YEAR_RECORDS_H
#define YEAR_RECORDS_H

#include "AccountRecords.h"
#include "QuarterRecords.h"
#include "JournalModification.h"
#include "Date.h"

class YearRecords : public AccountRecords {
    private:
        vector<QuarterRecords> quarters;
        vector<JournalModification*> entries;
    public:
        YearRecords(DateUnit, ValueType, double);
        void addEntry(JournalModification*);
        const vector<QuarterRecords> &getQuarterRecords() const { return quarters; }
        const vector<JournalModification*> &getEntries() const { return entries; }
        const MonthRecords &getMonthRecords(DateUnit month) const { return quarters[(month-1) / 3].getMonthRecords()[(month-1) % 3]; }
};

#endif