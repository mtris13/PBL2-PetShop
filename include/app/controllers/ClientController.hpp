/* ClientController.hpp - Logic xử lý cho Client (Updated) */

#pragma once
#include "app/Menu.hpp"
#include "domain/entities/Bill.hpp"
#include "domain/entities/Client.hpp"
#include "domain/services/SpaService.hpp"
#include "ds/LinkedList.hpp"
#include "infra/repositories/AccountRepoFile.hpp"
#include "infra/repositories/BillRepoFile.hpp"
#include "infra/repositories/CartRepoFile.hpp"
#include "infra/repositories/PetRepoFile.hpp"
#include <iomanip>
#include <iostream>

using namespace std;

class ClientController {
private:
    Client *currentClient;
    SpaService spaService;
    PetRepository petRepo;
    CartRepository cartRepo;
    BillRepository billRepo;

    void viewPetsForSale() {
        system("cls");
        Menu::displayHeader("PETS FOR SALE");
        cout << "\n1. View Dogs\n";
        cout << "2. View Cats\n";
        cout << "0. Back\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            displayDogsForSale();
        } else if (choice == 2) {
            displayCatsForSale();
        }
        Menu::displayHeader("ADD PET TO CART");
        string petId;
        cout << "Enter Pet ID (or '0' to cancel): ";
        cin >> petId;
        cin.ignore();

        if (petId == "0")
            return;

        if (!petRepo.isValidPetId(petId)) {
            Menu::displayError("Invalid Pet ID!");
            return;
        }

        if (!petRepo.isAvailablePet(petId)) {
            Menu::displayError("This pet is not available!");
            return;
        }

        // Lấy thông tin pet
        if (petId[0] == 'd') {
            Dog dog = petRepo.getDogInfo(petId);
            cartRepo.addToCart(currentClient->getId(), dog.getId(), dog.getName(),
                               dog.getPrice());
            Menu::displaySuccess("Dog added to cart successfully!");
        } else {
            Cat cat = petRepo.getCatInfo(petId);
            cartRepo.addToCart(currentClient->getId(), cat.getId(), cat.getName(),
                               cat.getPrice());
            Menu::displaySuccess("Cat added to cart successfully!");
        }
    }

    void displayDogsForSale() {
        Menu::displayHeader("DOGS FOR SALE");
        cout << "\n========== DOGS FOR SALE ==========\n";
        cout << left << setw(6) << "ID" << setw(15) << "Name" << setw(20) << "Breed"
             << right << setw(6) << "Age" << setw(14) << "Price" << setw(10)
             << "Energy" << setw(12) << "Status"
             << "\n";
        cout << string(83, '-') << "\n";
        // print dog information (that avaiable)
        LinkedList<Dog> dog = petRepo.getAllDogInfoAvailable();
        Node<Dog> *current = dog.getHead();
        while (current != nullptr) {
            cout << left << setw(6) << current->getData().getId() << setw(15) << current->getData().getName() << setw(20) << current->getData().getBreed()
                 << right << setw(6) << current->getData().getAge() << setw(14) << fixed << setprecision(0)
                 << current->getData().getPrice() << setw(7) << current->getData().getEnergyLevel() << "/10" << setw(12)
                 << current->getData().getStatus() << "\n";
            current = current->getNext();
        }
        cout << string(83, '-') << "\n";
    }

    void displayCatsForSale() {
        Menu::displayHeader("CATS FOR SALE");

        ifstream file("../data/Cat.txt");
        if (!file.is_open()) {
            Menu::displayError("Cannot open Cat.txt");
            return;
        }

        cout << "\n========== CATS FOR SALE ==========\n";
        cout << left << setw(6) << "ID" << setw(15) << "Name" << setw(20) << "Breed"
             << right << setw(6) << "Age" << setw(14) << "Price" << setw(15) << left
             << "        Fur Length" << right << setw(12) << "Status"
             << "\n";
        cout << string(93, '-') << "\n";

        string line;
        int count = 0;
        while (getline(file, line)) {
            if (line.empty())
                continue;

            stringstream ss(line);
            string id, status, name, breed, age, price, fur;
            getline(ss, id, '|');
            getline(ss, status, '|');
            getline(ss, name, '|');
            getline(ss, breed, '|');
            getline(ss, age, '|');
            getline(ss, price, '|');
            getline(ss, fur, '|');

            string statusText = (status == "1") ? "Available" : "Sold";

            cout << left << setw(6) << id << setw(15) << name << setw(20) << breed
                 << right << setw(6) << age << setw(14) << fixed << setprecision(0)
                 << stof(price) << setw(12) << left << "" << fur << right << setw(15)
                 << statusText << "\n";

            count++;
        }

        cout << string(93, '-') << "\n";
        file.close();
    }

    void addToCart() {
        Menu::displayHeader("ADD PET TO CART");

        string petId;
        cout << "Enter Pet ID (or '0' to cancel): ";
        cin >> petId;
        cin.ignore();

        if (petId == "0")
            return;

        if (!petRepo.isValidPetId(petId)) {
            Menu::displayError("Invalid Pet ID!");
            return;
        }

        if (!petRepo.isAvailablePet(petId)) {
            Menu::displayError("This pet is not available!");
            return;
        }

        // Lấy thông tin pet
        if (petId[0] == 'd') {
            Dog dog = petRepo.getDogInfo(petId);
            cartRepo.addToCart(currentClient->getId(), dog.getId(), dog.getName(),
                               dog.getPrice());
            Menu::displaySuccess("Dog added to cart successfully!");
        } else {
            Cat cat = petRepo.getCatInfo(petId);
            cartRepo.addToCart(currentClient->getId(), cat.getId(), cat.getName(),
                               cat.getPrice());
            Menu::displaySuccess("Cat added to cart successfully!");
        }
    }

    void viewCart() {
        system("cls");
        Menu::displayHeader("MY CART");
        cartRepo.displayCart(currentClient->getId());
        if (!cartRepo.isCartEmpty(currentClient->getId())) {
            cout << "\n1. Checkout\n";
            cout << "2. Remove Item\n";
            cout << "0. Back\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;
            cin.ignore();

            if (choice == 1) {
                checkout();
            } else if (choice == 2) {
                removeFromCart();
            }
        }
    }

    void removeFromCart() {
        string petId;
        cout << "Enter Pet ID to remove: ";
        cin >> petId;
        cin.ignore();

        cartRepo.removeFromCart(currentClient->getId(), petId);
        Menu::displaySuccess("Item removed from cart!");
    }

    void checkout() {
        Menu::displayHeader("CHECKOUT");

        LinkedList<CartItem> cartItems = cartRepo.getCartItems(currentClient->getId());

        if (cartItems.isEmpty()) {
            Menu::displayError("Cart is empty!");
            return;
        }
        system("cls");

        // Hiển thị giỏ hàng
        cartRepo.displayCart(currentClient->getId());

        cout << "\nConfirm purchase? (y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore();

        if (confirm != 'y' && confirm != 'Y') {
            cout << "Purchase cancelled.\n";
            return;
        }

        // Tạo bill items
        LinkedList<BillItem> billItems;
        Node<CartItem> *item = cartItems.getHead();
        while (item != nullptr) {
            billItems.pushBack(BillItem(item->getData().getPetId(), item->getData().getPetName(), "Pet", item->getData().getPrice()));
            // Đánh dấu pet là đã bán
            petRepo.setStatusUnavailable(item->getData().getPetId());
            item = item->getNext();
        }

        // Tạo hóa đơn
        Bill bill = billRepo.createBillFromCart(
            currentClient->getId(), currentClient->getName(), billItems);

        // Lưu hóa đơn
        billRepo.saveBill(bill);
        system("cls");

        // Hiển thị hóa đơn
        bill.display();

        // Xóa giỏ hàng
        cartRepo.clearCart(currentClient->getId());
        Menu::displaySuccess("Purchase completed successfully!");
        cout << "Bill ID: " << bill.getBillId() << "\n";
    }

    void viewSpaServices() {
        Menu::clearScreen();
        spaService.displayAllSpaServices();
    }

    void bookSpaService() {
        system("cls");
        Menu::displayHeader("BOOK SPA SERVICE");

        // Show available services first
        spaService.displayAllSpaServices();

        cout << "\n";
        string serviceId, date, time;

        cout << "Enter Service ID (or '0' to cancel): ";
        cin >> serviceId;
        if (serviceId == "0")
            return;
        cout << "Enter Date (dd/mm/yyyy): ";
        cin >> date;

        cout << "Enter Time (hh:mm): ";
        cin >> time;

        spaService.createBooking(currentClient->getId(), "CUSTOMER_PET", serviceId,
                                 date, time);
    }

    void viewMyBookings() {
        Menu::clearScreen();
        Menu::displayHeader("MY BOOKINGS");
        spaService.viewMyBookings(currentClient->getId());
    }

    void cancelMyBooking() {
        system("cls");
        Menu::displayHeader("CANCEL BOOKING");

        // Show my bookings first
        spaService.viewMyBookings(currentClient->getId());

        cout << "\n";
        string bookingId;
        cout << "Enter Booking ID to cancel: ";
        cin >> bookingId;

        spaService.cancelBooking(bookingId);
    }

    void viewMyProfile() {
        system("cls");

        Menu::displayHeader("MY PROFILE");

        cout << "\n=== CLIENT INFORMATION ===\n";
        cout << "ID: " << currentClient->getId() << "\n";
        cout << "Name: " << currentClient->getName() << "\n";
        cout << "Gender: " << currentClient->getGender() << "\n";
        cout << "Address: " << currentClient->getStreet() << ", "
             << currentClient->getCity() << "\n";
        cout << "==========================\n";

        cout << "\nDo you want to update your profile? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();

        if (choice == 'y' || choice == 'Y') {
            updateProfile();
        }
    }

    void updateProfile() {
        Menu::displayHeader("UPDATE PROFILE");

        string name, password, gender, street, city;

        cout << "Enter New Name (or press Enter to keep current): ";
        getline(cin, name);
        if (name.empty())
            name = currentClient->getName();

        cout << "Enter New Password (or press Enter to keep current): ";
        getline(cin, password);
        if (password.empty())
            password = currentClient->getPassword();

        cout << "Enter New Gender (male/female, or press Enter to keep current): ";
        getline(cin, gender);
        if (gender.empty())
            gender = currentClient->getGender();

        cout << "Enter New Street (or press Enter to keep current): ";
        getline(cin, street);
        if (street.empty())
            street = currentClient->getStreet();

        cout << "Enter New City (or press Enter to keep current): ";
        getline(cin, city);
        if (city.empty())
            city = currentClient->getCity();

        // Update client object
        currentClient->setName(name);
        currentClient->setPassword(password);
        currentClient->setGender(gender);
        currentClient->setStreet(street);
        currentClient->setCity(city);

        // Save to file
        AccountRepository accountRepo;
        accountRepo.setClientInfo(*currentClient);

        Menu::displaySuccess("Profile updated successfully!");
    }

public:
    ClientController(Client *client) : currentClient(client) {}

    void run() {
        int choice;
        do {
            Menu::clearScreen();
            cout << "\n========================================\n";
            cout << "  CLIENT PANEL - " << currentClient->getName() << "\n";
            cout << "========================================\n";
            cout << "1. View Pets for Sale\n";
            // cout << "2. Add Pet to Cart\n";
            cout << "2. View My Cart\n";
            // cout << "3. View Spa Services\n";
            cout << "3. Book Spa Service\n";
            cout << "4. My Bookings\n";
            cout << "5. Cancel Booking\n";
            cout << "6. My Profile\n";
            cout << "0. Logout\n";
            cout << "========================================\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                viewPetsForSale();
                Menu::pause();
                break;
            // case 2:
            //   addToCart();
            //   Menu::pause();
            //   break;
            case 2:
                viewCart();
                Menu::pause();
                break;
            // case 3:
            //   viewSpaServices();
            //   Menu::pause();
            //   break;
            case 3:
                bookSpaService();
                Menu::pause();
                break;
            case 4:
                viewMyBookings();
                Menu::pause();
                break;
            case 5:
                cancelMyBooking();
                Menu::pause();
                break;
            case 6:
                viewMyProfile();
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