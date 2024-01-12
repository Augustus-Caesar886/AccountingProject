#ifndef ACCOUNT_MODIFICATION_H
#define ACCOUNT_MODIFICATION_H

#include "ValueType.h"
#include "Date.h"

#include <string>
using std::string;

#include <utility>
using std::pair;

class AccountModification {
    protected:
        double amount;
        ValueType type;
        Date day;
        string description;
        AccountModification(double amount, ValueType type, const Date &day, const string& description) : amount(amount), type(type), day(day), description(description) {}
    public:
        pair<double, ValueType> get() const { return pair(amount, type); }
        const Date& getDate() const { return day; }
        const string& getDescription() const { return description; }
};

#endif