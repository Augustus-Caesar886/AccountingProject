#ifndef DATE_H
#define DATE_H

#include <string>
using std::string;

using DateUnit = unsigned short;

struct Date {
    DateUnit year, month, day;
    Date(DateUnit year, DateUnit month, DateUnit day) : year(year), month(month), day(day) {}
    Date(const string&);
    string stringForm() const;
};

#endif