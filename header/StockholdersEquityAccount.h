#ifndef STOCKHOLDERS_EQUITY_ACCOUNT_H
#define STOCKHOLDERS_EQUITY_ACCOUNT_H

#include "Account.h"

class StockholdersEquityAccount : public Account {
    public:
        StockholdersEquityAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::credit, AccountType::StockholdersEquity, year, beginningBalance) {}
};


#endif