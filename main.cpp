#include <iostream> // for cout and cin
#include <stdlib.h> //  for exit and system
#include <fstream>  // for file handling ofstream and ifstream
#include <sstream>  // for string stream
#include <string>   // for string class

using namespace std;

string fname = "Electricity bill ledger.csv"; // using a variable so don't need to change everywhere.

const string RED = "\033[31m";  // for red colour {Errors}
const string RESET = "\033[0m"; // for reseting colours

void initializeDatabase();    // checks file exists else create it.
bool isUnique(string id);     // checks if consumer id is unique
void appendRecord();          // adds a new comma separated record to the end of the CSV file
string searchByID(string id); // performs a search to find and return a specific record
void updateRecord();          // modifies or deletes an entry
bool parse(string line, string &id, string &name, string &unit);

int main()
{
    string id;
    initializeDatabase(); // step 1: Initialization of database

    while (true)
    {
        // step 2: user interaction {menu}
        cout << "\n=== Electricity Bill Ledger (CSV) ===\n1) Write/Add Consumer\n2) Search by Consumer ID\n3) Update/Delete Consumer\n4) Exit\nChoose: ";

        int choice;
        if (!(cin >> choice)) // // checking for newline characrter or invalid characters
        {
            cin.clear();             // clear warning flags
            cin.ignore(10000, '\n'); // ignore characters in input buffer
            continue;
        }
        if (choice == 1)
        {
            appendRecord();
        }
        else if (choice == 2)
        {
            string id; // getting the id to be checked
            cout << "Enter Consumer ID: ";
            cin >> id;
            searchByID(id);
        }
        else if (choice == 3)
        {
            updateRecord();
        }
        else if (choice == 4)
        {
            break;
        }
    }
    // system("pause");
    return 0;
}

void initializeDatabase()
{

    ifstream fin(fname); // open file in read mode
    if (fin.is_open())
        return; // file exists and can be opened than return

    // file doesn't exist so create it
    ofstream fout(fname); // open file in write mode

    fout << "consumer_id,name,unit" << endl; // initialize file with headers

    fout.close(); // close file
}

bool isUnique(string id)
{
    ifstream fin(fname);
    if (!fin.is_open())
    {
        cout << RED << "File is missing or unreadable" << RESET << endl;
        return true; // if missing/unreadable, then treat as unique
    }

    // check uniqueness
    string line;             // holds current line of the file
    if (!getline(fin, line)) // skip header
    {
        cout << RED << "File is empty or missing header" << RESET << endl;
        return true;
    }

    // loop through file
    string consumerId, name, unit;
    while (getline(fin, line))
    {
        if (line.empty()) // skip empty lines
            continue;
        if (!parse(line, consumerId, name, unit)) // parse line
            continue;
        if (consumerId == id) // check if consumerId is unique
        {
            cout << RED << "Consumer ID is not unique" << RESET << endl;
            return false;
        }
    }
    fin.close();
    return true;
}

bool parse(string line, string &id, string &name, string &unit)
{
    string a, b, c;        // temp variables
    stringstream ss(line); // stringstream for string parsing

    if (!getline(ss, a, ',')) // get id
        return false;         // return false if not found
    if (!getline(ss, b, ',')) // get name
        return false;         // return false if not found
    if (!getline(ss, c, ',')) // get unit
        return false;         // return false if not found

    // assign values
    id = a;
    name = b;
    unit = c;

    return true; // return true if found
}

void appendRecord()
{
    string id, name, unit;
    cout << "Enter Consumer ID: ";
    cin >> id;
    cout << "Enter Name: ";
    cin.ignore(); // ignore newline
    getline(cin, name);
    cout << "Enter Unit: ";
    cin >> unit;

    // check if id is unique
    if (isUnique(id) == false)
        return;

    // add record
    ofstream fout(fname, ios::app); // open file in append mode
    if (!fout.is_open())
    {
        cout << RED << "File is missing or unreadable" << RESET << endl;
        return;
    }
    fout << id << "," << name << "," << unit << endl;
    fout.close();
}

string searchByID(string id)
{
    ifstream fin(fname);
    if (!fin.is_open())
    {
        cout << RED << "File is missing or unreadable" << RESET << endl;
        return "";
    }

    string line, consumerId, name, unit;
    if (!getline(fin, line)) // skip header
    {
        cout << RED << "File is empty or missing header" << RESET << endl;
        return "";
    }

    while (getline(fin, line))
    {
        if (line.empty()) // skip empty lines
            continue;
        if (!parse(line, consumerId, name, unit)) // parse line and skip if data not found
            continue;
        if (consumerId == id) // check if consumerId is unique
        {
            cout << "Found record: " << line << endl;
            return line;
        }
    }
    cout << RED << "Record not found" << RESET << endl;
    return "";
}

void updateRecord()
{
    string id; // getting the id to be checked
    cout << "Enter Consumer ID: ";
    cin >> id;

    // getting the string to be checked
    string valid = searchByID(id);
    if (valid.empty()) // check if record is found
        return;

    cout << "1) Update\n";
    cout << "2) Delete\n";
    cout << "Choose: ";

    int option;
    if (!(cin >> option))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    ifstream fin(fname);
    if (!fin.is_open())
    {
        cout << RED << "File is missing or unreadable" << RESET << endl;
        return;
    }
    string header;
    if (!getline(fin, header)) // skip header
    {
        fin.close();
        cout << RED << "File is empty or missing header" << RESET << endl;
        return;
    }

    string temp_file = "temp.csv";
    ofstream fout(temp_file);

    fout << header << "\n"; // write header to temp file

    string line, consumerId, name, unit;

    while (getline(fin, line))
    {
        if (line.empty()) // skip empty lines
            continue;
        if (!parse(line, consumerId, name, unit)) // parse line and skip if data not found
            continue;

        if (consumerId == id && option == 2) // check if consumerId is unique
        {
            continue;
        }
        else if (consumerId == id && option == 1) // check if consumerId is unique
        {
            cout << "Enter Name: ";
            cin.ignore(); // ignore newline
            getline(cin, name);
            cout << "Enter Unit: ";
            cin >> unit;
            fout << id << "," << name << "," << unit << endl;
        }
        else
        {
            fout << line << endl;
        }
    }

    fin.close();
    fout.close();

    remove(fname.c_str());
    rename(temp_file.c_str(), fname.c_str());
}
