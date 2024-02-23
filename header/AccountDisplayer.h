#ifndef ACCOUNT_DISPLAYER_H
#define ACCOUNT_DISPLAYER_H

#include <iostream>
using std::ostream;

#include "Account.h"
#include "Period.h"

class AccountDisplayer {
    private:
        Account* toDisplay;
        Period period;
    public:
        AccountDisplayer(Account* toDisplay, const Period& period) : toDisplay(toDisplay), period(period) {}

        void display(ostream&) const;
};

#endif