#pragma once
#include "Pet.hpp"
#include <string>

class Dog : public Pet {
private:
  int energyLevel;

public:
  Dog() {}
  Dog(const std::string &id, const std::string &name, const std::string &breed,
      const int &age, const long &price, const bool &status,
      const int &energyLevel = 5,
      const std::string &desc = "No description yet.")
      : Pet(id, name, breed, age, price, status, desc),
        energyLevel(energyLevel) {}

  int getEnergyLevel() const { return energyLevel; }
  void setEnergyLevel(int value) { energyLevel = value; }

  std::string getDescription() const { return description; }
  void setDescription(const std::string &d) { description = d; }

  std::string getType() const override { return "Dog"; }

  void showInfo(std::ostream &out) const override {
    std::cout << "=== DOG INFO ===\n";
    Pet::showInfo(out);
    std::cout << "Energy Level: " << energyLevel << "/10\n";
  }
};