#include "infra/repositories/BillRepoFile.hpp"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;
BillRepository::BillRepository(const std::string &billPath,
                               const std::string &billMasterPath)
    : billFolderPath(billPath), billMasterFilePath(billMasterPath) {
  // Tải file "tóm tắt" vào bộ nhớ
  _loadAllBillsFromFile();
}
long safe_stol_bill(const std::string &str) {
  if (str.empty())
    return 0L;
  try {
    return std::stol(str);
  } catch (const std::exception &e) {
    std::cerr << "Loi safe_stol_bill: " << e.what() << std::endl;
    return 0L;
  }
}
// --- HÀM TẢI DỮ LIỆU MỚI (THAY THẾ HÀM CŨ) ---
void BillRepository::_loadAllBillsFromFile() {
  ifstream file(billMasterFilePath); // <-- Dùng biến đường dẫn "tóm tắt"
  if (!file.is_open()) {
    cerr << "Luu y: Khong tim thay file bill_summary.txt. Se tao file moi khi "
            "co thanh toan."
         << '\n';
    return; // Không phải lỗi, chỉ là chưa có file
  }

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string billId, clientId, clientName, date, time, totalAmountStr;

    // Đọc 6 trường từ file "tóm tắt"
    getline(ss, billId, '|');
    getline(ss, clientId, '|');
    getline(ss, clientName, '|');
    getline(ss, date, '|');
    getline(ss, time, '|');
    getline(ss, totalAmountStr, '|');

    // Tạo đối tượng Bill "tóm tắt" (không có 'items')
    Bill bill(billId, clientId, clientName, date, time);

    // Dùng hàm setter để gán tổng tiền (CHÚNG TA SẼ THÊM HÀM NÀY Ở GIAI ĐOẠN 2)
    bill.setTotalAmount(safe_stol_bill(totalAmountStr));

    this->m_bills.pushBack(bill); // Thêm vào bộ nhớ
  }
  file.close();
}
string BillRepository::getCurrentDateTime(bool dateOnly) {
  time_t now = time(0);
  tm *ltm = localtime(&now);

  std::stringstream ss;
  ss << setfill('0') << setw(2) << ltm->tm_mday << "/" << setw(2)
     << (1 + ltm->tm_mon) << "/" << (1900 + ltm->tm_year);

  if (!dateOnly) {
    ss << " " << setw(2) << ltm->tm_hour << ":" << setw(2) << ltm->tm_min;
  }

  return ss.str();
}

string BillRepository::generateBillId() {
  int maxNum = 0;

  // Đọc tất cả files trong folder bills để tìm số lớn nhất
  // Simplified: chỉ tăng dần từ BILL001
  std::ifstream checkFile(billFolderPath + "counter.txt");
  if (checkFile.is_open()) {
    checkFile >> maxNum;
    checkFile.close();
  }

  maxNum++;

  // Lưu lại counter
  std::ofstream saveFile(billFolderPath + "counter.txt");
  saveFile << maxNum;
  saveFile.close();

  // Tạo ID
  std::stringstream ss;
  ss << "BILL" << setfill('0') << setw(3) << maxNum;
  return ss.str();
}
// Lưu hóa đơn vào file
void BillRepository::saveBill(const Bill &bill) {
  std::string filePath = billFolderPath + bill.getBillId() + ".txt";
  ofstream file(filePath);

  if (!file.is_open()) {
    cerr << "Error: Cannot create bill file: " << filePath << '\n';
    return;
  }

  // Header
  file << "========================================\n";
  file << "           SALES INVOICE             \n";
  file << "            DUT PETSHOP                \n";
  file << "========================================\n";
  file << "Invoice ID: " << bill.getBillId() << "\n";
  file << "Customer: " << bill.getClientName() << "\n";
  file << "ID: " << bill.getClientId() << "\n";
  file << "Date: " << bill.getDate() << "\n";
  file << "Time: " << bill.getTime() << "\n";
  file << "----------------------------------------\n";
  file << "Order Details:\n";
  file << "----------------------------------------\n";

  // Items
  int stt = 1;
  Node<BillItem> *item = bill.getItems().getHead();
  while (item != nullptr) {
    file << stt++ << ". " << item->getData().getItemName() << " ("
         << item->getData().getItemType() << ")\n";
    file << "   ID: " << item->getData().getItemId() << "\n";
    file << "   Price: " << fixed << setprecision(0)
         << item->getData().getPrice() << " VND\n";
    // if (item.quantity > 1) {
    //     file << "   So luong: " << item.quantity << "\n";
    // }
    file << "\n";
    item = item->getNext();
  }

  // Footer
  file << "----------------------------------------\n";
  file << "TOTAL: " << bill.getTotalAmount() << " VND\n";
  file << "========================================\n";
  file << "             THANK YOU!               \n";
  file << "       See you again next time!     \n";
  file << "========================================\n";

  file.close();
  this->m_bills.pushBack(bill);

  // 2b. Thêm vào file "bill_summary.txt" (để lần sau khởi động)
  std::ofstream masterFile(
      billMasterFilePath,
      std::ios::app); // Mở ở chế độ "append" (thêm vào cuối)
  if (masterFile.is_open()) {
    masterFile << bill.getBillId() << "|" << bill.getClientId() << "|"
               << bill.getClientName() << "|" << bill.getDate() << "|"
               << bill.getTime() << "|" << bill.getTotalAmount() << "\n";
    masterFile.close();
  } else {
    cerr << "Error: Cannot open bill master file: " << billMasterFilePath
         << '\n';
  }
  //   std::cout << "\n[SUCCESS] Hoa don da duoc luu tai: " << filePath << "\n";
}

// Tạo hóa đơn từ giỏ hàng
Bill BillRepository::createBillFromCart(const std::string &clientId,
                                        const std::string &clientName,
                                        const LinkedList<BillItem> &items) {
  std::string billId = generateBillId();
  std::string dateTime = getCurrentDateTime();

  // Tách date và time
  size_t spacePos = dateTime.find(' ');
  std::string date = dateTime.substr(0, spacePos);
  std::string time = dateTime.substr(spacePos + 1);

  Bill bill(billId, clientId, clientName, date, time);

  // Thêm items vào bill
  Node<BillItem> *current = items.getHead();
  while (current != nullptr) {
    bill.addItem(current->getData());
    current = current->getNext();
  }

  return bill;
}

// Đọc hóa đơn từ file
Bill BillRepository::loadBill(const std::string &billId) {
  Bill bill;
  std::string filePath = billFolderPath + billId + ".txt";
  ifstream file(filePath);

  if (!file.is_open()) {
    cerr << "Error: Cannot open bill file: " << filePath << '\n';
    return bill;
  }

  // Đọc và parse file (simplified)
  // Trong thực tế cần parse chi tiết hơn
  file.close();
  return bill;
}

// Lấy danh sách tất cả bills
LinkedList<std::string> BillRepository::getAllBillIds() {
  LinkedList<std::string> billIds;
  // Cần implement directory listing
  // Đơn giản hóa: đọc từ counter
  return billIds;
}

// Thống kê doanh thu
long BillRepository::calculateTotalRevenue() {
  long total = 0;
  // Đọc tất cả bills và tính tổng
  // Simplified version
  return total;
}
LinkedList<Bill> BillRepository::getAllBills() {
  // Trả về bản sao (hoặc tham chiếu) của danh sách tóm tắt
  return this->m_bills;
}
// Thống kê doanh thu từ bán pet
long BillRepository::calculatePetSalesRevenue() {
  long total = 0;
  // Đọc tất cả bills, lọc items có type = "Pet"
  return total;
}

// Thống kê doanh thu từ spa
long BillRepository::calculateSpaRevenue() {
  long total = 0;
  // Đọc tất cả bills, lọc items có type = "Service"
  return total;
}
LinkedList<Bill> BillRepository::getBillsByClientId(const string &clientId) {
  LinkedList<Bill> results;
  Node<Bill> *current = m_bills.getHead();
  while (current != nullptr) {
    if (current->getData().getClientId() == clientId) {
      results.pushBack(current->getData());
    }
    current = current->getNext();
  }
  return results;
}