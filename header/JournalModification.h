#ifndef JOURNAL_MODIFICATION_H
#define JOURNAL_MODIFICATION_H

#include "AccountModification.h"
#include "Account.h"

class JournalModification : public AccountModification {
    private:
        Account* affectedAccount;
    public:
        JournalModification(double amount, ValueType type, const Date &day, const string &description, Account* affectedAccount) : AccountModification(amount, type, day, description), affectedAccount(affectedAccount) {}
        Account* getAffectedAccount() { return affectedAccount; }
        const Account* getAffectedAccount() const { return affectedAccount; }
};

#endif