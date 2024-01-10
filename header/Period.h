#ifndef PERIOD_H
#define PERIOD_H

#include "Date.h"

class Period {
    private:
        Date startDate;
        Date endDate;
    public:
        Period(const Date &startDate, const Date &endDate) : startDate(startDate), endDate(endDate) {}
        Date getStartDate() const { return startDate; }
        Date getEndDate() const { return endDate; }
};

#endif