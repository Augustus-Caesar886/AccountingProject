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
        unordered_map<Account*, Account> contraLinker;
        unordered_map<string, Account*> nameLinker;
        DateUnit year;
        string toUpper(const string&) const; 
    public:
        AccountLibrary(DateUnit year) : year(year) {}
        DateUnit getYear() const { return year; }
        void addAccount(const string&, AccountType, double beginningBalance = 0);
        void linkAccount(const string&, const string&, AccountType, double beginningBalance = 0);
        Account* findLinked(const string&);
        bool addAlias(const string&, const string&);
        void removeAlias(const string&);
        Account& getAccount(const string& );
        const Account& getAccount(const string&) const ;
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