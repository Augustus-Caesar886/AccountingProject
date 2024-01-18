#ifndef PROTOTYPE_FILE_READER
#define PROTOTYPE_FILE_READER

#include <fstream>
using std::ifstream;

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::getline;

#include "Account.h"

#include <stdexcept>
using std::exception;
using std::invalid_argument;

#include <unordered_map>
using std::unordered_map;

struct AccountInitializer {
    string name;
    ValueType valueType;
    AccountType accountType;
    DateUnit year;
    double beginningBalance = 0;
};

struct LedgerInitializer {
    string accName;
    double amount;
    ValueType valueType;
    string date;
    string description;
};

class AccountDriver : public Account {
    public:
        AccountDriver(const string& name, ValueType valueType, AccountType accountType, DateUnit year, double beginningBalance = 0) : Account(name, valueType, accountType, year, beginningBalance) {}
};

class PrototypeFileReader {
    private:
        unordered_map<string, Account> accountLibrary;
        ValueType charToValueType(char);
        AccountType charToAccountType(char);
    public:
        PrototypeFileReader(ifstream &inputFile, DateUnit year);
        Account& getAccount(const string& name) { return accountLibrary.find(name)->second; }
        
};




#endif