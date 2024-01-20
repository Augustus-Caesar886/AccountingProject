#ifndef DIVIDENDS_ACCOUNT_H
#define DIVIDENDS_ACCOUNT_H

#include "Account.h"

class DividendsAccount : public Account {
    public:
        DividendsAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::Dividends, year, beginningBalance) {}
};


#endif