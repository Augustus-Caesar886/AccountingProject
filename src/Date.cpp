#include "../header/Date.h"

using std::to_string;

Date::Date(const string& mmddyyyy) {
    month = (mmddyyyy[0] - '0') * 10 + (mmddyyyy[1] - '0');
    day = (mmddyyyy[3] - '0') * 10 + (mmddyyyy[4] - '0');
    year = (mmddyyyy[6] - '0') * 1000 + (mmddyyyy[7] - '0') * 100 + (mmddyyyy[8] - '0') * 10 + (mmddyyyy[9] - '0');
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