#ifndef LIABILITY_ACCOUNT_H
#define LIABILITY_ACCOUNT_H

#include "Account.h"

class LiabilityAccount : public Account {
    public:
        LiabilityAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::credit, AccountType::Liability, year, beginningBalance) {}
};


#endif