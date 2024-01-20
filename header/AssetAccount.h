#ifndef ASSET_ACCOUNT_H
#define ASSET_ACCOUNT_H

#include "Account.h"

class AssetAccount : public Account {
    public:
        AssetAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::Asset, year, beginningBalance) {}
};


#endif