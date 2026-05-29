#include <iostream>
#include <string>

#include "Utils.h"
#include "Models.h"
#include "LibrarySystem.h"

#define adminCode "admin123" // arxikopoiisi kwdikou gia admin (gia apokleismo xrhshs apo opoiodhpote)

using namespace std;

// Θέτουμε αρχική τιμή (καλύτερος συγχρονισμός αποθηκευμένων, επίσης είχε πρόβλημα χωρίς οπότε we had to :_) )
unsigned int Hexgeneator::counter = 1000;

int main() {
    LibrarySystem myLibrary;

    // myLibrary.addBulkBooks(); // gemisma vivliothikis gia testing

    myLibrary.loadData();

    cout << "\n=== LIBRARY MANAGEMENT SYSTEM ===" << endl;

    while (true) {
    menu_main:
        string choice;
        cout << "\nDwse idiotita:\n" << "-> member\n" << "-> admin\n" << "-> guest\n" << "-> exit (gia exodo)\n> ";
        choice = getStringInput();

        if (choice == "exit") {
            cout << "Exodos apo to sustima. Kalo sou vradi!\n";
            break;
        }
        else if (choice == "member") {
            bool loggedIn = false;
        menu_member:
            string temp;
            if (!loggedIn) {
                cout << "\nEpiloges member:\n" << "-> login\n" << "-> search\n" << "-> back\n> ";
                temp = getStringInput();
                if (temp == "back") {
                    goto menu_main;
                }
                else if (temp == "search") {
                    cout << "Dwse timi gia anazitisi: ";
                    string memberName = getStringInput();

                    vector<Member> foundMembers = myLibrary.searchMember(memberName);
                    cout << "\n--- Apotelesmata anazitisis melous gia '" << memberName << "' ---\n";
                    if (foundMembers.empty()) {
                        cout << "-> To melos den vrethike.\n";
                    }
                    else {
                        for (const auto& m : foundMembers) {
                            cout << "-> Vrethike: " << m.name << "\n";
                        }
                    }
                    cout << "---------------------------------------------\n";
                    goto menu_member;
                }
                else if (temp == "login") {
                    cout << "Dwse onomatepwnymo gia login: ";
                    string memberName = getStringInput();
                    myLibrary.login(memberName, loggedIn);
                    goto menu_member;
                }
                else {
                    cout << "Lathos epilogi, prospathise xana.\n";
                    goto menu_member;
                }
            }
            else {
                cout << "\nEpiloges member:\n" << "-> history\n" << "-> display\n" << "-> search\n" << "-> borrow\n" << "-> return\n" << "-> back\n> ";
                temp = getStringInput();

                if (temp == "back") {
                    goto menu_main;
                }
                else if (temp == "history") {
                    // nea pros8hkh (prospa8hsa)
                    string memberName = myLibrary.getActiveUser();
                    myLibrary.displayHistory(memberName);
                    goto menu_member;
                }
                else if (temp == "search") {
                    cout << "Dialekse tropo anazitisis:\n" << "-> title\n" << "-> author\n> ";
                    string searchType;
                    searchType = getStringInput();

                    cout << "Dwse timi gia anazitisi: ";
                    string searchValue = getStringInput();

                    vector<Book> results = myLibrary.searchBook(searchValue, searchType);
                    cout << "\n--- Apotelesmata anazitisis gia '" << searchValue << "' (" << searchType << ") ---\n";
                    if (results.empty()) {
                        cout << "-> To vivlio den vrethike.\n";
                    }
                    else {
                        for (const auto& book : results) {
                            cout << "-> Vrethike: " << book.title << " apo " << book.author
                                << " (Diathesima: " << book.getAvailableCopies() << ")\n";
                        }
                    }
                    cout << "--------------------------------------------------\n";

                    goto menu_member;
                }
                else if (temp == "display") {
                    myLibrary.displayBooks();
                    goto menu_member;
                }
                else if (temp == "borrow") {
                    cout << "Dwse ton titlo tou vivliou pou theleis na daneisteis: ";
                    string bookTitle = getStringInput();
                    myLibrary.borrowBook(bookTitle);
                    goto menu_member;
                }
                else if (temp == "return") {
                    cout << "Dwse ton titlo tou vivliou pou theleis na epistrepseis: ";
                    string bookTitle = getStringInput();
                    myLibrary.returnBook(bookTitle);
                    goto menu_member;
                }
                else {
                    cout << "Lathos epilogi, prospathise xana.\n";
                    goto menu_member;
                }
            }
        }
        else if (choice == "admin") {
        menu_admin:
            cout << "incert administrator code: ";
            string code = getStringInput();
            if (code != adminCode) {
                cout << "Lathos kwdikos, prospathise xana.\n";
                goto menu_main;
            }
            cout << "\nEpiloges Admin:\n" << "-> add_book\n" << "-> add_member\n" << "-> view_history\n" << "-> back\n> ";
            string temp = getStringInput();

            if (temp == "back") {
                goto menu_main;
            }
            else if (temp == "add_book") {
                cout << "Dwse onoma vivliou: ";
                string bookName = getStringInput();

                cout << "Dwse onoma syggraphea: ";
                string authorName = getStringInput();

                cout << "Dwse kathgoria: ";
                string category = getStringInput();

                cout << "Dwse arithmo antigrafon: ";
                int copies;
                cin >> copies;

                myLibrary.addBook(bookName, authorName, category, copies);
                goto menu_admin;
            }
            else if (temp == "add_member") {
                cout << "Dwse onomatepwnymo melous: ";
                string memberName = getStringInput();

                cout << "Dwse kwdiko gia to neo melos (arithmo): ";
                string memberCode = getStringInput();

                myLibrary.addMember(memberName, memberCode);
                goto menu_admin;
            }
            else if (temp == "view_history") {
                cout << "Dwse onoma melous gia na deis to istoriko tou: ";
                string memberName = getStringInput();
                myLibrary.displayHistory(memberName);
            }
            else {
                cout << "Lathos epilogi, prospathise xana.\n";
                goto menu_admin;
            }
        }
        else if (choice == "guest") {
        menu_guest:
            cout << "\nEpiloges Guest:\n" << "-> search\n" << "-> display\n" << "-> back\n> ";
            string temp = getStringInput();

            if (temp == "back") {
                goto menu_main;
            }
            else if (temp == "search") {
                cout << "Dialekse tropo anazitisis:\n" << "-> title\n" << "-> author\n> ";
                string searchType = getStringInput();

                cout << "Dwse timi gia anazitisi: ";
                string searchValue = getStringInput();

                vector<Book> results = myLibrary.searchBook(searchValue, searchType);
                cout << "\n--- Apotelesmata anazitisis gia '" << searchValue << "' (" << searchType << ") ---\n";
                if (results.empty()) {
                    cout << "-> To vivlio den vrethike.\n";
                }
                else {
                    for (const auto& book : results) {
                        cout << "-> Vrethike: " << book.title << " apo " << book.author
                            << " (Diathesima: " << book.getAvailableCopies() << ")\n";
                    }
                }
                cout << "--------------------------------------------------\n";
                goto menu_guest;
            }
            else if (temp == "display") {
                myLibrary.displayBooks();
                goto menu_guest;
            }
            else {
                cout << "Lathos epilogi, prospathise xana.\n";
                goto menu_guest;
            }
        }
        else {
            cout << "Lathos epilogi, prospathise xana.\n";
        }
    }

    myLibrary.saveData();
    return 0;
};