#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct PasswordEntry {
  string service;
  string username;
  string password;
};

class PasswordManager {
private:
  vector<PasswordEntry> entries;

public:
  void loadDatabase(const string& filename);
  void saveDatabase(const string& filename);
  void addEntry(const string& service, const string& username, const string& password);
  PasswordEntry* getEntry(const string& service) const;
  bool deleteEntry(const string& service);
  void listServices() const;
};

#endif // PASSWORD_MANAGER_H
