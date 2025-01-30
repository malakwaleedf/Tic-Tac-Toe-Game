#pragma once

using namespace std;

uint64_t customHash(const string& str);
string hashPassword(const string& password_to_be_hashed);
string getPassword();
string signup(sqlite3* db);
string login(sqlite3* db);