#pragma once
#include "Pet.hpp"
#include <iostream> // Cần cho ostream
#include <string>   // Nên include string

class Cat : public Pet {
private:
    std::string furLength; // Độ dài lông: Short, Medium, Long
public:
    Cat() {}
    Cat(const std::string &id, const std::string &name, const std::string &breed, const int &age,
        const long &price, const bool &status, const std::string &furLength = "Short", const std::string &desc = "No description yet.") // Thêm desc
        : Pet(id, name, breed, age, price, status, desc), furLength(furLength) {}                                                       // Khởi tạo description

    // --- Getter/Setter cho thuộc tính cũ và mới ---
    std::string getFurLength() const { return furLength; }
    void setFurLength(const std::string &value) { furLength = value; }

    // --- Hàm getType() ---
    std::string getType() const override { return "Cat"; }

    // --- Hàm showInfo() (cho console hoặc file) ---
    void showInfo(std::ostream &out) const override {
        out << "=== CAT INFO ===\n";
        Pet::showInfo(out); // Gọi hàm của class cha
        out << "Fur Length: " << furLength << "\n";
    }
};