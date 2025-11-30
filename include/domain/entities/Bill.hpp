#pragma once
#include "Bill_Item.hpp"
#include "ds/LinkedList.hpp"
#include <iostream>
#include <string>

class Bill {
private:
  std::string billId;         // Mã hóa đơn (BILL001)
  std::string clientId;       // ID khách hàng
  std::string clientName;     // Tên khách hàng
  LinkedList<BillItem> items; // Danh sách sản phẩm/dịch vụ
  long totalAmount;           // Tổng tiền
  std::string date;           // Ngày tạo hóa đơn
  std::string time;           // Giờ tạo hóa đơn

public:
  Bill()
      : billId(""), clientId(""), clientName(""), totalAmount(0), date(""),
        time("") {}

  Bill(const std::string &id, const std::string &cId, const std::string &cName,
       const std::string &d, const std::string &t)
      : billId(id), clientId(cId), clientName(cName), totalAmount(0), date(d),
        time(t) {}
  Bill(const Bill &other)
      : billId(other.billId), clientId(other.clientId),
        clientName(other.clientName), items(other.items), // <-- cần deep copy
        totalAmount(other.totalAmount), date(other.date), time(other.time) {}

  Bill &operator=(const Bill &other) {
    if (this == &other)
      return *this;
    billId = other.billId;
    clientId = other.clientId;
    clientName = other.clientName;
    items = other.items; // <-- deep copy
    totalAmount = other.totalAmount;
    date = other.date;
    time = other.time;
    return *this;
  }
  std::string getBillId() const { return billId; }
  std::string getClientId() const { return clientId; }
  std::string getClientName() const { return clientName; }
  const LinkedList<BillItem> &getItems() const { return items; }
  long getTotalAmount() const { return totalAmount; }
  std::string getDate() const { return date; }
  std::string getTime() const { return time; }
  void setBillId(const std::string &id) { billId = id; }
  void setClientId(const std::string &id) { clientId = id; }
  void setClientName(const std::string &name) { clientName = name; }
  void setDate(const std::string &d) { date = d; }
  void setTime(const std::string &t) { time = t; }

  void addItem(const BillItem &item) {
    items.pushBack(item);
    totalAmount += item.getPrice(); // Mỗi pet chỉ có số lượng là 1
  }
  void setTotalAmount(long total) { this->totalAmount = total; }
  void setItems(const LinkedList<BillItem> &items) {
    this->items = items;
    // (Có thể bạn cần tính lại totalAmount ở đây nếu logic của bạn yêu cầu)
  }
  void recalculateTotal() {
    long total = 0;
    Node<BillItem> *item = items.getHead();
    while (item != nullptr) {
      total += item->getData().getPrice();
      item = item->getNext();
    }
    totalAmount = total;
  }
  void display() const {
    std::cout << "\n========================================\n";
    std::cout << "           HOA DON BAN HANG             \n";
    std::cout << "========================================\n";
    std::cout << "Ma hoa don: " << billId << "\n";
    std::cout << "Khach hang: " << clientName << " (" << clientId << ")\n";
    std::cout << "Ngay: " << date << " " << time << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "STT | Ten san pham         | Gia      \n";
    std::cout << "----------------------------------------\n";

    int stt = 1;
    Node<BillItem> *item = items.getHead();
    while (item != nullptr) {
      std::cout << stt++ << ".  | " << item->getData().getItemName() << " ("
                << item->getData().getItemType() << ") | "
                << item->getData().getPrice() << "\n";
      item = item->getNext();
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Tong cong: " << totalAmount << " VND\n";
    std::cout << "========================================\n";
    std::cout << "      Cam on quy khach!                 \n";
    std::cout << "========================================\n";
  }
};