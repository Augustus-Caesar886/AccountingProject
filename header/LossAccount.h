#ifndef LOSS_ACCOUNT_H
#define LOSS_ACCOUNT_H

#include "Account.h"

class LossAccount : public Account {
    public:
        LossAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::LOSS, year, beginningBalance) {}
};


#endif