#ifndef ACCOUNT_RECORDS_H
#define ACCOUNT_RECORDS_H

#include "Date.h"
#include "ValueType.h"
#include "LedgerModification.h"

#include <vector>
using std::vector;

class AccountRecords {
    protected:
        double beginningBalance, endingBalance;
        DateUnit year;
        ValueType accountType;
        AccountRecords(DateUnit year, ValueType accountType, double beginningBalance = 0) : year(year), accountType(accountType), beginningBalance(beginningBalance), endingBalance(beginningBalance) {}
    public:
        ValueType getValueType() const { return accountType; }
        double getBeginningBalance() const { return beginningBalance; }
        double getEndingBalance() const { return endingBalance; }
        void setBeginningBalance(double bb) { beginningBalance = bb; }
        void setEndingBalance(double eb) { endingBalance = eb; }
        DateUnit getYear() const { return year; }
        virtual void addEntry(const LedgerModification&);
        virtual const vector<LedgerModification>& getEntries() const = 0;
};

#endif