#pragma once
#include <iostream>
#include <string>
using namespace std;

class Service {
private:
    string id;          // Mã dịch vụ (vd: SP001)
    string name;        // Tên dịch vụ spa
    string description; // Mô tả chi tiết
    long price;         // Giá dịch vụ
    int duration;       // Thời gian thực hiện (phút)

public:
    // Constructor
    Service() : id(""), name(""), description(""), price(0.0f), duration(0) {}

    Service(const string &id, const string &name, const string &desc, long price, int duration)
        : id(id), name(name), description(desc), price(price), duration(duration) {}

    // Getters
    string getId() const { return id; }
    string getName() const { return name; }
    string getDescription() const { return description; }
    long getPrice() const { return price; }
    int getDuration() const { return duration; }

    // Setters
    void setId(const string &id) { this->id = id; }
    void setName(const string &name) { this->name = name; }
    void setDescription(const string &desc) { this->description = desc; }
    void setPrice(long price) { this->price = price; }
    void setDuration(int duration) { this->duration = duration; }

    // Display info
    // void showInfo() const {
    //     cout << "=== SPA SERVICE INFO ===\n";
    //     cout << "ID: " << id << "\n";
    //     cout << "Name: " << name << "\n";
    //     cout << "Description: " << description << "\n";
    //     cout << "Price: " << price << " VND\n";
    //     cout << "Duration: " << duration << " minutes\n";
    //     cout << "========================\n";
    // }

    // Hiển thị dạng ngắn gọn
    void showBrief() const {
        cout << "[" << id << "] " << name << " - " << price << " VND (" << duration
             << " mins)\n";
    }

    friend std::ostream &operator<<(std::ostream &out, Service &serv);
};

inline std::ostream &operator<<(std::ostream &out, Service &serv) {
    out << "=== SPA SERVICE INFO ===\n";
    out << "ID: " << serv.id << "\n";
    out << "Name: " << serv.name << "\n";
    out << "Description: " << serv.description << "\n";
    out << "Price: " << serv.price << " VND\n";
    out << "Duration: " << serv.duration << " minutes\n";
    out << "========================\n";

    return out;
}
