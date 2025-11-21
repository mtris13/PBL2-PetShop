/* Menu.hpp - Hiển thị các menu cho từng loại user */

#pragma once
#include <iostream>
#include <string>
using namespace std;

class Menu {
public:
  // ===== MAIN MENU =====
  static void displayWelcome() {
    cout << "\n";
    cout << "========================================\n";
    cout << "     WELCOME TO TISU PETSHOP          \n";
    cout << "========================================\n";
  }

  static void displayMainMenu() {
    cout << "\n========== MAIN MENU ==========\n";
    cout << "1. Login\n";
    cout << "2. Register (Client)\n";
    cout << "0. Exit\n";
    cout << "===============================\n";
    cout << "Enter your choice: ";
  }

  // ===== ADMIN MENU =====
  static void displayAdminMenu(const string &adminName) {
    cout << "\n========================================\n";
    cout << "  ADMIN PANEL - " << adminName << "\n";
    cout << "========================================\n";
    cout << "1. Manage Accounts\n";
    cout << "2. Manage Pets\n";
    cout << "3. Manage Spa Services\n";
    cout << "4. Manage Bookings\n";
    cout << "5. View Statistics\n";
    cout << "0. Logout\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
  }

  static void displayAccountManagementMenu() {
    cout << "\n===== ACCOUNT MANAGEMENT =====\n";
    cout << "1. View All Accounts\n";
    cout << "2. Add Staff Account\n";
    cout << "3. Update Account\n";
    cout << "4. Delete Account\n";
    cout << "5. View Account Details\n";
    cout << "0. Back\n";
    cout << "==============================\n";
    cout << "Enter your choice: ";
  }

  static void displayPetManagementMenu() {
    cout << "\n===== PET MANAGEMENT =====\n";
    cout << "1. View All Pets\n";
    cout << "2. Add New Pet\n";
    cout << "3. Update Pet Info\n";
    cout << "4. Delete Pet\n";
    cout << "5. Search Pet\n";
    cout << "0. Back\n";
    cout << "==========================\n";
    cout << "Enter your choice: ";
  }

  static void displaySpaManagementMenu() {
    cout << "\n===== SPA SERVICE MANAGEMENT =====\n";
    cout << "1. View All Services\n";
    cout << "2. Add New Service\n";
    cout << "3. Update Service\n";
    cout << "4. Delete Service\n";
    cout << "0. Back\n";
    cout << "==================================\n";
    cout << "Enter your choice: ";
  }

  static void displayBookingManagementMenu() {
    cout << "\n===== BOOKING MANAGEMENT =====\n";
    cout << "1. View All Bookings\n";
    cout << "2. View Bookings by Date\n";
    cout << "3. Confirm Booking\n";
    cout << "4. Complete Booking\n";
    cout << "5. Cancel Booking\n";
    cout << "6. Delete Booking\n";
    cout << "0. Back\n";
    cout << "==============================\n";
    cout << "Enter your choice: ";
  }

  // ===== STAFF MENU =====
  static void displayStaffMenu(const string &staffName) {
    cout << "\n========================================\n";
    cout << "  STAFF PANEL - " << staffName << "\n";
    cout << "========================================\n";
    cout << "1. View Pets\n";
    cout << "2. View Spa Services\n";
    cout << "3. Manage Bookings\n";
    cout << "4. View Today's Schedule\n";
    cout << "0. Logout\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
  }

  static void displayStaffBookingMenu() {
    cout << "\n===== BOOKING MANAGEMENT =====\n";
    cout << "1. View All Bookings\n";
    cout << "2. View Bookings by Date\n";
    cout << "3. Confirm Booking\n";
    cout << "4. Complete Booking\n";
    cout << "0. Back\n";
    cout << "==============================\n";
    cout << "Enter your choice: ";
  }

  // ===== CLIENT MENU =====
  static void displayClientMenu(const string &clientName) {
    cout << "\n========================================\n";
    cout << "  CLIENT PANEL - " << clientName << "\n";
    cout << "========================================\n";
    cout << "1. View Pets for Sale\n";
    cout << "2. View Spa Services\n";
    cout << "3. Book Spa Service\n";
    cout << "4. My Bookings\n";
    cout << "5. Cancel Booking\n";
    cout << "6. My Profile\n";
    cout << "0. Logout\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
  }

  // ===== UTILITY FUNCTIONS =====
  static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
  }

  static void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
  }

  static void displayHeader(const string &title) {
    cout << "\n========================================\n";
    cout << "  " << title << "\n";
    cout << "========================================\n";
  }

  static void displaySuccess(const string &message) {
    cout << "\n[SUCCESS] " << message << "\n";
  }

  static void displayError(const string &message) {
    cout << "\n[ERROR] " << message << "\n";
  }

  static void displayInfo(const string &message) {
    cout << "\n[INFO] " << message << "\n";
  }
};