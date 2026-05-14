#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdio>

using namespace std;

static const string CSV_FILE = "electricity_bill_ledger.csv";
static const string CSV_HEADER = "consumer_id,name,unit";

static bool parseCSVLine(const string& line, string& consumerId, string& name, string& unit) {
    // Simple CSV split (no quoted commas expected for this assignment)
    // Expected: consumer_id,name,unit
    string a, b, c;
    string token;
    stringstream ss(line);

    if (!getline(ss, a, ',')) return false;
    if (!getline(ss, b, ',')) return false;
    if (!getline(ss, c, ',')) return false;

    consumerId = a;
    name = b;
    unit = c;
    return true;
}

// Mandatory Generic Functions

// initializeDatabase(): Checks for the CSV file and creates it with headers if it is missing.
void initializeDatabase() {
    ifstream fin(CSV_FILE.c_str());
    if (fin.good()) return; // exists

    ofstream fout(CSV_FILE.c_str(), ios::out);
    fout << CSV_HEADER << "\n";
    fout.close();
}

// isUnique(string id): Scans the CSV to prevent duplicate IDs during the "Write" process.
bool isUnique(string id) {
    ifstream fin(CSV_FILE.c_str());
    if (!fin.is_open()) return true; // if missing/unreadable, treat as unique

    string line;
    // Skip header
    if (!getline(fin, line)) return true;

    string consumerId, name, unit;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        if (!parseCSVLine(line, consumerId, name, unit)) continue;
        if (consumerId == id) {
            fin.close();
            return false;
        }
    }
    fin.close();
    return true;
}

// appendRecord(string data): Adds a new comma-separated record to the end of the CSV file.
void appendRecord(string data) {
    ofstream fout(CSV_FILE.c_str(), ios::app);
    fout << data << "\n";
    fout.close();
}

// searchByID(string id): Performs a sequential search to find and return a specific record.
string searchByID(string id) {
    ifstream fin(CSV_FILE.c_str());
    if (!fin.is_open()) return "";

    string line;
    // Skip header
    if (!getline(fin, line)) return "";

    string consumerId, name, unit;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        if (!parseCSVLine(line, consumerId, name, unit)) continue;

        if (consumerId == id) {
            fin.close();
            return line; // return full CSV record line
        }
    }

    fin.close();
    return "";
}

// updateRecord(string id, string newData): Modifies or deletes an entry using a temporary file to maintain data integrity.
void updateRecord(string id, string newData) {
    ifstream fin(CSV_FILE.c_str());
    if (!fin.is_open()) return;

    // Create temp file in same directory
    string tempFile = "electricity_bill_ledger_temp.csv";
    ofstream fout(tempFile.c_str(), ios::out);

    // Copy header
    string header;
    if (!getline(fin, header)) {
        fin.close();
        fout.close();
        return;
    }
    fout << header << "\n";

    string line;
    string consumerId, name, unit;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        if (!parseCSVLine(line, consumerId, name, unit)) continue;

        if (consumerId == id) {
            // Match found: if newData is empty => delete; else replace
            if (!newData.empty()) {
                fout << newData << "\n";
            }
        } else {
            fout << line << "\n";
        }
    }

    fin.close();
    fout.close();

    // Replace original with temp
    remove(CSV_FILE.c_str());
    rename(tempFile.c_str(), CSV_FILE.c_str());
}

static void printRecordLine(const string& line) {
    if (line.empty()) {
        cout << "Record not found.\n";
        return;
    }
    cout << "Found record: " << line << "\n";
}

int main() {
    initializeDatabase();

    while (true) {
        cout << "\n=== Electricity Bill Ledger (CSV) ===\n";
        cout << "1) Write/Add Consumer\n";
        cout << "2) Search by Consumer ID\n";
        cout << "3) Update/Delete Consumer\n";
        cout << "4) Exit\n";
        cout << "Choose: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            string id, name, unit;
            cout << "Enter consumer id: ";
            cin >> id;
            cout << "Enter name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, name);
            cout << "Enter unit: ";
            cin >> unit;

            if (!isUnique(id)) {
                cout << "Duplicate ID. Record not added.\n";
                continue;
            }

            string data = id + "," + name + "," + unit;
            appendRecord(data);
            cout << "Record added.\n";
        } else if (choice == 2) {
            string id;
            cout << "Enter consumer id to search: ";
            cin >> id;

            string record = searchByID(id);
            printRecordLine(record);
        } else if (choice == 3) {
            string id;
            cout << "Enter consumer id to update/delete: ";
            cin >> id;

            string existing = searchByID(id);
            if (existing.empty()) {
                cout << "ID not found. Nothing to update/delete.\n";
                continue;
            }

            cout << "Existing record: " << existing << "\n";
            cout << "1) Update\n";
            cout << "2) Delete\n";
            cout << "Choose: ";

            int op;
            if (!(cin >> op)) {
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }

            if (op == 1) {
                string name, unit;
                cout << "Enter new name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, name);
                cout << "Enter new unit: ";
                cin >> unit;

                string newData = id + "," + name + "," + unit;
                updateRecord(id, newData);
                cout << "Record updated.\n";
            } else if (op == 2) {
                updateRecord(id, "");
                cout << "Record deleted.\n";
            } else {
                cout << "Invalid option.\n";
            }
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    system("pause");
    return 0;
}
