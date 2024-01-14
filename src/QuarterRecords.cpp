#include "../header/QuarterRecords.h"

#include <stdexcept>
using std::invalid_argument;

using std::to_string;

QuarterRecords::QuarterRecords(DateUnit year, DateUnit quarter, ValueType valueType, double beginningBalance) : AccountRecords(year, valueType, beginningBalance), quarter(quarter) {
    if(quarter < 1 or quarter > 4) throw invalid_argument("Accounting quarter not within bounds");

    for(unsigned i = 1; i <= 3; ++i) {
        months.push_back(MonthRecords(year, i + 3 * (quarter - 1), valueType, beginningBalance));
    }
}

void QuarterRecords::addEntry(const LedgerModification& entry) {
    if(entry.getDate().month < (quarter - 1) * 3 + 1 or entry.getDate().month > quarter * 3) throw invalid_argument("Invalid month for Quarter " + to_string(quarter));

    AccountRecords::addEntry(entry);
    months[entry.getDate().month - 3 * (quarter-1) - 1].addEntry(entry);
    quarterRecords.push_back(entry);
}