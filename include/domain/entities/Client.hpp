#pragma once
#include "Account.hpp"
using namespace std;

class Client : public Account {
private:
    string street;
    // string district; // deleted this
    string city;

public:
    Client() : Account(), street(""), city("") {}
    Client(const string &id, const string &name, const string &password, const string &gender, const string &street, const string &city)
        : Account(id, name, password, gender), street(street), city(city) {}

    // Getter
    string getStreet() const { return street; }
    string getCity() const { return city; }
    string getAddress() const { return street + ", " + city; }

    // Setter
    void setStreet(const string &street) { this->street = street; }
    void setCity(const string &city) { this->city = city; }
};
