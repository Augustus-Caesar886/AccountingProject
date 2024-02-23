#include "../header/AccountDisplayer.h"

using std::ostream;
using std::endl;

#include <iomanip>
#include <sstream>

using std::string;
using std::ostringstream;

const unsigned DEFAULT_WIDTH = 11;

void displayEntry(ostream&, const JournalModification&);

void AccountDisplayer::display(ostream& toWrite) const {
    toWrite << "---";
    for(auto it : toDisplay->getName()) {
        toWrite << (char)std::toupper(it);
    }
    toWrite << "---" << endl;
    auto precision = toWrite.precision();
    toWrite.precision(2);
    toWrite << period.getStartDate().stringForm() << "\t" << std::right << std::setw(DEFAULT_WIDTH) << /* std::setfill(6) <<*/ std::fixed << toDisplay->getRecords().getMonthRecords(period.getStartDate().month).getBeginningBalance() << "\t| Beginning Balance" << endl;
    for(auto it : toDisplay->getEntries()) {
        displayEntry(toWrite, *it);
    }
    toWrite << period.getEndDate().stringForm() << "\t" << std::right << std::setw(DEFAULT_WIDTH) << /* std::setfill(6) <<*/ std::fixed << toDisplay->getRecords().getMonthRecords(period.getEndDate().month).getEndingBalance() << "\t| Ending Balance" << endl;
    toWrite << endl;

    toWrite.precision(precision);
}


void displayEntry(ostream& toWrite, const JournalModification& modification) {
    //Write date of transaction
    toWrite << modification.getDate().stringForm() << "\t" << std::right;

    //Write value amount, accounting for whether to include parentheses
    string displayValue = "";
    if(modification.get().second != modification.getAffectedAccount()->getBalanceType()) {
        displayValue += "(";
        toWrite << std::setw(DEFAULT_WIDTH + 1);
    } else {
        toWrite << std::setw(DEFAULT_WIDTH);
    }

    ostringstream ss;
    ss.precision(2);
    ss << std::fixed << modification.get().first;
    displayValue += ss.str();

    if(modification.get().second != modification.getAffectedAccount()->getBalanceType()) displayValue += ")";
    toWrite << displayValue;

    toWrite << "\t| " << modification.getDescription() << endl;
}