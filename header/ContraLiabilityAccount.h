#ifndef CONTRA_LIABILITY_ACCOUNT_H
#define CONTRA_LIABILITY_ACCOUNT_H

#include "Account.h"

class ContraLiabilityAccount : public Account {
    public:
        ContraLiabilityAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::ContraLiability, year, beginningBalance) {}
};


#endif