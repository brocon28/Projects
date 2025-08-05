#include <iostream>
#include "PasswordManager.h"

using namespace std;

int main() {
    PasswordManager manager;
    manager.loadDatabase("data/passwords.dat");

    bool running = true;
    while (running) {
        cout << "\n===== Password Manager =====\n";
        cout << "1. Add Entry\n";
        cout << "2. Retrieve Entry\n";
        cout << "3. Delete Entry\n";
        cout << "4. List Services\n";
        cout << "5. Save & Exit\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;
        cin.ignore();  // Clear newline from input buffer

        switch (choice) {
            case 1: {
                string service, username, password;
                cout << "Service Name: ";
                getline(cin, service);
                cout << "Username: ";
                getline(cin, username);
                cout << "Password: ";
                getline(cin, password);

                manager.addEntry(service, username, password);
                break;
            }
            case 2: {
                string service;
                cout << "Enter service to retrieve: ";
                getline(cin, service);

                PasswordEntry* entry = manager.getEntry(service);
                if (entry) {
                    cout << "Username: " << entry->username << '\n';
                    cout << "Password: " << entry->password << '\n';
                    delete entry;  // Free heap-allocated entry
                } else {
                    cout << "Service not found.\n";
                }
                break;
            }
            case 3: {
                string service;
                cout << "Enter service to delete: ";
                getline(cin, service);

                manager.deleteEntry(service);
                break;
            }
            case 4:
                manager.listServices();
                break;

            case 5:
                manager.saveDatabase("data/passwords.dat");
                cout << "Database saved. Exiting.\n";
                running = false;
                break;

            default:
                cout << "Invalid option. Try again.\n";
        }
    }

    return 0;
}
