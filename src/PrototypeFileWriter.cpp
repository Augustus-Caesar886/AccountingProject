#include "../header/PrototypeFileWriter.h"

#include <string>
using std::string;

#include <fstream>
using std::endl;

string accountToChar(AccountType accountType) {
    switch(accountType) {
        case AccountType::Asset:
            return "a";
        case AccountType::Liability:
            return "l";
        case AccountType::Revenue:
            return "r";
        case AccountType::Expense:
            return "e";
        case AccountType::StockholdersEquity:
            return "s";
    }
}

void PrototypeFileWriter::writeToFile(ofstream& outputFile) {
    for(auto it : accounts) {
        outputFile << it.getName() << "," << (it.getBalanceType() == ValueType::debit ? "d," : "c,") << accountToChar(it.getAccountType()) << "," << it.getBeginningBalance() << endl;
    }

    for(unsigned i = 0; i < 3; ++i) {
        outputFile << "J,";
    }
    outputFile << "J" << endl;

    for(unsigned i = 0; i < accountNames.size(); ++i) {
        outputFile << (entries[i].get().second == ValueType::debit ? "d," : "c,") << accountNames[i] << "," << entries[i].get().first << "," << entries[i].getDate().stringForm() << "," << entries[i].getDescription() << endl;
    }
}