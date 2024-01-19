#ifndef PROTOTYPE_FILE_WRITER
#define PROTOTYPE_FILE_WRITER

#include "Account.h"

#include <fstream>
using std::ofstream;

using std::string;

class PrototypeFileWriter {
    private:
        vector<LedgerModification> entries;
        vector<string> accountNames;
        vector<Account> accounts;
    public:
        PrototypeFileWriter(const vector<LedgerModification>& entries, const vector<string>& accountNames, const vector<Account>& accounts) : entries(entries), accountNames(accountNames), accounts(accounts) {}
        void writeToFile(ofstream&);
};

#endif