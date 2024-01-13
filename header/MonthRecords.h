#ifndef MONTH_RECORDS_H
#define MONTH_RECORDS_H

#include "AccountRecords.h"
#include "Date.h"
#include "LedgerModification.h"
#include "ValueType.h"

#include <vector>
using std::vector;

class MonthRecords : public AccountRecords {
    private:
        DateUnit month;
        vector<LedgerModification> entries;
    public:
        MonthRecords(DateUnit year, DateUnit month, ValueType valueType, double beginningBalance) : AccountRecords(year, valueType, beginningBalance), month(month) {}
        DateUnit getMonth() const { return month; }
        void addEntry(const LedgerModification&);
        const vector<LedgerModification> &getEntries() const { return entries; }
};

#endif