#ifndef CONTRA_REVENUE_ACCOUNT_H
#define CONTRA_REVENUE_ACCOUNT_H

#include "Account.h"

class ContraRevenueAccount : public Account {
    public:
        ContraRevenueAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::ContraRevenue, year, beginningBalance) {}
};


#endif