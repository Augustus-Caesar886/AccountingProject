#include "../header/Account.h"

void Account::addEntry(JournalModification* entry) {
    records.addEntry(entry);
}

bool Account::operator==(const Account& rhs) const {
    return name == rhs.name and valueType == rhs.valueType and accountType == rhs.accountType and getBeginningBalance() == rhs.getBeginningBalance() and getBalance() == rhs.getBalance();
}