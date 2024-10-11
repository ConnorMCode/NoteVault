#include "MetaData.h"
#include <ctime>
#include <sstream>

namespace Core {

    std::string MetaData::getCurrentDate() {
        // Get current time
        time_t now = time(0);
        tm* ltm = localtime(&now);

        // Extract year, month, and day
        int year = 1900 + ltm->tm_year;
        int month = 1 + ltm->tm_mon;
        int day = ltm->tm_mday;

        // Format the date as YYYY-MM-DD
        std::ostringstream oss;
        oss << year << "-"
            << (month < 10 ? "0" : "") << month << "-"
            << (day < 10 ? "0" : "") << day;

        return oss.str();
    }

}
