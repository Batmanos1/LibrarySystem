#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

//Δημιουργια ημερομηνιας
class DateCalc {
public:
    static string getCurrentDate() {
        time_t currentTime = time(0);
        tm localTime;
        localtime_s(&localTime, &currentTime);
        ostringstream dateTemp;
        dateTemp << (localTime.tm_year + 1900) << "-" << (localTime.tm_mon + 1) << "-" << localTime.tm_mday;
        string date = dateTemp.str();
        return date;
    }

    static string getDueDate() {
        time_t currentTime = time(0);
        tm localTime;
        localtime_s(&localTime, &currentTime);
        localTime.tm_mday += 14; // DEKATESSEREIS_FLEBARHHHHHHHHHHHHHHH-The due date reference...
        mktime(&localTime); // Kanoniki anaparastasi hmeromhnias giati einai pio aesthetic
        ostringstream dateTemp;
        dateTemp << (localTime.tm_year + 1900) << "-" << (localTime.tm_mon + 1) << "-" << localTime.tm_mday;
        string dueDate = dateTemp.str();
        return dueDate;
    }

    static int getDaysLeft(string dueDate) {
        int year, month, day;
        char dash1, dash2;
        string date = getCurrentDate();
        stringstream ss1(date);
        ss1 >> year >> dash1 >> month >> dash2 >> day;
        tm currentDateTM = {};
        currentDateTM.tm_year = year - 1900;
        currentDateTM.tm_mon = month - 1;
        currentDateTM.tm_mday = day;

        stringstream ss2(dueDate);
        ss2 >> year >> dash1 >> month >> dash2 >> day;
        tm dueDateTM = {};
        dueDateTM.tm_year = year - 1900;
        dueDateTM.tm_mon = month - 1;
        dueDateTM.tm_mday = day;

        time_t time1 = mktime(&currentDateTM);
        time_t time2 = mktime(&dueDateTM);

        double diffInSeconds = difftime(time2, time1);

        int finalDaysLeft = diffInSeconds / (60 * 60 * 24); // thstheiastuokwlos °v°

        return finalDaysLeft;
    }
};

// Δημιουργία μοναδικών 16δικών κωδικών.
class Hexgeneator {
private:
    static unsigned int counter;
public:
    static string generateHexCode() {
        counter++;
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%05X", counter);
        return string(buffer);
    }

    static void syncCounter(string hexCode) {
        unsigned int loadedVal;
        stringstream ss;
        ss << std::hex << hexCode;
        ss >> loadedVal;
        if (loadedVal > counter) {
            counter = loadedVal;
        }
    }
};

// Διαδικασία για καθαρή ανάγνωση κειμένου ο.ο
string getStringInput() {
    string input;
    cin >> ws;
    getline(cin, input);
    return input;
}