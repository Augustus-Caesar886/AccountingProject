#include "../header/Account.h"

void Account::addEntry(const LedgerModification& entry) {
    records.addEntry(entry);
}