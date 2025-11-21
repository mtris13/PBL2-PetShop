/* Chức năng đăng nhập */
#include "app/Menu.hpp"
#include "domain/entities/Admin.hpp"
#include "domain/entities/Client.hpp"
#include "domain/entities/Staff.hpp"
#include "infra/repositories/AccountRepoFile.hpp"
// #include <conio.h>
#include <iostream>

using namespace std;

class AuthService {
private:
  // 1. THAY ĐỔI LỚN: Dùng con trỏ để nhận repo từ bên ngoài
  AccountRepository *m_accountRepo; // Dùng con trỏ
  const int AdminIdLength = 3;
  const int ClientIdLength = 10;
  const int StaffIdLength = 5;

public:
  // 2. THAY ĐỔI LỚN: Dùng Constructor Injection
  AuthService(AccountRepository *repo) : m_accountRepo(repo) {}

  // 3. HÀM MỚI CHO GUI:
  //    Chỉ làm logic, không cin/cout
  Account *validateLogin(const std::string &id, const std::string &password) {
    if (m_accountRepo->isValidPassword(id, password)) {
      return m_accountRepo->findAccountById(id);
    }
    return nullptr; // Trả về nullptr nếu thất bại
  }

  // --- Hàm login() cũ của bạn (vẫn giữ nguyên cho bản terminal) ---
  Account *login() {
    std::string id, pass;
    do {
      // Menu::clearScreen(); // Qt sẽ tự xóa
      // Menu::displayHeader("LOGIN"); // Qt hiển thị
      std::cout << "Enter your ID: ";
      std::cin >> id;
      std::cout << "Enter your password: ";
      std::cin >> pass;

      // Dùng hàm validateLogin mới
      Account *user = validateLogin(id, pass);
      if (user != nullptr) {
        std::cout << "Login successfully! Welcome to PETSHOP.\n";
        return user;
      }

      std::cout << "Your ID or password is not correct!\n";
      std::cin.get();
      system("pause");
    } while (true);
  }

  Client createClientAccount() {
    string id, name, pw, gender, street, city;

    while (true) {
      cout << "Enter your phone number (ID): ";
      cin >> id;
      if (id.length() != ClientIdLength) {
        cout << "Invalid phone number!\n";
        continue;
      }
      cin.ignore(); // xóa '\n' còn lại sau khi nhập id
      break;
    }

    cout << "Enter your name: ";
    getline(cin, name);

    cout << "Enter your password: ";
    cin >> pw;

    cout << "1. Male\n2. Female\nEnter your gender: ";
    cin >> gender;
    gender = (gender == "1") ? "male" : "female";

    cin.ignore(); // xóa '\n' trước khi dùng getline

    cout << "Enter your address: ";
    getline(cin, street);

    cout << "Enter your city: ";
    getline(cin, city);

    return Client(id, name, pw, gender, street, city);
  }

  // WILL REMAKE

  // Account *login(const string &username, const string &password) {
  //   Account *acc = accountRepo.findAccountByNameAndPass(username, password);
  //   if (acc == nullptr) {
  //     cout << "Sai ten dang nhap hoac mat khau!\n";
  //     return nullptr;
  //   }

  //   cout << "Dang nhap thanh cong! Xin chao " << acc->getAccountName() << "
  //   ("
  //        << acc->getRole() << ")\n";

  //   // displayAccountInfo(acc);
  //   return acc;
  // }

  // void displayAccountInfo(Account *acc) {
  //   if (acc == nullptr) {
  //     cout << "Khong co tai khoan de hien thi.\n";
  //     return;
  //   }

  //   cout << "\n--- Thong tin tai khoan ---\n";
  //   cout << "ID: " << acc->getAccountId() << "\n";
  //   cout << "Ten dang nhap: " << acc->getAccountName() << "\n";
  //   cout << "Gioi tinh: " << acc->getGender() << "\n";
  //   cout << "Vai tro: " << acc->getRole() << "\n";

  //   // Kiem tra xem co phai Client hoac Staff khong
  //   if (acc->getRole() == "C") {
  //     Client *c = dynamic_cast<Client *>(acc);
  //     if (c != nullptr) {
  //       cout << "Dia chi: " << c->getStreet() << ", " << c->getDistrict()
  //            << ", " << c->getCity() << "\n";
  //     }
  //   } else if (acc->getRole() == "S") {
  //     Staff *s = dynamic_cast<Staff *>(acc);
  //     if (s != nullptr) {
  //       cout << "Luong: " << s->getSalary() << "\n";
  //     }
  //   }

  //   cout << "---------------------------\n";
  // }
};
