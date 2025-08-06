Command-Line Password Manager (C++)
A simple command-line tool to securely store, retrieve, and manage passwords. Passwords are encrypted using a basic XOR cipher and saved in a local file.

Features
Add, retrieve, delete password entries by service name

List all stored services

Encrypt/decrypt stored data using XOR cipher (for learning purposes)

Simple text file storage

Cross-platform CLI interface

Project Structure
PasswordManager/
├── main.cpp             // CLI interface and program entry point
├── PasswordManager.h    // Class and struct declarations
├── PasswordManager.cpp  // Password manager implementation
├── utils.h              // Utility functions (encryption)
├── utils.cpp            // XOR cipher implementation
├── README.md            // This documentation file
├── data/                // Folder for encrypted password database file
└── data/passwords.dat   // Encrypted password storage file (auto-generated)


Usage Guide
After running the program, you will see a menu:
===== Password Manager =====
1. Add Entry
2. Retrieve Entry
3. Delete Entry
4. List Services
5. Save & Exit
Choose an option:
Add Entry: Enter service, username, and password.

Retrieve Entry: Get credentials for a given service.

Delete Entry: Remove an entry by service name.

List Services: View all stored services.

Save & Exit: Save data and exit the program.

Encryption Details
Uses XOR cipher with a fixed key ('K') to encrypt and decrypt the stored data file.

The encrypted file is not human-readable, but the program automatically decrypts when loading.

This encryption is for educational/demo use only and is not secure for real passwords.

