#include "../header/PrototypeFileReader.h"

ValueType PrototypeFileReader::charToValueType(char c) {
    switch(c) {
        case 'd':
            return ValueType::debit;
        case 'c':
            return ValueType::credit;
    }
}

AccountType PrototypeFileReader::charToAccountType(char c) {
    switch(c) {
        case 'a':
            return AccountType::Asset;
        case 'l':
            return AccountType::Liability;
        case 's':
            return AccountType::StockholdersEquity;
        case 'r':
            return AccountType::Revenue;
        case 'e':
            return AccountType::Expense;
    }
}

PrototypeFileReader::PrototypeFileReader(ifstream &inputFile, DateUnit year) {
    try {
        while(inputFile) {
            AccountInitializer toAdd;
            char nextChar;
            getline(inputFile, toAdd.name, ',');
            inputFile >> nextChar;
            inputFile.get();
            toAdd.valueType = charToValueType(nextChar);
            inputFile >> nextChar;
            inputFile.get();
            toAdd.accountType = charToAccountType(nextChar);
            inputFile >> toAdd.beginningBalance;
            if(nextChar == 'J') throw exception();
            accountLibrary.emplace(toAdd.name, AccountDriver(toAdd.name, toAdd.valueType, toAdd.accountType, year, toAdd.beginningBalance));
            inputFile.get();
        }
    } catch(exception e) {}
    inputFile.clear();
    string temp;
    getline(inputFile, temp);
    while(inputFile) {
        LedgerInitializer toAdd;
        string nextString;
        getline(inputFile, nextString, ',');
        toAdd.valueType = charToValueType(nextString[0]);
        getline(inputFile, toAdd.accName, ',');
        inputFile >> toAdd.amount;
        inputFile.get();
        getline(inputFile, toAdd.date, ',');
        getline(inputFile, toAdd.description);
        try {
            accountLibrary.find(toAdd.accName)->second.addEntry(LedgerModification(toAdd.amount, toAdd.valueType, Date(toAdd.date), toAdd.description));
        } catch(invalid_argument e) {
            continue;
        }
    }   
}

vector<Account> PrototypeFileReader::getAccounts() const {
    vector<Account> accounts;
    accounts.reserve(accountLibrary.size());
    for(auto it : accountLibrary) {
        accounts.push_back(it.second);
    }
    return accounts;
}