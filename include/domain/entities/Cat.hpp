#pragma once
#include "Pet.hpp"
#include <iostream>
#include <string>

class Cat : public Pet {
private:
  std::string furLength;

public:
  Cat() {}
  Cat(const std::string &id, const std::string &name, const std::string &breed,
      const int &age, const long &price, const bool &status,
      const std::string &furLength = "Short",
      const std::string &desc = "No description yet.")
      : Pet(id, name, breed, age, price, status, desc), furLength(furLength) {}

  std::string getFurLength() const { return furLength; }
  void setFurLength(const std::string &value) { furLength = value; }

  std::string getType() const override { return "Cat"; }

  void showInfo(std::ostream &out) const override {
    out << "=== CAT INFO ===\n";
    Pet::showInfo(out);
    out << "Fur Length: " << furLength << "\n";
  }
};