#include "../header/AccountDisplayer.h"

using std::ostream;
using std::endl;

#include <iomanip>

void displayEntry(ostream&, const JournalModification&);

void AccountDisplayer::display(ostream& toWrite) const {
    toWrite << "---";
    for(auto it : toDisplay->getName()) {
        toWrite << (char)std::toupper(it);
    }
    toWrite << "---" << endl;

    toWrite << period.getStartDate().stringForm() << "\t" << std::right << std::setw(6) << /* std::setfill(6) <<*/ (double)(toDisplay->getRecords().getQuarterRecords()[(period.getStartDate().month-1) / 4].getMonthRecords()[(period.getStartDate().month-1) % 3].getBeginningBalance()) << "\t" << "Beginning Balance" << endl;
    for(auto it : toDisplay->getEntries()) {
        displayEntry(toWrite, *it);
    }

    toWrite << endl << endl;
}


void displayEntry(ostream& toWrite, const JournalModification& modification) {
    toWrite << modification.getDate().day << endl;
}