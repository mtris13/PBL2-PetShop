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
  AccountRepository *m_accountRepo;
  const int AdminIdLength = 3;
  const int ClientIdLength = 10;
  const int StaffIdLength = 5;

public:
  AuthService(AccountRepository *repo) : m_accountRepo(repo) {}
  Account *validateLogin(const std::string &id, const std::string &password) {
    if (m_accountRepo->isValidPassword(id, password)) {
      return m_accountRepo->findAccountById(id);
    }
    return nullptr; // Trả về nullptr nếu thất bại
  }
  Account *login() {
    std::string id, pass;
    do {
      std::cout << "Enter your ID: ";
      std::cin >> id;
      std::cout << "Enter your password: ";
      std::cin >> pass;

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
      cin.ignore();
      break;
    }

    cout << "Enter your name: ";
    getline(cin, name);

    cout << "Enter your password: ";
    cin >> pw;

    cout << "1. Male\n2. Female\nEnter your gender: ";
    cin >> gender;
    gender = (gender == "1") ? "male" : "female";

    cout << "Enter your address: ";
    getline(cin, street);

    cout << "Enter your city: ";
    getline(cin, city);

    return Client(id, name, pw, gender, street, city);
  }
};
