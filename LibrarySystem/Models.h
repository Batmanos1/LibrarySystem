#pragma once

#include <iostream>
#include <string>

using namespace std;

// Κλάση μελών
class Member {
public:
    string name;
    string code;
    Member(string n, string c) : name(n), code(c) {}
};

// Κλάση βιβλίων (γενικά στοιχεία, όχι αντίγραφα)
class Book {
private:
    int totalCopies;
    int availableCopies;
    int borrowedCopies;

public:
    string title;
    string author;
    string category;

    Book() : title(""), author(""), category(""), totalCopies(0), availableCopies(0), borrowedCopies(0) {
    }

    Book(string t, string a, string c, int copies)
        : title(t), author(a), category(c), totalCopies(copies), availableCopies(copies), borrowedCopies(0) {
    }

    Book(string t, string a, string c, int total, int avail, int borrowed)
        : title(t), author(a), category(c), totalCopies(total), availableCopies(avail), borrowedCopies(borrowed) {
    }

    int getAvailableCopies() const { return availableCopies; }
    int getBorrowedCopies() const { return borrowedCopies; }
    int getTotalCopies() const { return totalCopies; }

    void updateCopies(int addition) {
        totalCopies += addition;
        availableCopies += addition;
    }

    void updateBorrowed(int change, bool flag) {
        if (flag && borrowedCopies + change <= totalCopies && borrowedCopies + change >= 0) {
            borrowedCopies += change;
            availableCopies = totalCopies - borrowedCopies;
        }
        else if (flag && borrowedCopies - change >= 0) {
            borrowedCopies -= change; availableCopies = totalCopies - borrowedCopies;
        }
        else {
            cout << "Lathos arithmos antigrafon gia daneismo/epistrofi. Prospathise xana.\n";
        }
    }
};

// Αντίγραφα βιβλίων (κάθε αντίγραφο ξεχωριστό)
class BookCopy : public Book {
public:
    string copyID;
    bool available;

    BookCopy() : Book(), copyID(""), available(false) {
    }

    BookCopy(string t, string a, string c, string id)
        : Book(t, a, c, 1), copyID(id), available(true) {
    }

    BookCopy(string t, string a, string c, string id, bool isAvailable)
        : Book(t, a, c, 1), copyID(id), available(isAvailable) {
    }

    bool getAvailability() const { return available; }
    string getCopyID() const { return copyID; }

    void borrowCopy(bool flag) {
        if (available) {
            available = false;
            updateBorrowed(1, true);
            flag = true;
        }
        else {
            flag = false;
        }
    }

    void returnCopy(bool flag) {
        if (!available) {
            available = true;
            updateBorrowed(-1, true);
            flag = true;
        }
        else {
            flag = false;
        }
    }
};

// Ιστορικό δανεισμών (κάθε μέλος)
class historyEntry {
private:
    int dueYear;
    int dueMonth;
    int dueDay;
public:
    string title;
    string author;
    string category;
    string copyID;
    string borrowDate;
    string returnDate;
    string dueDate;
    bool flag; // true an einai katanemimeno apo idio melos

    historyEntry(string t, string a, string c, string id, string bDate, string dDate)
        : title(t), author(a), category(c), copyID(id), borrowDate(bDate), dueDate(dDate), returnDate(""), flag(true) {
    }

    string getBorrowDate() const { return borrowDate; }
    string getDueDate() const { return dueDate; }
    string getReturnDate() const { return returnDate; }
    int getState() { return flag; }
    void setBorrowDate(string bDate) { borrowDate = bDate; }
    void setDueDate(string dDate) { dueDate = dDate; }
    void setReturnDate(string rDate) { returnDate = rDate; }
    void setDueDateComponents(int year, int month, int day) {
        dueYear = year;
        dueMonth = month;
        dueDay = day;
    }
};