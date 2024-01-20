#ifndef CONTRA_EQUITY_ACCOUNT_H
#define CONTRA_EQUITY_ACCOUNT_H

#include "Account.h"

class ContraEquityAccount : public Account {
    public:
        ContraEquityAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::ContraEquity, year, beginningBalance) {}
};


#endif