#ifndef YEAR_RECORDS_H
#define YEAR_RECORDS_H

#include "AccountRecords.h"
#include "QuarterRecords.h"
#include "LedgerModification.h"
#include "Date.h"

class YearRecords : public AccountRecords {
    private:
        vector<QuarterRecords> quarters;
        vector<LedgerModification> entries;
    public:
        YearRecords(DateUnit, ValueType, double);
        void addEntry(const LedgerModification&);
        const vector<QuarterRecords> &getQuarterRecords() const { return quarters; }
        const vector<LedgerModification> &getEntries() const { return entries; }
};

#endif