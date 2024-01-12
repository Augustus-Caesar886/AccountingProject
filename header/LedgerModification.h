#ifndef LEDGER_MODIFICATION_H
#define LEDGER_MODIFICATION_H

#include "AccountModification.h"

class LedgerModification : public AccountModification {
    public:
        LedgerModification(double amount, ValueType type, const Date &day, const string& description) : AccountModification(amount, type, day, description) {}
};

#endif