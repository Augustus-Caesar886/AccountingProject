#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "YearRecords.h"
#include "ValueType.h"
#include "Date.h"
#include "JournalModification.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

enum AccountType {
    Asset, Liability, StockholdersEquity, Revenue, Expense, GAIN, LOSS, Dividends, ContraAsset, ContraLiability, ContraEquity, ContraRevenue, ContraExpense
};

class Account {
    protected:
        string name;
        YearRecords records;
        DateUnit year;
        ValueType valueType;
        AccountType accountType;
        Account(const string& name, ValueType valueType, AccountType accountType, DateUnit year, double beginningBalance = 0) : name(name), valueType(valueType), accountType(accountType), year(year), records(year, valueType, beginningBalance) {}
    public:
        const string& getName() const { return name; }
        double getBalance() const { return records.getEndingBalance(); }
        double getBeginningBalance() const { return records.getBeginningBalance(); }
        ValueType getBalanceType() const { return valueType; }
        AccountType getAccountType() const { return accountType; }
        DateUnit getYear() const { return year; }
        void addEntry(JournalModification*);
        const vector<JournalModification*> &getEntries() const { return records.getEntries(); }
        const vector<JournalModification*> &getQuartersEntries(DateUnit quarter) const { return records.getQuarterRecords()[quarter-1].getEntries(); }
        const vector<JournalModification*> &getMonthsEntries(DateUnit month) const { return records.getQuarterRecords()[(month-1) / 3].getMonthRecords()[(month-1) % 3].getEntries(); }
        const YearRecords &getRecords() const { return records; }

        bool operator==(const Account&) const;
};

#endif