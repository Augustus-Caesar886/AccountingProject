#include "../header/LedgerModification.h"

bool LedgerModification::operator==(const LedgerModification& operand) const {
    return amount == operand.amount and type == operand.type and day == operand.day and description == operand.description;
}