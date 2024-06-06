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

bool openExisting();
string setUpCompany();
void addNewYear(const string&, DateUnit);
ProgramManager openFile(const string&, DateUnit);

int main() {
    std::filesystem::create_directory(DATAPATH);
    string companyName = "";
    DateUnit year = 0;
    cout << "Welcome to the " << PROGRAM_NAME << " version " << CURRENT_BUILD << endl;
    if(openExisting()) {
        //Open existing company
        cout << "Enter the company name: ";
        companyName = "";
        year = 0;
        getline(cin, companyName);
        cout << "Enter the year: ";
        cin >> year;
        cin.get();
    } else {
        companyName = setUpCompany();
        if(companyName != "") {
            year = 0;
            cout << "Enter the accounting year to begin recording: ";
            cin >> year;
            addNewYear(companyName, year);
        }
    }
    ProgramManager program = openFile(companyName, year);
    return 0;
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
    getline(cin, input);
    while(input != "q") {
        startingFile << input << endl;
        getline(cin, input);
    }
    startingFile.close();
}

void initializeAccounts(ProgramManager&, istream&);
void initializeAliases(ProgramManager&, istream&);
void initializeJournal(ProgramManager&, istream&);

ProgramManager openFile(const string& companyName, DateUnit year) {
    ifstream file(DATAPATH + companyName + "/" + std::to_string(year) + ".ar");

    if(not file.is_open()) {
        throw "Could not open " + DATAPATH + companyName + "/" + std::to_string(year) + ".ar";
    }

    ProgramManager program(year);

    initializeAccounts(program, file);
    initializeAliases(program, file);
    initializeJournal(program, file);

    file.close();
    return program;
}

void initializeAccounts(ProgramManager& program, istream& file) {

}
void initializeAliases(ProgramManager& progran, istream& file) {

}
void initializeJournal(ProgramManager& program, istream& file) {
    
}