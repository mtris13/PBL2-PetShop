#pragma once
#include <iostream>
#include <string>
using namespace std;

class Account {
protected:
  string id;
  string name;
  string password;
  string gender;

public:
  Account() : id(""), name(""), password(""), gender("") {}
  Account(const string &Id, const string &Name, const string &Pass,
          const string &Gen)
      : id(Id), name(Name), password(Pass), gender(Gen) {}
  virtual ~Account() {}

  // Getter
  string getId() const { return id; }
  string getName() const { return name; }
  string getPassword() const { return password; }
  string getGender() const { return gender; }

  // Setter
  void setId(const string &id) { this->id = id; }
  void setName(const string &name) { this->name = name; }
  void setPassword(const string &password) { this->password = password; }
  void setGender(const string &gender) { this->gender = gender; }
};