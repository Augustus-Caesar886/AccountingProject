#ifndef QUARTER_RECORDS_H
#define QUARTER_RECORDS_H

#include "AccountRecords.h"
#include "MonthRecords.h"
#include "Date.h"

#include <vector>
using std::vector;

class QuarterRecords : public AccountRecords {
    private:
        DateUnit quarter;
        vector<MonthRecords> months;
        vector<JournalModification*> quarterRecords;
    public:
        QuarterRecords(DateUnit year, DateUnit quarter, ValueType valueType, double beginningBalance);
        DateUnit getQuarter() const { return quarter; }
        void addEntry(JournalModification*);
        const vector<JournalModification*> &getEntries() const { return quarterRecords; }
        const vector<MonthRecords> &getMonthRecords() const { return months; }
        void adjustPeriodBalances(double newValue); //Sets beginningBalance and endingBalance, only to be used on QuarterRecords objects with no entries
};

#endif