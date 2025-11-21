/* App.hpp - Main Application Entry Point */

#pragma once
#include "app/Menu.hpp"
#include "app/controllers/AdminController.hpp"
#include "app/controllers/ClientController.hpp"
#include "app/controllers/StaffController.hpp"
#include "domain/services/AuthService.hpp"
#include "infra/repositories/AccountRepoFile.hpp"
#include <iostream>

using namespace std;

class App {
private:
  AuthService authService;
  AccountRepository accountRepo;

  void handleLogin() {
    // Menu::clearScreen();
    // Menu::displayHeader("LOGIN");

    Account *account = authService.login();

    if (account == nullptr) {
      Menu::displayError("Login failed!");
      Menu::pause();
      return;
    }

    // Xác định loại tài khoản dựa vào độ dài ID
    string id = account->getId();
    int idLength = id.length();

    if (idLength == 3) {
      // Admin account
      Admin *admin = dynamic_cast<Admin *>(account);
      if (admin != nullptr) {
        AdminController adminController(admin);
        adminController.run();
      }
    } else if (idLength == 5) {
      // Staff account
      Staff *staff = dynamic_cast<Staff *>(account);
      if (staff != nullptr) {
        StaffController staffController(staff);
        staffController.run();
      }
    } else if (idLength == 10) {
      // Client account
      Client *client = dynamic_cast<Client *>(account);
      if (client != nullptr) {
        ClientController clientController(client);
        clientController.run();
      }
    }

    delete account; // Clean up
  }

  void handleRegister() {
    Menu::clearScreen();
    Menu::displayHeader("CLIENT REGISTRATION");

    string id, name, password, gender, street, city;

    cout << "Enter Phone Number (10 digits): ";
    cin >> id;

    // Validate phone number
    if (id.length() != 10) {
      Menu::displayError("Phone number must be 10 digits!");
      Menu::pause();
      return;
    }

    // Check if ID already exists
    if (accountRepo.isValidId(id)) {
      Menu::displayError("This phone number is already registered!");
      Menu::pause();
      return;
    }

    cin.ignore();
    cout << "Enter Your Name: ";
    getline(cin, name);

    cout << "Enter Password: ";
    cin >> password;

    cout << "Enter Gender (male/female): ";
    cin >> gender;

    cin.ignore();
    cout << "Enter Street Address: ";
    getline(cin, street);

    cout << "Enter City: ";
    getline(cin, city);

    // Create new client account
    Client newClient(id, name, password, gender, street, city);
    accountRepo.setClientInfo(newClient);

    Menu::displaySuccess("Registration successful! You can now login.");
    Menu::pause();
  }

public:
  App() {}

  void run() {
    int choice;

    do {
      Menu::clearScreen();
      Menu::displayWelcome();
      Menu::displayMainMenu();
      cin >> choice;
      cin.ignore();

      switch (choice) {
      case 1:
        handleLogin();
        break;
      case 2:
        handleRegister();
        break;
      case 0:
        Menu::displayInfo("Thank you for using PetShop System!");
        Menu::displayInfo("Goodbye!");
        break;
      default:
        Menu::displayError("Invalid choice! Please try again.");
        Menu::pause();
      }

    } while (choice != 0);
  }
};