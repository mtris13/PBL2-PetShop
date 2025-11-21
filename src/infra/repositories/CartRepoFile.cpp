#include "infra/repositories/CartRepoFile.hpp"
#include "app/Menu.hpp"
using namespace std;

// PRIVATE
bool CartRepository::isAvailableId(const string &petCode,
                                   const string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open())
    return false;
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    string id = "";
    for (int i = 0; i < 4; i++)
      id += line[i];
    if (id == petCode)
      return true;
  }
  return false;
}

// Thêm pet vào giỏ hàng
void CartRepository::addToCart(const string &clientId, const string &petId,
                               const string &petName, long price) {
  string filePath = getCartFilePath(clientId);
  ofstream file(filePath, ios::app);

  if (!file.is_open()) {
    cerr << "Error: Cannot open cart file for " << clientId << '\n';
    return;
  }

  if (isAvailableId(petId, filePath))
    return; // đã tồn tại trong giỏ hàng

  file << petId << "|" << petName << "|" << to_string(price) << "\n";
  file.close();
}

// Lấy tất cả items trong giỏ hàng
LinkedList<CartItem> CartRepository::getCartItems(const string &clientId) {
  LinkedList<CartItem> items;
  string filePath = getCartFilePath(clientId);
  ifstream file(filePath);

  if (!file.is_open()) {
    // Giỏ hàng trống hoặc chưa tồn tại
    return items;
  }

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id, name, priceStr;

    getline(ss, id, '|');
    getline(ss, name, '|');
    getline(ss, priceStr, '|');

    items.pushBack(CartItem(id, name, stol(priceStr)));
  }

  file.close();
  return items;
}

// Xóa một item khỏi giỏ hàng
void CartRepository::removeFromCart(const string &clientId,
                                    const string &petId) {
  string filePath = getCartFilePath(clientId);
  string tempPath = cartFolderPath + "temp.txt";

  ifstream in(filePath);
  ofstream out(tempPath);

  if (!in.is_open()) {
    cerr << "Error: Cart file not found\n";
    return;
  }

  if (!out.is_open()) {
    cerr << "Error: Cannot create temp file\n";
    in.close();
    return;
  }

  string line;
  while (getline(in, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id;
    getline(ss, id, '|');

    if (id != petId) {
      out << line << "\n";
    }
  }

  in.close();
  out.close();

  remove(filePath.c_str());
  rename(tempPath.c_str(), filePath.c_str());
}

// Xóa toàn bộ giỏ hàng (sau khi thanh toán)
void CartRepository::clearCart(const string &clientId) {
  string filePath = getCartFilePath(clientId);
  ofstream file(filePath, ios::trunc);
  file.close();
}

// Kiểm tra giỏ hàng có trống không
bool CartRepository::isCartEmpty(const string &clientId) {
  LinkedList<CartItem> items = getCartItems(clientId);
  return items.isEmpty();
}

// Tính tổng tiền trong giỏ hàng
long CartRepository::calculateTotal(const string &clientId) {
  LinkedList<CartItem> items = getCartItems(clientId);
  long total = 0;
  Node<CartItem> *current = items.getHead();
  while (current != nullptr) {
    total += current->getData().getPrice();
    current = current->getNext();
  }
  return total;
}

// Hiển thị giỏ hàng
void CartRepository::displayCart(const string &clientId) {
  LinkedList<CartItem> items = getCartItems(clientId);

  if (items.isEmpty()) {
    cout << "\nGio hang trong!\n";
    return;
  }

  Menu::displayHeader("GIO HANG CUA BAN");

  cout << left << setw(5) << "STT" << setw(10) << "ID Pet" << setw(25)
       << "Ten Pet" << right << setw(15) << "Gia (VND)\n";
  cout << string(55, '-') << "\n";

  int stt = 1;
  long total = 0;
  Node<CartItem> *current = items.getHead();
  while (current != nullptr) {
    string petId = current->getData().getPetId();
    if (m_petRepo->isAvailablePet(petId)) {
      cout << left << setw(5) << stt++ << setw(10) << petId << setw(25)
           << current->getData().getPetName() << right << setw(15) << fixed
           << setprecision(0) << current->getData().getPrice() << "\n";
      total += current->getData().getPrice();
    }
    current = current->getNext();
  }

  cout << string(55, '-') << "\n";
  cout << right << setw(45) << "Tong cong: " << setw(10) << fixed
       << setprecision(0) << total << " VND\n";
  cout << string(55, '=') << "\n";
}