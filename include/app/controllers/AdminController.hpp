/* AdminController.hpp - Logic xử lý cho Admin (COMPLETED) */

#pragma once
#include "app/Menu.hpp"
#include "domain/entities/Admin.hpp"
#include "domain/entities/Cat.hpp"
#include "domain/entities/Client.hpp"
#include "domain/entities/Dog.hpp"
#include "domain/entities/Staff.hpp"
#include "domain/services/SpaService.hpp"
#include "infra/repositories/AccountRepoFile.hpp"
#include "infra/repositories/BillRepoFile.hpp"
#include "infra/repositories/BookingRepoFile.hpp"
#include "infra/repositories/PetRepoFile.hpp"
#include "infra/repositories/ServiceRepoFile.hpp"
#include <iomanip>
#include <iostream>

using namespace std;

class AdminController {
private:
    const int AdminIdLength = 3;
    const int StaffIdLength = 5;
    const int ClientIdLength = 10;
    Admin *currentAdmin;
    AccountRepository accountRepo;
    PetRepository petRepo;
    SpaService spaService;
    BillRepository billRepo;
    BookingRepository bookingRepo;
    ServiceRepository serviceRepo;

    // ===== ACCOUNT MANAGEMENT =====
    void manageAccounts() {
        int choice;
        do {
            Menu::clearScreen();
            Menu::displayAccountManagementMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                viewAllAccounts();
                break;
            case 2:
                addStaffAccount();
                break;
            case 3:
                updateAccount();
                break;
            case 4:
                deleteAccount();
                break;
            case 5:
                viewAccountDetails();
                break;
            case 0:
                break;
            default:
                Menu::displayError("Invalid choice!");
            }

            if (choice != 0)
                Menu::pause();
        } while (choice != 0);
    }

    void viewAllAccounts() {
        Menu::displayHeader("ALL ACCOUNTS");
        int count = 0;

        // Admin accounts
        cout << "\n========== ADMIN ACCOUNTS ==========\n";
        cout << setw(8) << left << "ID" << setw(25) << left << "Name" << setw(10)
             << left << "Gender" << "\n";
        cout << string(43, '-') << "\n";

        LinkedList<Admin> admin = accountRepo.getAllAdminInfo();
        Node<Admin> *ad = admin.getHead();
        while (ad != nullptr) {
            cout << setw(8) << left << ad->getData().getId() << setw(25) << left << ad->getData().getName() << setw(10)
                 << left << ad->getData().getGender() << "\n";
            ad = ad->getNext();
            count++;
        }
        cout << "Total: " << count << " admin(s)\n";
        count = 0;

        // Staff accounts
        cout << "\n========== STAFF ACCOUNTS ==========\n";
        cout << setw(8) << left << "ID" << setw(25) << left << "Name" << setw(10)
             << left << "Gender" << setw(15) << right << "Salary" << "\n";
        cout << string(58, '-') << "\n";

        LinkedList<Staff> staffList = accountRepo.getAllStaffInfo();
        Node<Staff> *staffNode = staffList.getHead();
        while (staffNode != nullptr) {
            cout << setw(8) << left << staffNode->getData().getId()
                 << setw(25) << left << staffNode->getData().getName()
                 << setw(10) << left << staffNode->getData().getGender()
                 << setw(15) << right << fixed << setprecision(0) << staffNode->getData().getSalary() << "\n";
            staffNode = staffNode->getNext();
            count++;
        }
        cout << "Total: " << count << " staff(s)\n";
        count = 0;

        // Client accounts
        cout << "\n========== CLIENT ACCOUNTS ==========\n";
        cout << setw(13) << left << "Phone" << setw(25) << left << "Name"
             << setw(10) << left << "Gender" << setw(30) << left << "Address"
             << "\n";
        cout << string(78, '-') << "\n";

        LinkedList<Client> clients = accountRepo.getAllClientInfo();
        Node<Client> *clientNode = clients.getHead();
        while (clientNode != nullptr) {
            cout << setw(13) << left << clientNode->getData().getId() // Giả sử Id là Phone
                 << setw(25) << left << clientNode->getData().getName()
                 << setw(10) << left << clientNode->getData().getGender()
                 << setw(30) << left << clientNode->getData().getAddress() << "\n";
            clientNode = clientNode->getNext();
            count++;
        }
        cout << "Total: " << count << " client(s)\n";
    }

    void addStaffAccount() {
        Menu::displayHeader("ADD STAFF ACCOUNT");

        string id, name, password, gender;
        long salary;

        cout << "Enter Staff ID (5 characters, e.g., SV001): ";
        cin >> id;
        cin.ignore();

        if (id.length() != StaffIdLength) {
            Menu::displayError("Staff ID must be exactly 5 characters!");
            return;
        }

        if (accountRepo.isValidId(id)) {
            Menu::displayError("Staff ID already exists!");
            return;
        }

        cout << "Enter Name: ";
        getline(cin, name);
        cout << "Enter Password: ";
        cin >> password;
        cout << "Enter Gender (male/female): ";
        cin >> gender;
        cout << "Enter Salary: ";
        cin >> salary;

        Staff newStaff(id, name, password, gender, salary);
        accountRepo.setStaffInfo(newStaff);

        Menu::displaySuccess("Staff account created successfully!");
    }

    void updateAccount() {
        Menu::displayHeader("UPDATE ACCOUNT");

        string id;
        cout << "Enter Account ID: ";
        cin >> id;
        cin.ignore();

        if (!accountRepo.isValidId(id)) {
            Menu::displayError("Account not found!");
            return;
        }

        int length = id.length();
        string name, password, gender;

        if (length == AdminIdLength) { // Admin
            Admin admin = accountRepo.getAdminInfo(id);
            cout << "\nCurrent Info:\n"
                 << "Name: " << admin.getName() << "\n"
                 << "Gender: " << admin.getGender() << "\n";

            cout << "\nEnter New Name (or press Enter to keep): ";
            getline(cin, name);
            if (!name.empty())
                admin.setName(name);

            cout << "Enter New Password (or press Enter to keep): ";
            getline(cin, password);
            if (!password.empty())
                admin.setPassword(password);

            cout << "Enter New Gender (or press Enter to keep): ";
            getline(cin, gender);
            if (!gender.empty())
                admin.setGender(gender);

            accountRepo.setAdminInfo(admin);

        } else if (length == StaffIdLength) { // Staff
            Staff staff = accountRepo.getStaffInfo(id);
            cout << "\nCurrent Info:\n"
                 << "Name: " << staff.getName() << "\n"
                 << "Gender: " << staff.getGender() << "\n"
                 << "Salary: " << staff.getSalary() << "\n";

            cout << "\nEnter New Name (or press Enter to keep): ";
            getline(cin, name);
            if (!name.empty())
                staff.setName(name);

            cout << "Enter New Password (or press Enter to keep): ";
            getline(cin, password);
            if (!password.empty())
                staff.setPassword(password);

            cout << "Enter New Gender (or press Enter to keep): ";
            getline(cin, gender);
            if (!gender.empty())
                staff.setGender(gender);

            string salaryStr;
            while (true) {
                cout << "Enter New Salary (or press Enter to keep): ";
                getline(cin, salaryStr);

                if (salaryStr.empty()) {
                    break;
                }
                try {
                    long number = stol(salaryStr);
                    staff.setSalary(number);
                    break;
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Input is not a number. Please try again.\n";
                } catch (const std::out_of_range &e) {
                    std::cerr << "Error: Number is too large or too small. Please try again.\n";
                }
            }
            accountRepo.setStaffInfo(staff);

        } else if (length == ClientIdLength) { // Client
            Client client = accountRepo.getClientInfo(id);
            cout << "\nCurrent Info:\n"
                 << "Name: " << client.getName() << "\n"
                 << "Gender: " << client.getGender() << "\n"
                 << "Address: " << client.getStreet() << ", " << client.getCity() << "\n";

            cout << "\nEnter New Name (or press Enter to keep): ";
            getline(cin, name);
            if (!name.empty())
                client.setName(name);

            cout << "Enter New Password (or press Enter to keep): ";
            getline(cin, password);
            if (!password.empty())
                client.setPassword(password);

            cout << "Enter New Gender (or press Enter to keep): ";
            getline(cin, gender);
            if (!gender.empty())
                client.setGender(gender);

            string street, city;
            cout << "Enter New Street (or press Enter to keep): ";
            getline(cin, street);
            if (!street.empty())
                client.setStreet(street);

            cout << "Enter New City (or press Enter to keep): ";
            getline(cin, city);
            if (!city.empty())
                client.setCity(city);

            accountRepo.setClientInfo(client);
        }

        Menu::displaySuccess("Account updated successfully!");
    }

    void deleteAccount() {
        Menu::displayHeader("DELETE ACCOUNT");

        string id;
        cout << "Enter Account ID to delete: ";
        cin >> id;

        if (!accountRepo.isValidId(id)) {
            Menu::displayError("Account not found!");
            return;
        }

        if (id == currentAdmin->getId()) {
            Menu::displayError("Cannot delete your own account!");
            return;
        }

        if (id.length() == AdminIdLength) {
            Menu::displayError("Cannot delete admin accounts!");
            return;
        }

        char confirm;
        cout << "Are you sure you want to delete this account? (y/n): ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            accountRepo.deleteAccount(id);
            Menu::displaySuccess("Account deleted successfully!");
        } else {
            Menu::displayInfo("Deletion cancelled.");
        }
    }

    void viewAccountDetails() {
        Menu::displayHeader("ACCOUNT DETAILS");

        string id;
        cout << "Enter Account ID: ";
        cin >> id;

        if (!accountRepo.isValidId(id)) {
            Menu::displayError("Account not found!");
            return;
        }

        int length = id.length();

        if (length == AdminIdLength) {
            Admin admin = accountRepo.getAdminInfo(id);
            cout << "\n========== ADMIN ACCOUNT ==========\n";
            cout << "ID: " << admin.getId() << "\n";
            cout << "Name: " << admin.getName() << "\n";
            cout << "Gender: " << admin.getGender() << "\n";
            cout << "===================================\n";

        } else if (length == StaffIdLength) {
            Staff staff = accountRepo.getStaffInfo(id);
            cout << "\n========== STAFF ACCOUNT ==========\n";
            cout << "ID: " << staff.getId() << "\n";
            cout << "Name: " << staff.getName() << "\n";
            cout << "Gender: " << staff.getGender() << "\n";
            cout << "Salary: " << fixed << setprecision(0) << staff.getSalary() << " VND\n";
            cout << "===================================\n";

        } else if (length == ClientIdLength) {
            Client client = accountRepo.getClientInfo(id);
            cout << "\n========== CLIENT ACCOUNT ==========\n";
            cout << "ID (Phone): " << client.getId() << "\n";
            cout << "Name: " << client.getName() << "\n";
            cout << "Gender: " << client.getGender() << "\n";
            cout << "Address: " << client.getAddress() << "\n";
            cout << "====================================\n";
        }
    }

    // ===== PET MANAGEMENT =====
    void managePets() {
        int choice;
        do {
            Menu::clearScreen();
            Menu::displayPetManagementMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                viewAllPets();
                break;
            case 2:
                addNewPet();
                break;
            case 3:
                updatePet();
                break;
            case 4:
                deletePet();
                break;
            case 5:
                searchPet();
                break;
            case 0:
                break;
            default:
                Menu::displayError("Invalid choice!");
            }

            if (choice != 0)
                Menu::pause();
        } while (choice != 0);
    }

    void displayPets(LinkedList<Dog> &dogList) {
        int dogCount = 0;
        cout << "\n========== DOGS ==========\n";
        cout << setw(6) << left << "ID" << setw(12) << left << "Sold"
             << setw(15) << left << "Name" << setw(20) << left << "Breed" << setw(3) << right
             << "Age" << setw(12) << right << "Price" << setw(8) << right << "Energy" << "\n";
        cout << string(80, '-') << "\n";

        Node<Dog> *dog = dogList.getHead();
        while (dog != nullptr) {
            cout << setw(6) << left << dog->getData().getId() << setw(12) << left << dog->getData().getStatus()
                 << setw(15) << left << dog->getData().getName() << setw(20) << left << dog->getData().getBreed() << setw(3)
                 << right << dog->getData().getAge() << setw(12) << right << fixed << setprecision(8)
                 << dog->getData().getPrice() << setw(8) << right << dog->getData().getEnergyLevel() << "/10\n";
            dog = dog->getNext();
            dogCount++;
        }
        cout << "Total Dogs: " << dogCount << "\n";
        cout << string(80, '-') << "\n";
    }

    void displayPets(LinkedList<Cat> &catList) {
        cout << "\n========== CATS ==========\n";
        cout << setw(6) << left << "ID" << setw(12) << left << "Sold" << setw(15)
             << left << "Name" << setw(20) << left << "Breed" << setw(3) << right
             << "Age" << setw(12) << right << "Price" << setw(8) << right << "Fur" << "\n";
        cout << string(80, '-') << "\n";

        int catCount = 0;
        Node<Cat> *cat = catList.getHead();
        while (cat != nullptr) {
            cout << setw(6) << left << cat->getData().getId() << setw(12) << left << cat->getData().getStatus()
                 << setw(15) << left << cat->getData().getName() << setw(20) << left << cat->getData().getBreed() << setw(3)
                 << right << cat->getData().getAge() << setw(12) << right << fixed << setprecision(0)
                 << cat->getData().getPrice() << setw(8) << right << cat->getData().getFurLength() << "\n";
            cat = cat->getNext();
            catCount++;
        }
        cout << "Total Cats: " << catCount << "\n";
        cout << string(80, '-') << "\n";
    }

    void viewAllPets() {
        Menu::displayHeader("ALL PETS");
        int dogCount = 0;

        // ======== DOGS ========

        LinkedList<Dog> dogList = petRepo.getAllDogInfo();
        displayPets(dogList);
        // ======== CATS ========
        LinkedList<Cat> catList = petRepo.getAllCatInfo();
        displayPets(catList);
    }

    void addNewPet() {
        Menu::displayHeader("ADD NEW PET");

        string type;
        cout << "Pet Type:\n";
        cout << "1. Dog\n";
        cout << "2. Cat\n";
        cout << "Enter choice: ";
        cin >> type;
        cin.ignore();

        string id, name, breed;
        int age;
        long price;
        while (true) {
            type = (type == "1") ? "dog" : "cat";
            id = petRepo.generatePetId(type);
            cout << "This pet's ID is: " << id;

            cout << "Enter Name: ";
            getline(cin, name);
            cout << "Enter Breed: ";
            getline(cin, breed);
            cout << "Enter Age (years): ";
            cin >> age;
            cout << "Enter Price (VND): ";
            cin >> price;
            string desc;
            cout << "Enter description: ";
            cin.ignore();
            getline(cin, desc);

            if (type == "1") {
                int energy;
                cout << "Enter Energy Level (1-10): ";
                cin >> energy;

                if (energy < 1 || energy > 10) {
                    Menu::displayError("Energy level must be between 1 and 10!");
                    return;
                }

                Dog newDog(id, name, breed, age, price, 1, energy, desc);
                petRepo.setDogInfo(newDog);
                Menu::displaySuccess("Dog added successfully!");
                cout << newDog;
            } else if (type == "2") {
                string furLength;
                cout << "Enter Fur Length (Short/Medium/Long): ";
                cin >> furLength;

                Cat newCat = Cat(id, name, breed, age, price, 1, furLength, desc);
                petRepo.setCatInfo(newCat);
                Menu::displaySuccess("Cat added successfully!");
                cout << newCat;
            } else {
                Menu::displayError("Invalid pet type!");
            }
            break;
        }
    }

    void updatePet() {
        Menu::displayHeader("UPDATE PET");

        string id;
        cout << "Enter Pet ID to update: ";
        cin >> id;
        cin.ignore();

        if (!petRepo.isValidPetId(id)) {
            Menu::displayError("Pet not found!");
            return;
        }

        if (id[0] == 'd') {
            Dog dog = petRepo.getDogInfo(id);
            cout << "\n====Current Info=====\n"
                 << dog; // operator <<

            string name, breed, ageStr, priceStr, energyStr;

            cout << "\nEnter New Name (or press Enter to keep): ";
            getline(cin, name);
            if (!name.empty())
                dog.setName(name);

            cout << "Enter New Breed (or press Enter to keep): ";
            getline(cin, breed);
            if (!breed.empty())
                dog.setBreed(breed);

            while (true) {
                cout << "Enter New Age (or press Enter to keep): ";
                getline(cin, ageStr);
                if (ageStr.empty())
                    break;
                try {
                    int age = stoi(ageStr);
                    dog.setAge(age);
                    break;
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Input is not a number. Please try again.\n";
                } catch (const std::out_of_range &e) {
                    std::cerr << "Error: Number is too large or too small. Please try again.\n";
                }
            }

            while (true) {
                cout << "Enter New Price (or press Enter to keep): ";
                getline(cin, priceStr);
                if (priceStr.empty())
                    break;
                try {
                    long price = stol(priceStr); // SỬA: Đã dùng stol
                    dog.setPrice(price);
                    break;
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Input is not a number. Please try again.\n";
                } catch (const std::out_of_range &e) {
                    std::cerr << "Error: Number is too large or too small. Please try again.\n";
                }
            }

            while (true) {
                cout << "Enter New Energy Level (or press Enter to keep): ";
                getline(cin, energyStr);
                if (energyStr.empty())
                    break;
                try {
                    int energy = stoi(energyStr);
                    dog.setEnergyLevel(energy);
                    break;
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Input is not a number. Please try again.\n";
                } catch (const std::out_of_range &e) {
                    std::cerr << "Error: Number is too large or too small. Please try again.\n";
                }
            }

            petRepo.setDogInfo(dog);
            Menu::displaySuccess("Dog updated successfully!");
            cout << dog;
        } else { // Cat
            Cat cat = petRepo.getCatInfo(id);
            cout << "\n======Current Info======\n"
                 << cat; // operator <<

            string name, breed, ageStr, priceStr, fur;

            cout << "\nEnter New Name (or press Enter to keep): ";
            getline(cin, name);
            if (!name.empty())
                cat.setName(name);

            cout << "Enter New Breed (or press Enter to keep): ";
            getline(cin, breed);
            if (!breed.empty())
                cat.setBreed(breed);

            while (true) {
                cout << "Enter New Age (or press Enter to keep): ";
                getline(cin, ageStr);
                if (ageStr.empty())
                    break;
                try {
                    int age = stoi(ageStr);
                    cat.setAge(age);
                    break;
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Input is not a number. Please try again.\n";
                } catch (const std::out_of_range &e) {
                    std::cerr << "Error: Number is too large or too small. Please try again.\n";
                }
            }

            while (true) {
                cout << "Enter New Price (or press Enter to keep): ";
                getline(cin, priceStr);
                if (priceStr.empty())
                    break;
                try {
                    long price = stol(priceStr); // SỬA: Đã dùng stol
                    cat.setPrice(price);
                    break;
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Input is not a number. Please try again.\n";
                } catch (const std::out_of_range &e) {
                    std::cerr << "Error: Number is too large or too small. Please try again.\n";
                }
            }

            cout << "Enter New Fur Length (or press Enter to keep): ";
            getline(cin, fur);
            if (!fur.empty())
                cat.setFurLength(fur);

            petRepo.setCatInfo(cat);
            Menu::displaySuccess("Cat updated successfully!");
            cout << cat;
        }
    }

    void deletePet() {
        Menu::displayHeader("DELETE PET");

        string id;
        cout << "Enter Pet ID to delete: ";
        cin >> id;

        if (!petRepo.isValidPetId(id)) {
            Menu::displayError("Pet not found!");
            return;
        }

        cout << "==== PET NEEDED DELETE INFO ====\n";
        if (id[0] == 'd') {
            Dog dog = petRepo.getDogInfo(id);
            cout << dog;
        } else if (id[0] == 'c') {
            Cat cat = petRepo.getCatInfo(id);
            cout << cat;
        }
        char confirm;
        cout << "Are you sure you want to delete this pet? (y/n): ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            petRepo.deletePet(id);
            Menu::displaySuccess("Pet deleted successfully!");
        } else {
            Menu::displayInfo("Deletion cancelled.");
        }
    }

    void searchPet() {
        Menu::displayHeader("SEARCH PET");

        cout << "Search by:\n";
        cout << "1. Pet ID\n";
        cout << "2. Name\n";
        cout << "3. Breed\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string id;
            cout << "Enter Pet ID: ";
            cin >> id;

            if (!petRepo.isValidPetId(id)) {
                Menu::displayError("Pet not found!");
                return;
            }

            if (id[0] == 'd') {
                Dog dog = petRepo.getDogInfo(id);
                cout << "\n=== DOG FOUND ===\n";
                cout << dog;
            } else {
                Cat cat = petRepo.getCatInfo(id);
                cout << "\n=== CAT FOUND ===\n";
                cout << cat;
            }

        } else if (choice == 2 || choice == 3) {
            string keyword;
            cout << "Enter search keyword: ";
            getline(cin, keyword);

            string searchType = (choice == 2) ? "name" : "breed";

            LinkedList<Dog> dogs = petRepo.searchDog(searchType, keyword);
            LinkedList<Cat> cats = petRepo.searchCat(searchType, keyword);

            bool found = !dogs.isEmpty() || !cats.isEmpty();
            if (!found) {
                Menu::displayInfo("No pets found matching your search.");
                return;
            }

            cout << "\n=== SEARCH RESULTS ===\n";
            displayPets(dogs);
            displayPets(cats);

        } else {
            Menu::displayError("Invalid choice!");
        }
    }

    // ===== SPA SERVICE MANAGEMENT =====
    void manageSpaServices() {
        int choice;
        do {
            Menu::clearScreen();
            Menu::displaySpaManagementMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                spaService.displayAllSpaServices();
                break;
            case 2:
                addSpaService();
                break;
            case 3:
                updateSpaService();
                break;
            case 4:
                deleteSpaService();
                break;
            case 0:
                break;
            default:
                Menu::displayError("Invalid choice!");
            }

            if (choice != 0)
                Menu::pause();
        } while (choice != 0);
    }

    void addSpaService() {
        Menu::displayHeader("ADD SPA SERVICE");

        string id, name, desc;
        long price;
        int duration;

        cout << "Enter Service ID (e.g., 011): ";
        id = "SP" + id;
        cin >> id;
        cin.ignore();
        cout << "Enter Service Name: ";
        getline(cin, name);
        cout << "Enter Description: ";
        getline(cin, desc);
        cout << "Enter Price (VND): ";
        cin >> price;
        cout << "Enter Duration (minutes): ";
        cin >> duration;

        spaService.addSpaService(id, name, desc, price, duration);
    }

    void updateSpaService() {
        Menu::displayHeader("UPDATE SPA SERVICE");

        string id, name, desc;
        long price;
        int duration;

        cout << "Enter Service ID to update: ";
        cin >> id;
        cin.ignore();
        cout << "Enter New Service Name: ";
        getline(cin, name);
        cout << "Enter New Description: ";
        getline(cin, desc);
        cout << "Enter New Price: ";
        cin >> price;
        cout << "Enter New Duration (minutes): ";
        cin >> duration;

        spaService.updateSpaService(id, name, desc, price, duration);
    }

    void deleteSpaService() {
        Menu::displayHeader("DELETE SPA SERVICE");

        string id;
        cout << "Enter Service ID to delete: ";
        cin >> id;

        char confirm;
        cout << "Are you sure? (y/n): ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            spaService.deleteSpaService(id);
        }
    }

    // ===== BOOKING MANAGEMENT =====
    void manageBookings() {
        int choice;
        do {
            Menu::clearScreen();
            Menu::displayBookingManagementMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                spaService.viewAllBookings();
                break;
            case 2:
                viewBookingsByDate();
                break;
            case 3:
                confirmBooking();
                break;
            case 4:
                completeBooking();
                break;
            case 5:
                cancelBooking();
                break;
            case 6:
                deleteBooking();
                break;
            case 0:
                break;
            default:
                Menu::displayError("Invalid choice!");
            }

            if (choice != 0)
                Menu::pause();
        } while (choice != 0);
    }

    void viewBookingsByDate() {
        string date;
        cout << "Enter Date (dd/mm/yyyy): ";
        cin >> date;
        spaService.viewBookingsByDate(date);
    }

    void confirmBooking() {
        string bookingId;
        cout << "Enter Booking ID to confirm: ";
        cin >> bookingId;
        spaService.confirmBooking(bookingId);
    }

    void completeBooking() {
        string bookingId;
        cout << "Enter Booking ID to complete: ";
        cin >> bookingId;
        spaService.completeBooking(bookingId);
    }

    void cancelBooking() {
        string bookingId;
        cout << "Enter Booking ID to cancel: ";
        cin >> bookingId;
        spaService.cancelBooking(bookingId);
    }

    void deleteBooking() {
        string bookingId;
        cout << "Enter Booking ID to delete: ";
        cin >> bookingId;

        char confirm;
        cout << "Are you sure? (y/n): ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            spaService.deleteBookingPermanent(bookingId);
        }
    }

    // ===== STATISTICS =====
    void viewStatistics() {
        int choice;
        do {
            system("cls");
            Menu::displayHeader("SYSTEM STATISTICS");
            cout << "1. Account Statistics\n";
            cout << "2. Pet Inventory Statistics\n";
            cout << "3. Spa Service Statistics\n";
            cout << "0. Back\n";
            cout << "==============================\n";
            cout << "Enter your choice: ";
            cin >> choice;
            if (cin.fail()) { // Nếu người dùng nhập chữ
                cin.clear();  // 1. Sửa trạng thái lỗi
                choice = -1;  // 2. Gán số -1 (để chạy default)
            }
            // 3. Luôn dọn dẹp phím Enter và chữ thừa
            cin.ignore();
            switch (choice) {
            case 1: {
                Menu::displayHeader("ACCOUNT STATISTICS");

                AccountStats counts = accountRepo.countAccount();
                int totalAll = counts.totalAdmin + counts.totalStaff + counts.totalClient;

                cout << "\n========== ACCOUNT TOTALS ==========\n";
                cout << "Admin Accounts:   " << counts.totalAdmin << "\n";
                cout << "Staff Accounts:   " << counts.totalStaff << "\n";
                cout << "Client Accounts:  " << counts.totalClient << "\n\n";

                cout << "Overall:\n";
                cout << "  Total Accounts: " << totalAll << "\n";
                cout << "======================================\n";
                break;
            }
            case 2: {
                PetStats stats = petRepo.countPet();

                cout << "\n========== PET SALES STATISTICS ==========\n";
                cout << "Dogs:\n";
                cout << "  Total: " << stats.totalDogs << "\n";
                cout << "  Available: " << stats.availableDogs << "\n";
                cout << "  Sold: " << stats.soldDogs << "\n\n";

                cout << "Cats:\n";
                cout << "  Total: " << stats.totalCats << "\n";
                cout << "  Available: " << stats.availableCats << "\n";
                cout << "  Sold: " << stats.soldCats << "\n\n";

                cout << "Overall:\n";
                cout << "  Total Pets: " << (stats.totalDogs + stats.totalCats) << "\n";
                cout << "  Total Available: " << (stats.availableDogs + stats.availableCats) << "\n";
                cout << "  Total Sold: " << (stats.soldDogs + stats.soldCats) << "\n";
                cout << "==========================================\n";
                break;
            }
            case 3: {
                cout << "\n========== SPA SERVICES ==========\n";
                spaService.showStatistics();
                break;
            }
            case 0:
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
            }

            if (choice != 0) {
                cout << "\nPress Enter to continue...";
                cin.ignore();
            }

        } while (choice != 0);
    }

    // =============== QUẢN LÝ DOANH THHU ==========
    // void calculateRevenueStats() {
    //     // ===== SPA REVENUE =====
    //     long spaRevenue = 0;
    //     LinkedList<Booking> allBookings = bookingRepo.getAllBookings();
    //     Node<Booking> *current = allBookings.getHead();

    //     while (current != nullptr) {
    //         const Booking &booking = current->getData();
    //         if (booking.getStatus() == "Completed") {
    //             string serviceId = booking.getServiceId();
    //             long price = serviceRepo.getServicePrice(serviceId);
    //             spaRevenue += price;
    //         }
    //         current = current->getNext();
    //     }

    //     // ===== PET SALES REVENUE =====
    //     long petRevenue = 0;
    //     int soldPetsCount = 0;

    //     // --- Dogs ---
    //     ifstream dogFile("../data/Dog.txt");
    //     if (dogFile.is_open()) {
    //         string line;
    //         while (getline(dogFile, line)) {
    //             if (line.empty())
    //                 continue;

    //             stringstream ss(line);
    //             string id, status, name, breed, age, price, energy;

    //             getline(ss, id, '|');
    //             getline(ss, status, '|');
    //             getline(ss, name, '|');
    //             getline(ss, breed, '|');
    //             getline(ss, age, '|');
    //             getline(ss, price, '|');
    //             getline(ss, energy, '|');

    //             if (status == "0") { // sold
    //                 try {
    //                     petRevenue += stof(price);
    //                     soldPetsCount++;
    //                 } catch (...) {
    //                     cerr << "Warning: Invalid price for dog " << id << "\n";
    //                 }
    //             }
    //         }
    //         dogFile.close();
    //     } else {
    //         cerr << "Error: Cannot open Dog.txt\n";
    //     }

    //     // --- Cats ---
    //     ifstream catFile("../data/Cat.txt");
    //     if (catFile.is_open()) {
    //         string line;
    //         while (getline(catFile, line)) {
    //             if (line.empty())
    //                 continue;

    //             stringstream ss(line);
    //             string id, status, name, breed, age, price, fur;

    //             getline(ss, id, '|');
    //             getline(ss, status, '|'); // nếu chưa có status thì vẫn đọc rỗng
    //             getline(ss, name, '|');
    //             getline(ss, breed, '|');
    //             getline(ss, age, '|');
    //             getline(ss, price, '|');
    //             getline(ss, fur, '|');

    //             if (status == "0") { // sold
    //                 try {
    //                     petRevenue += stof(price);
    //                     soldPetsCount++;
    //                 } catch (...) {
    //                     cerr << "Warning: Invalid price for cat " << id << "\n";
    //                 }
    //             }
    //         }
    //         catFile.close();
    //     } else {
    //         cerr << "Error: Cannot open Cat.txt\n";
    //     }

    //     // ===== TOTAL =====
    //     long totalRevenue = spaRevenue + petRevenue;

    //     cout << "\n========== REVENUE REPORT ==========\n";
    //     cout << "Pet Sales:\n";
    //     cout << "  Pets Sold: " << soldPetsCount << "\n";
    //     cout << "  Revenue: " << fixed << setprecision(0) << petRevenue
    //          << " VND\n\n";

    //     cout << "Spa Services:\n";
    //     cout << "  Revenue: " << spaRevenue << " VND\n\n";

    //     cout << "----------------------------------------\n";
    //     cout << "TOTAL REVENUE: " << totalRevenue << " VND\n";
    //     cout << "----------------------------------------\n";

    //     // Breakdown percentages
    //     if (totalRevenue > 0) {
    //         cout << "\nRevenue Breakdown:\n";
    //         cout << "  Pet Sales: " << fixed << setprecision(1)
    //              << (petRevenue / totalRevenue * 100) << "%\n";
    //         cout << "  Spa Services: " << (spaRevenue / totalRevenue * 100) << "%\n";
    //     }

    //     cout << "========================================\n";
    // }

public:
    AdminController(Admin *admin) : currentAdmin(admin) {}

    void run() {
        int choice;
        do {
            Menu::clearScreen();
            Menu::displayAdminMenu(currentAdmin->getName());
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                manageAccounts();
                break;
            case 2:
                managePets();
                break;
            case 3:
                manageSpaServices();
                break;
            case 4:
                manageBookings();
                break;
            case 5:
                viewStatistics();
                Menu::pause();
                break;
            case 0:
                Menu::displayInfo("Logging out...");
                break;
            default:
                Menu::displayError("Invalid choice!");
                Menu::pause();
            }
        } while (choice != 0);
    }
};