#include "../header/JournalEntryCreator.h"

#include <stdexcept>
using std::runtime_error;

const JournalEntry& JournalEntryCreator::create() const {
    if(not toCreate.validate()) throw runtime_error("Journal Entry not valid");

    return toCreate;
}