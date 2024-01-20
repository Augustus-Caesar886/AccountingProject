#ifndef REVENUE_ACCOUNT_H
#define REVENUE_ACCOUNT_H

#include "Account.h"

class RevenueAccount : public Account {
    public:
        RevenueAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::credit, AccountType::Revenue, year, beginningBalance) {}
};


#endif