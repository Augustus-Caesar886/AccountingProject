#ifndef ACCOUNT_LIBRARY_H
#define ACCOUNT_LIBRARY_H

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include <string>
using std::string;

#include "Accounts.h"

class AccountLibrary {
    private:
        vector<AssetAccount> assets;
        vector<LiabilityAccount> liabilities;
        vector<StockholdersEquityAccount> stockholdersEquity;
        vector<ContraEquityAccount> lessEquity;
        vector<RevenueAccount> revenues;
        vector<ExpenseAccount> expenses;
        vector<GainAccount> gains;
        vector<LossAccount> losses;
        unordered_map<Account*, Account*> contraLinker;
        unordered_map<string, Account*> nameLinker;
        string toUpper(const string&); 
        bool compareStrings(const string& st1, const string& st2) { return toUpper(st1) == toUpper(st2); }
    public:
        AccountLibrary();
        void addAccount(const string&, AccountType, double beginningBalance = 0);
        void addAlias(const string&, const string&);
        Account& getAccount(const string& alias) { return *(nameLinker.find(alias)->second); }
        const Account& getAccount(const string& alias) const { return *(nameLinker.find(alias)->second); }
        const vector<AssetAccount> getAssets() const { return assets; }
        const vector<LiabilityAccount> getLiabilities() const { return liabilities; }
        const vector<StockholdersEquityAccount> getStockholdersEquity() const { return stockholdersEquity; }
        const vector<ContraEquityAccount> getContraEquity() const { return lessEquity; }
        const vector<RevenueAccount> getRevenues() const { return revenues; }
        const vector<ExpenseAccount> getExpenses() const { return expenses; }
        const vector<GainAccount> getGains() const { return gains; }
        const vector<LossAccount> getLosses() const { return losses; }
};

#endif