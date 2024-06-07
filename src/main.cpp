#include <iostream>
using std::cout;
using std::endl;
using std::cin;
using std::istream;
using std::string;

#include <fstream>
using std::ofstream;
using std::ifstream;

#include <exception>
using std::exception;

#include "../header/Date.h"
#include "../header/ProgramManager.h"

#include <sstream>
using std::basic_istringstream;

const string PROGRAM_NAME = "Accounting Software";
const string CURRENT_BUILD = "0.2.0-alpha";
const string DATAPATH = "CompanyData/";
const string ALIAS_DIVIDER = "ALIASES";
const string JOURNAL_DIVIDER = "JOURNAL";

enum MenuOption {
    RECORD_TRANSACTION, VIEW_SUMMARY, PREPARE_DOCUMENTS, MANAGE_ACCOUNTS, QUIT_TO_SELECTION, QUIT_TO_DESKTOP
};

bool openExisting();
string setUpCompany();
void addNewYear(const string&, DateUnit);
ProgramManager openFile(const string&, DateUnit);
MenuOption mainMenu(ProgramManager&, const string&);
void saveAndExit(ProgramManager&, const string&);

int main() {
    std::filesystem::create_directory(DATAPATH);
    string companyName = "";
    DateUnit year = 0;
    cout << "Welcome to the " << PROGRAM_NAME << " version " << CURRENT_BUILD << endl;
    if(openExisting()) {
        cout << "Enter the company name: ";
        companyName = "";
        year = 0;
        getline(cin, companyName);
        cout << "Enter the year: ";
        cin >> year;
        cin.get();
    } else {
        //New company registration
        companyName = setUpCompany();
        if(companyName != "") {
            year = 0;
            cout << "Enter the accounting year to begin recording: ";
            cin >> year;
            addNewYear(companyName, year);
        }
    }
    ProgramManager program = openFile(companyName, year);
    switch(mainMenu(program, companyName)) {
        
    }
    saveAndExit(program, companyName);
    return 0;
}

MenuOption mainMenu(ProgramManager& program, const string& companyName) {
    cout << endl << "Menu for " << companyName << ", " << program.getAccountLibrary().getYear() << endl;
    cout << "1. Record Transaction" << endl;
    cout << "2. View Account Summary" << endl;
    cout << "3. Prepare Financial Documents" << endl;
    cout << "4. Manage Accounts & Aliases" << endl;
    cout << "5. Return to Company Selection" << endl;
    cout << "6. Exit Program" << endl << endl;
    unsigned input = 0;
    cout << "Which option would you like to choose? (1-6): ";
    cin >> input;
    while(input < 1 or input > 6) {
        cout << "Invalid input, please try again: ";
        cin >> input;
    }
    switch(input) {
        case 1:
            return RECORD_TRANSACTION;
        case 2:
            return VIEW_SUMMARY;
        case 3:
            return PREPARE_DOCUMENTS;
        case 4:
            return MANAGE_ACCOUNTS;
        case 5:
            return QUIT_TO_SELECTION;
        case 6:
            return QUIT_TO_DESKTOP;
        default:
            throw std::runtime_error("Main menu option could not be parsed");
    }
}

void saveAndExit(ProgramManager&, const string&) {

}

bool openExisting() {
    cout << "Would you like to access records for an existing company or register a new company? (respond with existing/register): ";
    string response = "";
    getline(cin, response);
    while(response != "existing" and response != "register") {
        cout << "Invalid answer, please try again (\"existing\" or \"register\"): ";
        getline(cin, response);
    }
    return response == "existing";
}

string setUpCompany() {
    cout << "Enter the name of the company to create: ";
    string name = "", confirm = "";
    getline(cin, name);
    cout << "Please confirm that " << name << " is the correct name of the company (y/n): ";
    getline(cin, confirm);
    while(confirm != "y" and confirm != "n") {
        cout << "Invalid answer, please try again (\"y\" or \"n\"): ";
        getline(cin, confirm);
    }
    if(confirm == "n") return "";
    return name;
}

void addNewYear(const string& companyName, DateUnit year) {
    std::filesystem::create_directory(DATAPATH + companyName);

    ofstream startingFile(DATAPATH + companyName + "/" + std::to_string(year) + ".ar");
    if(not startingFile.is_open()) {
        throw "Could not open " + DATAPATH + companyName + "/" + std::to_string(year) + ".ar";
    }

    cout << "Enter all year-start account data in the form \"<ACCOUNT NAME>, <ACCOUNT TYPE>, <STARTING BALANCE>\" (q to stop)" << endl;
    string input = "";
    cin.get();
    getline(cin, input);
    while(input != "q") {
        startingFile << input << endl;
        getline(cin, input);
    }
    startingFile << ALIAS_DIVIDER << endl << JOURNAL_DIVIDER;
    startingFile.close();
}

void initializeAccounts(ProgramManager&, istream&);
void initializeAliases(ProgramManager&, istream&);
void initializeJournal(ProgramManager&, istream&);

ProgramManager openFile(const string& companyName, DateUnit year) {
    ifstream file(DATAPATH + companyName + "/" + std::to_string(year) + ".ar");

    if(not file.is_open()) {
        throw std::runtime_error("Could not open " + DATAPATH + companyName + "/" + std::to_string(year) + ".ar");
    }

    ProgramManager program(year);

    initializeAccounts(program, file);
    initializeAliases(program, file);
    initializeJournal(program, file);
    file.close();
    return program;
}

AccountType stringToAccountType(const string&);

void initializeAccounts(ProgramManager& program, istream& file) {
    string line = "";
    getline(file, line);
    while(line != ALIAS_DIVIDER) {
        string accName = "", accType = "";
        double startingBal = 0;
        basic_istringstream parser(line);
        getline(parser, accName, ',');
        getline(parser, accType, ',');
        accType.erase(0, 1);
        parser >> startingBal;
        AccountType type = stringToAccountType(accType);
        program.getAccountLibrary().addAccount(accName, type, startingBal);
        getline(file, line);
    }
}

AccountType stringToAccountType(const string& input) {
    string inputUpper = input;
    std::transform(inputUpper.begin(), inputUpper.end(), inputUpper.begin(), ::toupper);
    if (inputUpper == "ASSET") {
        return Asset;
    } else if (inputUpper == "LIABILITY") {
        return Liability;
    } else if (inputUpper == "STOCKHOLDERSEQUITY") {
        return StockholdersEquity;
    } else if (inputUpper == "REVENUE") {
        return Revenue;
    } else if (inputUpper == "EXPENSE") {
        return Expense;
    } else if (inputUpper == "GAIN") {
        return GAIN;
    } else if (inputUpper == "LOSS") {
        return LOSS;
    } else if (inputUpper == "DIVIDENDS") {
        return Dividends;
    } else if (inputUpper == "CONTRAASSET") {
        return ContraAsset;
    } else if (inputUpper == "CONTRALIABILITY") {
        return ContraLiability;
    } else if (inputUpper == "CONTRAEQUITY") {
        return ContraEquity;
    } else if (inputUpper == "CONTRAREVENUE") {
        return ContraRevenue;
    } else if (inputUpper == "CONTRAEXPENSE") {
        return ContraExpense;
    } else {
        throw std::runtime_error("Could not parse account type of " + input);
    }
}

void initializeAliases(ProgramManager& progran, istream& file) {

}
void initializeJournal(ProgramManager& program, istream& file) {
    
}