#include <iostream>
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::getline;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include "../header/Date.h"
#include "../header/PrototypeFileReader.h"
#include "../header/PrototypeFileWriter.h"

int main() {
    cout << "Enter company name: " << endl;
    string dirname = "";
    cin >> dirname;
    ifstream file;
    DateUnit year;

    while(not file.is_open()) {
        cout << "Enter accounting year: " << endl;
        cin >> year;
        cin.get();
        file.open("bin/companydata/" + dirname + "/" + std::to_string(year) + ".ar");
    }
    PrototypeFileReader fileReader(file, year);
    file.close();
    string input = "";
    while(input != "q") {
        cout << "Menu" << endl;
        cout << "1. See account data" << endl;
        cout << "2. Add new Ledger entry" << endl;
        cout << "3. Quick trial balance" << endl;
        cout << "q: Save & Quit" << endl;

        cin >> input;
        cin.get();
        if(input == "1") {
            cout << "Enter account name: " << endl;
            getline(cin, input);
            const Account &found = fileReader.getAccount(input);
            cout << "---" << input << "---" << endl;
            cout << "BB\t\t" << found.getBeginningBalance() << endl;
            for(auto it : found.getEntries()) {
                cout << it.getDate().stringForm() << (it.get().second == ValueType::debit ? " dr\t" : " cr\t") << it.get().first << "\t| " << it.getDescription() << endl;
            }
            cout << "EB\t\t" << found.getBalance() << endl << endl;
        } else if(input == "2") {
            cout << "Enter date (mm/dd/yyyy): " << endl; 
            cin >> input;
            cin.get();
            Date day(input);
            string description;
            cout << "Enter description: " << endl; 
            getline(cin, description);
            cout << "Input the entries in the following form: dr (or cr) \"Account Name Here\" amount" << endl;
            cout << "d to exit to main menu" << endl;
            input = "";
            vector<LedgerModification> modifications;
            vector<string> accNames;
            while(input != "d") {
                cin >> input;
                if(input == "d") {
                    double debits = 0, credits = 0;
                    for(auto it : modifications) {
                        if(it.get().second == ValueType::debit) {
                            debits += it.get().first;
                        } else {
                            credits += it.get().first;
                        }
                    }
                    if(debits != credits) {
                        cout << "Entries not balanced, discarding changes" << endl;
                        accNames.clear();
                        modifications.clear();
                    }
                    continue;
                }
                ValueType vt;
                if(input == "dr") {
                    vt = ValueType::debit;
                } else if (input == "cr") {
                    vt = ValueType::credit;
                }
                getline(cin, input, '"');
                getline(cin, input, '"');
                string accountName = input;
                double amount = 0;
                cin >> amount;
                cin.get();
                accNames.push_back(accountName);
                modifications.push_back(LedgerModification(amount, vt, day, description));
            }
            for(unsigned i = 0; i < accNames.size(); ++i) {
                fileReader.getAccount(accNames[i]).addEntry(modifications[i]);
                fileReader.push_back(modifications[i], accNames[i]);
            }
        } else if(input == "3") {
            double debits = 0, credits = 0;
            for(auto it : fileReader.getAccounts()) {
                if(it.getBalanceType() == ValueType::debit) {
                    debits += it.getBalance();
                } else if(it.getBalanceType() == ValueType::credit){
                    credits += it.getBalance();
                }
            }
            cout << "Debits: " << debits << endl;
            cout << "Credits: " << credits << endl;
        }
    }

    PrototypeFileWriter writer(fileReader.getEntries(), fileReader.getNames(), fileReader.getAccounts());
    ofstream outfile;
    outfile.open("bin/companydata/" + dirname + "/" + std::to_string(year) + ".ar");
    writer.writeToFile(outfile);
    outfile.close();
    return 0;
}