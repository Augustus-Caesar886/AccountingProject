#ifndef GAIN_ACCOUNT_H
#define GAIN_ACCOUNT_H

#include "Account.h"

class GainAccount : public Account {
    public:
        GainAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::credit, AccountType::GAIN, year, beginningBalance) {}
};


#endif