#pragma once

#include <iostream>
#include <string>

class CartItem {
private:
  std::string petId;
  std::string petName;
  long price;

public:
  CartItem() : petId(""), petName(""), price(0) {}
  CartItem(const std::string &id, const std::string &name, long p)
      : petId(id), petName(name), price(p) {}
  // GET
  std::string getPetId() { return petId; }
  std::string getPetName() { return petName; }
  long getPrice() { return price; }
};