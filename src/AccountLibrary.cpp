#include "../header/AccountLibrary.h"

#include <string>
using std::string;

#include <stdexcept>
using std::invalid_argument;

string AccountLibrary::toUpper(const string& toModify) const {
    string ret = toModify;
    for(auto& c : ret) {
        c = toupper(c);
    }
    return ret;
}

void AccountLibrary::addAccount(const string& name, AccountType accountType, double beginningBalance) {
    switch(accountType) {
        case AccountType::Asset:
            assets.push_back(AssetAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &assets.back());
            break;
        case AccountType::Liability:
            liabilities.push_back(LiabilityAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &liabilities.back());
            break;
        case AccountType::StockholdersEquity:
            stockholdersEquity.push_back(StockholdersEquityAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &stockholdersEquity.back());
            break;
        case AccountType::ContraEquity:
            lessEquity.push_back(ContraEquityAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &lessEquity.back());
            break;
        case AccountType::Revenue:
            revenues.push_back(RevenueAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &revenues.back());
            break;
        case AccountType::Expense:
            expenses.push_back(ExpenseAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &expenses.back());
            break;
        case AccountType::GAIN:
            gains.push_back(GainAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &gains.back());
            break;
        case AccountType::LOSS:
            losses.push_back(LossAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &losses.back());
            break;
        case AccountType::Dividends:
            dividends.push_back(DividendsAccount(name, year, beginningBalance));
            nameLinker.emplace(toUpper(name), &dividends.back());
            break;
        default:
            break;
    }
}

Account& AccountLibrary::getAccount(const string& alias) {
    if(nameLinker.count(toUpper(alias)) == 0) throw invalid_argument("No such alias " + alias);
    return *(nameLinker.find(toUpper(alias))->second); 
}

const Account& AccountLibrary::getAccount(const string& alias) const { 
    if(nameLinker.count(toUpper(alias)) == 0) throw invalid_argument("No such alias " + alias);
    return *(nameLinker.find(toUpper(alias))->second); 
}

void AccountLibrary::linkAccount(const string& originalAccount, const string& contraAccount, AccountType accountType, double beginningBalance) {
    switch(accountType) {
        case AccountType::ContraAsset:
            contraLinker.emplace(&getAccount(originalAccount), ContraAssetAccount(contraAccount, year, beginningBalance));
            nameLinker.emplace(toUpper(contraAccount), findLinked(originalAccount));
            break;
        case AccountType::ContraLiability:
            contraLinker.emplace(&getAccount(originalAccount), ContraLiabilityAccount(contraAccount, year, beginningBalance));
            nameLinker.emplace(toUpper(contraAccount), findLinked(originalAccount));
            break;
        case AccountType::ContraEquity:
            contraLinker.emplace(&getAccount(originalAccount), ContraEquityAccount(contraAccount, year, beginningBalance));
            nameLinker.emplace(toUpper(contraAccount), findLinked(originalAccount));
            break;
        case AccountType::ContraRevenue:
            contraLinker.emplace(&getAccount(originalAccount), ContraRevenueAccount(contraAccount, year, beginningBalance));
            nameLinker.emplace(toUpper(contraAccount), findLinked(originalAccount));
            break;
        case AccountType::ContraExpense:
            contraLinker.emplace(&getAccount(originalAccount), ContraExpenseAccount(contraAccount, year, beginningBalance));
            nameLinker.emplace(toUpper(contraAccount), findLinked(originalAccount));
            break;
        default:
            break;
    }
}

Account* AccountLibrary::findLinked(const string& name) {
    if(contraLinker.count(&getAccount(name)) == 0) return nullptr;

    return &contraLinker.find(&getAccount(name))->second;
}

bool AccountLibrary::addAlias(const string& existingAlias, const string& newAlias) {
    if(nameLinker.count(toUpper(newAlias)) != 0) return false;

    nameLinker.emplace(toUpper(newAlias), &getAccount(existingAlias));
    return true;
}

void AccountLibrary::removeAlias(const string& alias) {
    nameLinker.erase(toUpper(alias));
}