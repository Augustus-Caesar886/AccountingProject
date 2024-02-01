#ifndef ACCOUNT_LIBRARY_H
#define ACCOUNT_LIBRARY_H

#include <list>
using std::list;

#include <unordered_map>
using std::unordered_map;

#include <string>
using std::string;

#include "Accounts.h"

class AccountLibrary {
    private:
        list<AssetAccount> assets;
        list<LiabilityAccount> liabilities;
        list<StockholdersEquityAccount> stockholdersEquity;
        list<ContraEquityAccount> lessEquity;
        list<RevenueAccount> revenues;
        list<ExpenseAccount> expenses;
        list<GainAccount> gains;
        list<LossAccount> losses;
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
        const list<AssetAccount> getAssets() const { return assets; }
        const list<LiabilityAccount> getLiabilities() const { return liabilities; }
        const list<StockholdersEquityAccount> getStockholdersEquity() const { return stockholdersEquity; }
        const list<ContraEquityAccount> getContraEquity() const { return lessEquity; }
        const list<RevenueAccount> getRevenues() const { return revenues; }
        const list<ExpenseAccount> getExpenses() const { return expenses; }
        const list<GainAccount> getGains() const { return gains; }
        const list<LossAccount> getLosses() const { return losses; }
};

#endif