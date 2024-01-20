#ifndef EXPENSE_ACCOUNT_H
#define EXPENSE_ACCOUNT_H

#include "Account.h"

class ExpenseAccount : public Account {
    public:
        ExpenseAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::debit, AccountType::Expense, year, beginningBalance) {}
};


#endif