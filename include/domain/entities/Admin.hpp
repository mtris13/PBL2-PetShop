#pragma once

#include "Account.hpp"

class Admin : public Account {
public:
    Admin() : Account() {}
    Admin(const string &Id, const string &Name, const string &Pass, const string &Gen)
        : Account(Id, Name, Pass, Gen) {}
};