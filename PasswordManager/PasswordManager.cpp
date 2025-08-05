#include "PasswordManager.h"
#include <fstream>
#include "utils.h"

const char ENCRYPTION_KEY = 'K';

void PasswordManager::loadDatabase(const string& filename) {
    ifstream infile(filename);  // Removed ios::binary
    if (!infile) {
        cout << "No existing database found. Starting fresh.\n";
        return;
    }

    entries.clear();
    string encryptedLine;
    while (getline(infile, encryptedLine)) {
        string decryptedLine = xorEncryptDecrypt(encryptedLine, ENCRYPTION_KEY);

        size_t pos1 = decryptedLine.find('|');
        size_t pos2 = decryptedLine.find('|', pos1 + 1);

        if (pos1 != string::npos && pos2 != string::npos) {
            PasswordEntry entry;
            entry.service = decryptedLine.substr(0, pos1);
            entry.username = decryptedLine.substr(pos1 + 1, pos2 - pos1 - 1);
            entry.password = decryptedLine.substr(pos2 + 1);
            entries.push_back(entry);
        }
    }
    infile.close();
}


void PasswordManager::saveDatabase(const string& filename) {
    ofstream outfile(filename);  // Removed ios::binary
    if (!outfile) {
        cout << "Error saving database.\n";
        return;
    }

    for (const auto& entry : entries) {
        string line = entry.service + "|" + entry.username + "|" + entry.password;
        string encryptedLine = xorEncryptDecrypt(line, ENCRYPTION_KEY);
        outfile << encryptedLine << '\n';
    }
    outfile.close();
}


void PasswordManager::addEntry(const string& service, const string& username, const string& password) {
    PasswordEntry entry = { service, username, password };
    entries.push_back(entry);
    cout << "Entry added successfully.\n";
}

PasswordEntry* PasswordManager::getEntry(const string& service) const {
    for (const auto& entry : entries) {
        if (entry.service == service) {
            return new PasswordEntry(entry);  // Return a heap-allocated copy
        }
    }
    return nullptr;
}

bool PasswordManager::deleteEntry(const string& service) {
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->service == service) {
            entries.erase(it);
            cout << "Entry deleted.\n";
            return true;
        }
    }
    cout << "Service not found.\n";
    return false;
}

void PasswordManager::listServices() const {
    if (entries.empty()) {
        cout << "No services stored.\n";
        return;
    }

    cout << "Stored Services:\n";
    for (const auto& entry : entries) {
        cout << "- " << entry.service << '\n';
    }
}
