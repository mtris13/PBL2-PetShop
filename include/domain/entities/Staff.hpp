#pragma once

#include "Account.hpp"

class Staff : public Account {
private:
    long salary;

public:
    Staff() : Account(), salary(0.0) {}
    Staff(const string &Id, const string &Name, const string &Pass, const string &Gen, const long &Salary) : Account(Id, Name, Pass, Gen), salary(Salary) {}
    long getSalary() const { return salary; };              // getter
    void setSalary(long salary) { this->salary = salary; }; // setter
};