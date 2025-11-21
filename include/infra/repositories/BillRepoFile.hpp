// BillRepoFile.hpp (ĐÃ SỬA)
#pragma once
#include "domain/entities/Bill.hpp"
#include "ds/LinkedList.hpp"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

class BillRepository {
private:
  // --- SỬA 1: Xóa 'const' và đường dẫn ---
  std::string billFolderPath;
  std::string billMasterFilePath; // Đường dẫn đến file "tóm tắt"
  LinkedList<Bill> m_bills;
  string getCurrentDateTime(bool dateOnly = false);

public:
  // --- SỬA 2: Sửa Constructor ---
  BillRepository(const std::string &billPath,
                 const std::string &billMasterPath);
  void _loadAllBillsFromFile();
  // ... (Các hàm còn lại giữ nguyên) ...
  std::string generateBillId();
  void saveBill(const Bill &bill);
  Bill createBillFromCart(const std::string &clientId,
                          const std::string &clientName,
                          const LinkedList<BillItem> &items);
  Bill loadBill(const std::string &billId);
  LinkedList<std::string> getAllBillIds();
  long calculateTotalRevenue();
  long calculatePetSalesRevenue();
  long calculateSpaRevenue();
  // ...
  LinkedList<Bill> getBillsByClientId(const string &clientId); // <-- THÊM MỚI
  LinkedList<Bill> getAllBills();
};