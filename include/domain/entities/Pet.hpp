#pragma once
#include <iostream>
#include <string>

class Pet {
protected:
    std::string id;
    std::string name;
    std::string breed;
    int age;
    long price;
    bool status;
    std::string description;

public:
    Pet() {}
    Pet(const std::string &id, const std::string &name, const std::string &breed, const int &age, const long &price, const bool &status, const std::string &description)
        : id(id), name(name), breed(breed), age(age), price(price), status(status), description(description) {}

    virtual ~Pet() = default;

    // Getter
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getBreed() const { return breed; }
    int getAge() const { return age; }
    long getPrice() const { return price; }
    // bool getStatus() const { return status; }
    std::string getStatus() const { return ((status == 1) ? "available" : "unvailable"); }
    std::string getDescription() const { return description; }

    // Setter
    void setName(const std::string &newName) { name = newName; }
    void setBreed(const std::string &newBreed) { breed = newBreed; }
    void setAge(int newAge) { age = newAge; }
    void setPrice(long newPrice) { price = newPrice; }
    void setStatus(bool newStatus) { status = newStatus; }
    void setDescription(const std::string &d) { description = d; }

    // Phương thức ảo để biết loại thú
    virtual std::string getType() const = 0;

    virtual void showInfo(std::ostream &out) const {
        out << "ID: " << id << "\n"
            << "Name: " << name << "\n"
            << "Breed: " << breed << "\n"
            << "Age: " << age << "\n"
            << "Price: " << price << "\n"
            << "Status: " << ((status == 1) ? "available\n" : "unvailable\n")
            << "Description: " << description << "\n";
    }
    friend std::ostream &operator<<(std::ostream &out, const Pet &pet);
};

inline std::ostream &operator<<(std::ostream &out, const Pet &pet) {
    pet.showInfo(out);
    return out;
}