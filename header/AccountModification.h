#ifndef ACCOUNT_MODIFICATION_H
#define ACCOUNT_MODIFICATION_H

#include "ValueType.h"

#include <string>
using std::string;

#include <utility>
using std::pair;

class AccountModification {
    protected:
        double amount;
        ValueType type;
        string description;
    public:
        AccountModification(double amount, ValueType type, const string& description) : amount(amount), type(type), description(description) {}
        pair<double, ValueType> get() const { return pair(amount, type); }
        const string& getDescription() const { return description; }
};

#endif