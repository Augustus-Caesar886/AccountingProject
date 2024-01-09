#include "../header/Date.h"

#include <stdexcept>

using std::to_string;
using std::invalid_argument;
using std::stoi;

Date::Date(const string& mmddyyyy) {
    if(mmddyyyy.length() != 10) throw invalid_argument("not in mm/dd/yyyy format");
    month = stoi(mmddyyyy.substr(0, 2));
    if(month < 0 or month > 12) throw invalid_argument("invalid month");
    if(month != (mmddyyyy[0]-'0') * 10 + (mmddyyyy[1]-'0')) throw invalid_argument("bad month input");
    day = stoi(mmddyyyy.substr(3, 2));
    if(day < 0 or day > 31) throw invalid_argument("invalid day");
    if(day != (mmddyyyy[3]-'0') * 10 + (mmddyyyy[4]-'0')) throw invalid_argument("bad day input");
    year = stoi(mmddyyyy.substr(6, 4));
    if(year < 0 or year > 9999) throw invalid_argument("invalid year");
    if(year != (mmddyyyy[6]-'0') * 1000 + (mmddyyyy[7]-'0') * 100 + (mmddyyyy[8]-'0') * 10 + (mmddyyyy[9]-'0')) throw invalid_argument("bad year input");
}

string Date::stringForm() const {
    string output = "";
    
    if(month < 10) 
        output += "0";
    output += to_string(month) + "/";

    if(day < 10) 
        output += "0";
    output += to_string(day) + "/";

    for(int i = 1000; i >= 10; i/=10) {
        if(year < i) output += "0";
    }
    
    output += to_string(year);

    return output;
}