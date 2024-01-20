#ifndef CONTRA_ASSET_ACCOUNT_H
#define CONTRA_ASSET_ACCOUNT_H

#include "Account.h"

class ContraAssetAccount : public Account {
    public:
        ContraAssetAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::credit, AccountType::ContraAsset, year, beginningBalance) {}
};


#endif