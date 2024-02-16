#include "../header/JournalModificationCreator.h"

#include <stdexcept>
using std::invalid_argument;

#include <sstream>
using std::basic_istringstream;
using std::string;

JournalModification JournalModificationCreator::getJournalModification(const string& modification) const {
    //Form (Dr. or Cr.) <Account Name Here>, amount
    ValueType valueType = ValueType::debit;
    string accountIdentifier = "";
    double amount = 0;
    basic_istringstream modificationParser(modification);

    string buffer = "";
    modificationParser >> buffer;

    switch(toupper(buffer[0])) {
        case 'D':
            valueType = ValueType::debit;
            break;
        case 'C':
            valueType = ValueType::credit;
            break;
        default:
            throw invalid_argument("Invalid value type in journal modification");
            break;
    }

    getline(modificationParser >> std::ws, accountIdentifier, ',');

    modificationParser >> amount;
    if(modificationParser.fail()) throw invalid_argument("Could not read total value change in journal modification");

    return JournalModification(amount, valueType, day, description, &accounts->getAccount(accountIdentifier));
}