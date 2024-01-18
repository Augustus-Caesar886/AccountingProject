#include <iostream>
using std::cout;
using std::endl;
using std::cin;
using std::string;

#include <fstream>
using std::ifstream;

#include "../header/Date.h"
#include "../header/PrototypeFileReader.h"

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
        cout << "q: quit" << endl;

        cin >> input;
        cin.get();
        if(input == "1") {
            cout << "Enter account name: " << endl;
            getline(cin, input);
            Account &found = fileReader.getAccount(input);
            cout << "---" << input << "---" << endl;
            cout << "BB: " << found.getBeginningBalance() << endl;
            for(auto it : found.getEntries()) {
                cout << it.getDate().stringForm() << (it.get().second == ValueType::debit ? " dr " : " cr ") << it.get().first << endl;
            }
            cout << "EB: " << found.getBalance() << endl << endl;
        } else if(input == "2") {

        }
    }

    return 0;
}