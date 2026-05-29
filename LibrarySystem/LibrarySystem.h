#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>
#include <fstream>
#include <sstream>

#include "Utils.h"
#include "Models.h"

using namespace std;

// Κεντρικό Σύστημα
class LibrarySystem {
private:
    vector<Book> libraryCatalogue;
    map<string, BookCopy> bookCopies;
    map<string, vector<historyEntry>> memberHistory;
    vector<Member> membersList;
    string activeUser;

    string toLowerCase(const string& str) const {
        string s = str;
        transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
        return s;
    }

public:
    string getActiveUser() const { return activeUser; }

    void saveData() const {

        ofstream bookFile("Data/books.txt"); // anoigma arxeiou kai apothikeusi dedomenwn vivliwn
        for (const auto& b : libraryCatalogue) {
            bookFile << b.title << "|" << b.author << "|" << b.category << "|"
                << b.getTotalCopies() << "|" << b.getAvailableCopies() << "|" << b.getBorrowedCopies() << "\n";
        }
        bookFile.close();

        ofstream copyFile("Data/copies.txt"); // anoigma arxeiou kai apothikeusi dedomenwn antigrafwn vivliwn
        for (const auto& pair : bookCopies) {
            const auto& c = pair.second;
            copyFile << c.copyID << "|" << c.title << "|" << c.author << "|" << c.category << "|" << c.getAvailability() << "\n";
        }
        copyFile.close();

        ofstream memberFile("Data/members.txt"); // anoigma arxeiou kai apothikeusi dedomenwn melwn
        for (const auto& m : membersList) {
            memberFile << m.name << "|" << m.code << "\n";
            ofstream historyFile("Data/Histories/history_" + m.name + ".txt"); // apothikeusi istorikou kathe melous se diaforetiko arxeio
            for (const auto& entry : memberHistory.at(m.name)) {
                historyFile << "H|" << entry.title << "|" << entry.author << "|" << entry.category << "|"
                    << entry.copyID << "|" << entry.borrowDate << "|" << entry.dueDate << "|" << entry.returnDate << "|" << entry.flag << "\n";
            }
            historyFile.close();
        }
        memberFile.close();

        cout << "[System] Ta dedomena apothikeutikan epitixos.\n";
    }

    void loadData() {
        string line;

        ifstream bookFile("Data/books.txt"); // anoigma arxeiou kai fortwsh dedomenwn vivliwn
        if (bookFile.is_open()) {
            while (getline(bookFile, line)) {
                stringstream ss(line);
                string t, a, c, tC, aC, bC;
                getline(ss, t, '|'); getline(ss, a, '|'); getline(ss, c, '|');
                getline(ss, tC, '|'); getline(ss, aC, '|'); getline(ss, bC, '|');

                if (!t.empty()) {
                    libraryCatalogue.push_back(Book(t, a, c, stoi(tC), stoi(aC), stoi(bC)));
                }
            }
            bookFile.close();
        }

        ifstream copyFile("Data/copies.txt"); // anoigma arxeiou kai fortwsh dedomenwn antigrafwn vivliwn
        if (copyFile.is_open()) {
            while (getline(copyFile, line)) {
                stringstream ss(line);
                string id, t, a, c, availStr;
                getline(ss, id, '|'); getline(ss, t, '|'); getline(ss, a, '|');
                getline(ss, c, '|'); getline(ss, availStr, '|');

                if (!id.empty()) {
                    bool isAvail = (availStr == "1");
                    bookCopies.insert({ id, BookCopy(t, a, c, id, isAvail) });
                    Hexgeneator::syncCounter(id);
                }
            }
            copyFile.close();
        }

        ifstream memberFile("Data/members.txt"); // anoigma arxeiou kai fortwsh dedomenwn melwn
        if (memberFile.is_open()) {
            while (getline(memberFile, line)) {
                stringstream ss(line);
                string n, code;
                getline(ss, n, '|'); getline(ss, code, '|');

                if (!n.empty()) {
                    membersList.push_back(Member(n, code));
                }
                ifstream historyFile("Data/Histories/history_" + n + ".txt"); // fortwsh istorikou kathe melous apo diaforetiko arxeio
                if (historyFile.is_open()) {
                    string hLine;
                    while (getline(historyFile, hLine)) { // ponos kai ydrwtas na bgei auto, na eiste eugnomwn.
                        stringstream hss(hLine);
                        string prefix, t, a, c, id, bDate, dDate, rDate, flagStr;
                        getline(hss, prefix, '|'); getline(hss, t, '|'); getline(hss, a, '|');
                        getline(hss, c, '|'); getline(hss, id, '|'); getline(hss, bDate, '|');
                        getline(hss, dDate, '|'); getline(hss, rDate, '|'); getline(hss, flagStr, '|');
                        if (prefix == "H" && !id.empty()) {
                            bool flag = (flagStr == "1");
                            memberHistory[n].push_back(historyEntry(t, a, c, id, bDate, dDate));
                            memberHistory[n].back().setReturnDate(rDate);
                            memberHistory[n].back().flag = flag;
                        }
                    }
                    historyFile.close();
                }
            }
            memberFile.close();
        }

        cout << "[System] Ta dedomena fortothikan epitixos.\n";
    }

    void addBook(string t, string a, string c, int copies) {
    book_add:
        bool found = false;
        int cell = -1;
        string queryLowerT = toLowerCase(t);
        string queryLowerA = toLowerCase(a);

        for (size_t i = 0; i < libraryCatalogue.size(); ++i) {
            string targetLowerT = toLowerCase(libraryCatalogue[i].title);
            string targetLowerA = toLowerCase(libraryCatalogue[i].author);

            if (!targetLowerT.empty() && !targetLowerA.empty() && targetLowerT == queryLowerT && targetLowerA == queryLowerA) {
                cell = i;
                found = true;
                break;
            }
        }

        string copyID;

        if (found) {
            string temp;
            cout << "To vivlio yparxei idi.\n" << "-> new_copy\n" << "-> new_book\n" << "-> cancel\n>";
            temp = getStringInput();

            if (temp == "cancel") {
                cout << "Prosthiki vivliou akyrothike.\n";
            }
            else if (temp == "new_copy") {
                int additionalCopies;
                cout << "Dwse arithmo antigrafon pou thes na prostheseis: ";
                cin >> additionalCopies;
                libraryCatalogue[cell].updateCopies(additionalCopies);

                for (int i = 0; i < additionalCopies; i++) {
                    copyID = Hexgeneator::generateHexCode();
                    bookCopies.insert({ copyID, BookCopy(t, a, c, copyID) });
                }
                cout << "[+] Antigrafo prostethike epitixos! Diathesima antigrafa: " << libraryCatalogue[cell].getAvailableCopies() << "\n";
            }
            else if (temp == "new_book") {
                Book newBook(t, a, c, copies);
                libraryCatalogue.push_back(newBook);

                for (int i = 0; i < copies; i++) {
                    copyID = Hexgeneator::generateHexCode();
                    bookCopies.insert({ copyID, BookCopy(t, a, c, copyID) });
                }
                cout << "[+] To neo vivlio prostethike epitixos!\n";
            }
            else {
                cout << "Lathos epilogi, prospathise xana.\n";
                goto book_add;
            }
        }
        else {
            Book newBook(t, a, c, copies);
            libraryCatalogue.push_back(newBook);

            for (int i = 0; i < copies; i++) {
                copyID = Hexgeneator::generateHexCode();
                bookCopies.insert({ copyID, BookCopy(t, a, c, copyID) });
            }
            cout << "[+] To vivlio prostethike epitixos!\n";
        }
    }

    void displayBooks() const {
        cout << "\n--- Katalogos Vivlion ---\n";
        if (libraryCatalogue.empty()) {
            cout << "H vivliothiki einai adeia.\n";
        }
        else {
            for (const auto& book : libraryCatalogue) {
                cout << "Titlos: " << book.title
                    << " | Syggrapheas: " << book.author
                    << " | Kathgoria: " << book.category
                    << " | Diathesima Antigrafa: " << book.getAvailableCopies() << "\n";
            }
        }
        cout << "-------------------------\n";
    }

    vector<Book> searchBook(string search, string type) const {
        vector<Book> results;
        string queryLower = toLowerCase(search);

        for (const auto& book : libraryCatalogue) {
            string targetLower = (type == "title") ? toLowerCase(book.title) : toLowerCase(book.author); // kserw auto dhmiourgei 8ema an o xrhsths dwsei lathos type, alla den to ekana cuz we were running out of time :(

            if (!targetLower.empty() && targetLower.find(queryLower) != string::npos) {
                results.push_back(book);
            }
        }
        return results;
    }

    void addMember(string name, string code) {
        string queryLower = toLowerCase(name);

        for (const auto& member : membersList) {
            string targetLower = toLowerCase(member.name);
            if (targetLower == queryLower) {
                cout << "-> To melos yparxei idi: " << member.name << "\n";
                return;
            }
        }

        Member newMember(name, code);
        membersList.push_back(newMember);
        cout << "[+] To melos prostethike epitixos!\n";
    }

    vector<Member> searchMember(string name) const {
        vector<Member> results;
        string queryLower = toLowerCase(name);

        for (const auto& member : membersList) {
            string targetLower = toLowerCase(member.name);
            if (!targetLower.empty() && targetLower.find(queryLower) != string::npos) {
                results.push_back(member);
            }
        }
        return results;
    }

    void borrowBook(string title) {
        string queryLower = toLowerCase(title);

        bool flag;
        int borrowedCount = 0;
        for (auto& entry : memberHistory[activeUser]) {
            flag = false;
            if (entry.getState()) { borrowedCount++; }
        }
        if (borrowedCount >= 5) {
            cout << "[-] Den mporeis na daneisteis allo vivlio. Exeis ftasei to orio twn 5 daneismwn.\n";
            return;
        }

        for (auto& book : libraryCatalogue) {
            if (toLowerCase(book.title) == queryLower) {

                if (book.getAvailableCopies() > 0) {
                    cout << "=== " << book.title << " by " << book.author << " ===\n";
                    cout << "Diathesima antigrafa (Available Copies):\n";

                    for (auto& pair : bookCopies) {
                        if (toLowerCase(pair.second.title) == queryLower && pair.second.getAvailability() == true) {
                            cout << "- Copy ID: " << pair.first << "\n";
                        }
                    }

                    string selectedID;
                    cout << "\nPliktrologise to Copy ID pou theleis na daneisteis: ";
                    cin >> selectedID;

                    auto it = bookCopies.find(selectedID);

                    if (it != bookCopies.end() && toLowerCase(it->second.title) == queryLower) {
                        if (it->second.getAvailability()) {
                            bool tempFlag = false;
                            it->second.borrowCopy(tempFlag);
                            book.updateBorrowed(1, true);
                            newHistoryEntry(activeUser, selectedID);

                            cout << "[+] Epitixis daneismos tou antigrafou: " << selectedID << "\n";
                        }
                        else {
                            cout << "[-] Afto to antigrafo den einai diathesimo auth th stigmh.\n";
                        }
                    }
                    else {
                        cout << "[-] Lathos Copy ID. To antigrafo den vrethike i den anikei se afto to vivlio.\n";
                    }

                    cout << "-> Diathesima antigrafa pleon: " << book.getAvailableCopies() << "\n";
                }
                else {
                    cout << "[-] To vivlio yparxei alla den exoume diathesima antigrafa afti ti stigmi!\n";
                }
                return;
            }
        }
        cout << "-> To vivlio pou zhtises den vrethike ston katalogo.\n";
    }

    void newHistoryEntry(string memberName, string copyID) {
        if (memberName.empty()) return;
        if (bookCopies.find(copyID) == bookCopies.end()) {
            cout << "[-] To copyID den vrethike.\n";
            return;
        }

        string date = DateCalc::getCurrentDate();
        string duedate = DateCalc::getDueDate(); // <---------------------------------------------------------------------------------------------------------------------------- UwU
        const auto& book = bookCopies.at(copyID);
        memberHistory[memberName].push_back(historyEntry(
            book.title,
            book.author,
            book.category,
            copyID,
            date,
            duedate
        ));
    }

    void changeHEntryState(string memberName, string copyID, bool newFlag) {
        if (memberName.empty()) return;

        if (memberHistory.find(memberName) != memberHistory.end()) {
            for (auto& entry : memberHistory[memberName]) {
                if (entry.copyID == copyID && entry.flag == true) {
                    entry.flag = newFlag;
                    string returnDate = DateCalc::getCurrentDate();
                    entry.setReturnDate(returnDate);
                    break;
                }
            }
        }
    }

    void displayHistory(string memberName) const {
        string queryLower = toLowerCase(memberName);
        for (const auto& member : membersList) {
            string targetLower = toLowerCase(member.name);
            if (targetLower == queryLower) {
                cout << "\n=== Istoriko gia " << member.name << " ===\n";
                if (memberHistory.find(member.name) != memberHistory.end()) {
                    for (const auto& entry : memberHistory.at(member.name)) {
                        cout << "Titlos: " << entry.title
                            << " | Syggrapheas: " << entry.author
                            << " | Copy ID: " << entry.copyID
                            << " | Status: ";
                        if (!entry.flag) {
                            cout << "Epestrafein"
                                << " | Date Epistrofhs: " << entry.getReturnDate();
                        }
                        else {
                            cout << "Daneismeno "
                                << " | Days Left:       " << DateCalc::getDaysLeft(entry.getDueDate());
                        }
                        cout << " | Date Daneismou: " << entry.getBorrowDate()
                            << " | Due Date: " << entry.getDueDate() << "\n";
                    }
                }
                else {
                    cout << "-> Den yparxei istoriko gia afto to melos.\n";
                }
                return;
            }
        }
        cout << "-> To melos den vrethike ston katalogo.\n";
    }

    void returnBook(string title) {
        string queryLower = toLowerCase(title);

        for (auto& book : libraryCatalogue) {
            if (toLowerCase(book.title) == queryLower) {

                vector<string> userBorrowedCopies;

                if (memberHistory.find(activeUser) != memberHistory.end()) {
                    for (const auto& entry : memberHistory[activeUser]) {
                        if (toLowerCase(entry.title) == queryLower && entry.flag == true) {
                            userBorrowedCopies.push_back(entry.copyID);
                        }
                    }
                }

                if (!userBorrowedCopies.empty()) {
                    cout << "=== " << book.title << " by " << book.author << " ===\n";
                    cout << "Antigrafa pou exeis daneistei (Your Borrowed Copies):\n";

                    for (const string& id : userBorrowedCopies) {
                        cout << "- Copy ID: " << id << "\n";
                    }

                    string selectedID;
                    cout << "\nPliktrologise to Copy ID pou theleis na epistrepseis: ";
                    cin >> selectedID;

                    bool userOwnsCopy = false;
                    for (const string& id : userBorrowedCopies) {
                        if (id == selectedID) {
                            userOwnsCopy = true;
                            break;
                        }
                    }

                    if (userOwnsCopy) {
                        auto it = bookCopies.find(selectedID);

                        if (it != bookCopies.end()) {
                            bool tempFlag = false;
                            it->second.returnCopy(tempFlag);
                            book.updateBorrowed(-1, true);

                            changeHEntryState(activeUser, selectedID, false);

                            cout << "[+] Epitixis epistrofi tou antigrafou: " << selectedID << "\n";
                        }
                    }
                    else {
                        cout << "[-] Lathos Copy ID. Den exeis daneistei auto to antigrafo.\n";
                    }

                    cout << "-> Diathesima antigrafa pleon: " << book.getAvailableCopies() << "\n";
                }
                else {
                    cout << "[-] Den exeis daneistei kapoio antigrafo autou tou vivliou auti ti stigmi!\n";
                }
                return;
            }
        }
        cout << "-> To vivlio auto den anikei sth vivliothiki mas.\n";
    }

    void login(string name, bool& flag) {
        string queryLower = toLowerCase(name);
        vector<int> cell;

        for (size_t i = 0; i < membersList.size(); ++i) {
            string targetLower = toLowerCase(membersList[i].name);

            if (targetLower == queryLower) {
                cout << "-> Vrethike akrivis taftisi: " << membersList[i].name << "\n";
                cout << "Eisagete kwdiko: ";

                string code;
                cin >> code;

                if (code == membersList[i].code) {
                    cout << "Eisodos epitixis!\nKaloshrthes vre!\n";
                    activeUser = membersList[i].name;
                    flag = true;
                }
                else {
                    cout << "Lathos kwdikos. Prospathise xana.\n";
                }
                return;
            }
            else if (!targetLower.empty() && targetLower.find(queryLower) != string::npos) {
                cell.push_back(i);
            }
        }

        if (cell.size() == 1) {
            int index = cell[0];
            cout << "-> Vrethike: " << membersList[index].name << "\n";
            cout << "Eisagete kwdiko: ";

            string code;
            cin >> code;

            if (code == membersList[index].code) {
                cout << "Eisodos epitixis!\nKaloshrthes vre!\n";
                activeUser = membersList[index].name; // REMEMBER WHO LOGGED IN
                flag = true;
            }
            else {
                cout << "Lathos kwdikos. Prospathise xana.\n";
            }
        }
        else if (cell.size() > 1) {
            cout << "Vrethikan polla melh me afta ta grammata. Dwste perissotera stoixeia gia na taftopoihthei to melos.\n";
            for (size_t i = 0; i < cell.size(); i++) {
                cout << " - " << membersList[cell[i]].name << "\n";
            }
        }
        else {
            cout << "-> To melos den vrethike katholou.\n";
        }
    }


    // gia Testing
    void addBulkBooks() {
        // MONO an h vivlio8hkh einai adeia gia asfaleia.
        if (!libraryCatalogue.empty()) {
            cout << "[-] To sustima exei idi dedomena. H maziki prosthiki akyrwthike gia na apofeuxthoun diplokatoxwriseis.\n";
            return;
        }

        // Temporary structure gia ta data ( organwsh fr )
        struct TempBook { string t, a, c; int copies; };

        vector<TempBook> booksToAdd = {
            // !!AI GENERATED DATA BELLOW!! (MONO gia testing, euxomaste na mhn yparxei provlhma </3)
            // Electronics & Hardware
            {"The Art of Electronics", "Paul Horowitz", "Hardware", 5},
            {"Practical Electronics for Inventors", "Paul Scherz", "Hardware", 4},
            {"Microelectronic Circuits", "Adel S. Sedra", "Hardware", 3},
            {"Fundamentals of Electric Circuits", "Charles K. Alexander", "Hardware", 4},
            {"Arduino Cookbook", "Michael Margolis", "Hardware", 5},
            {"Digital Design", "M. Morris Mano", "Hardware", 3},

            // Programming & Software
            {"The C++ Programming Language", "Bjarne Stroustrup", "Programming", 4},
            {"Effective Modern C++", "Scott Meyers", "Programming", 3},
            {"Clean Code", "Robert C. Martin", "Software", 5},
            {"Design Patterns", "Erich Gamma", "Software", 3},

            // Game Development
            {"Godot Engine Game Development", "Denis Kozlov", "Game Design", 4},
            {"Game Programming Patterns", "Robert Nystrom", "Game Design", 3},
            {"The Art of Game Design", "Jesse Schell", "Game Design", 2},

            // Design & Fashion
            {"The Fashion Design Reference Book", "Jay Calderin", "Design", 2},
            {"Patternmaking for Fashion Design", "Helen Joseph Armstrong", "Design", 2},
            {"Fashion History", "Linda Welters", "History", 1},

            // Space & General Science
            {"Astrophysics for People in a Hurry", "Neil deGrasse Tyson", "Science", 4},
            {"Cosmos", "Carl Sagan", "Science", 3},
            {"Packing for Mars", "Mary Roach", "Science", 2},

            // Fiction
            {"Dune", "Frank Herbert", "Fiction", 5},
            {"Neuromancer", "William Gibson", "Fiction", 3},
            {"The Martian", "Andy Weir", "Fiction", 4}
        };

		// pros8hkh autwn twn vivliwn ston katalogo kai dimiourgia antigrafwn me monadiko copyID gia kathe antigrafo
        for (const auto& b : booksToAdd) {
            Book newBook(b.t, b.a, b.c, b.copies);
            libraryCatalogue.push_back(newBook);

            for (int i = 0; i < b.copies; i++) {
                string copyID = Hexgeneator::generateHexCode();
                bookCopies.insert({ copyID, BookCopy(b.t, b.a, b.c, copyID) });
            }
        }
        cout << "[+] Maziki prosthiki " << booksToAdd.size() << " vivliwn oloklirothike epitixos!\n";
    }
};