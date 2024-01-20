#ifndef CONTRA_EXPENSE_ACCOUNT_H
#define CONTRA_EXPENSE_ACCOUNT_H

#include "Account.h"

class ContraExpenseAccount : public Account {
    public:
        ContraExpenseAccount(const string& name, DateUnit year, double beginningBalance = 0) : Account(name, ValueType::credit, AccountType::ContraExpense, year, beginningBalance) {}
};


#endif