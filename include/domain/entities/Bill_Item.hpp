#pragma once
#include <iostream>
#include <string>

class BillItem {
private:
    std::string itemId;   // ID của pet hoặc service
    std::string itemName; // Tên
    std::string itemType; // "Pet" hoặc "Service"
    long price;

public:
    BillItem() : itemId(""), itemName(""), itemType(""), price(0) {}
    BillItem(const std::string &id, const std::string &name, const std::string &type, const long &p)
        : itemId(id), itemName(name), itemType(type), price(p) {}
    BillItem(const BillItem &item) {
        itemId = item.itemId;
        itemName = item.itemName;
        itemType = item.itemType;
        price = item.price;
    }
    // Có thể thêm operator =

    // GETTERS
    const std::string &getItemId() const { return itemId; }
    const std::string &getItemName() const { return itemName; }
    const std::string &getItemType() const { return itemType; }
    const long &getPrice() const { return price; }

    // SETTERS
    void setItemId(const std::string &id) { itemId = id; }
    void setItemName(const std::string &name) { itemName = name; }
    void setItemType(const std::string &type) { itemType = type; }
    void setPrice(long p) { price = p; }
    void setPrice(std::string p) { price = stol(p); }
};