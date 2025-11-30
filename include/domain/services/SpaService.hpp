
#pragma once
#include "domain/entities/Booking.hpp"
#include "domain/entities/Service.hpp"
#include "ds/LinkedList.hpp"
#include "infra/repositories/BookingRepoFile.hpp"
#include "infra/repositories/ServiceRepoFile.hpp"
#include <iomanip>
#include <iostream>

using namespace std;

class SpaService {
private:
  ServiceRepository serviceRepo;
  BookingRepository bookingRepo;

  void displayServiceTableHeader() {
    cout << "\n";
    cout << setw(8) << left << "ID" << setw(28) << left << "Service Name"
         << setw(15) << right << "Price (VND)" << setw(15) << right
         << "Duration (min)"
         << "\n";
    cout << string(66, '-') << "\n";
  }
  void displayServiceRow(const Service &service) {
    cout << setw(8) << left << service.getId() << setw(28) << left
         << service.getName() << setw(15) << right << fixed << setprecision(0)
         << service.getPrice() << setw(15) << right << service.getDuration()
         << "\n";
  }
  void displayBookingTableHeader() {
    cout << "\n";
    cout << setw(8) << left << "ID" << setw(13) << left << "Client ID"
         << setw(8) << left << "Spa ID" << setw(12) << left << "Date" << setw(8)
         << left << "Time" << setw(12) << left << "Status"
         << "\n";
    cout << string(61, '-') << "\n";
  }
  void displayBookingRow(const Booking &booking) {
    cout << setw(8) << left << booking.getBookingId() << setw(13) << left
         << booking.getClientId() << setw(8) << left << booking.getServiceId()
         << setw(12) << left << booking.getDate() << setw(8) << left
         << booking.getTime() << setw(12) << left << booking.getStatus()
         << "\n";
  }

public:
  SpaService() {}
  void displayAllSpaServices() {
    LinkedList<Service> services = serviceRepo.getAllServices();

    if (services.isEmpty()) {
      cout << "No spa services available.\n";
      return;
    }

    cout << "\n========== SPA SERVICES MENU ==========\n";
    displayServiceTableHeader();

    Node<Service> *current = services.getHead();
    while (current != nullptr) {
      displayServiceRow(current->getData());
      current = current->getNext();
    }
    cout << "\nTotal: " << services.getSize() << " spa services\n";
    cout << "=======================================\n";
  }
  void displaySpaServiceDetails(const string &serviceId) {
    if (!serviceRepo.isValidServiceId(serviceId)) {
      cout << "Error: Spa service not found: " << serviceId << "\n";
      return;
    }

    Service service = serviceRepo.getServiceInfo(serviceId);
    cout << "\n";
    cout << service;
  }
  bool createBooking(const string &clientId, const string &petId,
                     const string &serviceId, const string &date,
                     const string &time) {
    if (!serviceRepo.isValidServiceId(serviceId)) {
      cout << "Error: Invalid spa service ID: " << serviceId << "\n";
      return false;
    }

    if (!bookingRepo.isTimeSlotAvailable(date, time)) {
      cout << "Error: Time slot " << date << " " << time
           << " is already booked!\n";
      return false;
    }

    string bookingId = bookingRepo.generateBookingId();

    Booking newBooking(bookingId, clientId, petId, serviceId, date, time,
                       "Pending");
    bookingRepo.createBooking(newBooking);

    Service service = serviceRepo.getServiceInfo(serviceId);

    cout << "\n===== BOOKING CREATED SUCCESSFULLY =====\n";
    cout << "Booking ID: " << bookingId << "\n";
    cout << "Service: " << service.getName() << "\n";
    cout << "Date: " << date << "\n";
    cout << "Time: " << time << "\n";
    cout << "Duration: " << service.getDuration() << " minutes\n";
    cout << "Price: " << service.getPrice() << " VND\n";
    cout << "Status: Pending (waiting for confirmation)\n";
    cout << "=========================================\n";

    return true;
  }

  bool cancelBooking(const string &bookingId) {
    if (!bookingRepo.isValidBookingId(bookingId)) {
      cout << "Error: Booking not found: " << bookingId << "\n";
      return false;
    }

    Booking booking = bookingRepo.getBookingInfo(bookingId);

    if (booking.getStatus() == "Completed") {
      cout << "Error: Cannot cancel completed booking!\n";
      return false;
    }

    if (booking.getStatus() == "Cancelled") {
      cout << "Error: Booking already cancelled!\n";
      return false;
    }

    bookingRepo.updateStatus(bookingId, "Cancelled");

    cout << "\n===== BOOKING CANCELLED =====\n";
    cout << "Booking ID: " << bookingId << "\n";
    cout << "Date: " << booking.getDate() << "\n";
    cout << "Time: " << booking.getTime() << "\n";
    cout << "Status: Cancelled\n";
    cout << "=============================\n";

    return true;
  }
  bool deleteBookingPermanent(const string &bookingId) {
    if (!bookingRepo.isValidBookingId(bookingId)) {
      cout << "Error: Booking not found: " << bookingId << "\n";
      return false;
    }

    bookingRepo.deleteBooking(bookingId);
    cout << "Booking " << bookingId << " has been permanently deleted.\n";
    return true;
  }
  void viewMyBookings(const string &clientId) {
    LinkedList<Booking> bookings = bookingRepo.getBookingsByClient(clientId);

    if (bookings.isEmpty()) {
      cout << "You don't have any bookings yet.\n";
      return;
    }

    cout << "\n========== MY SPA BOOKINGS ==========\n";
    displayBookingTableHeader();

    Node<Booking> *current = bookings.getHead();
    while (current != nullptr) {
      displayBookingRow(current->getData());
      current = current->getNext();
    }

    cout << "\nTotal: " << bookings.getSize() << " bookings\n";
    cout << "=====================================\n";
  }
  void viewBookingDetails(const string &bookingId) {
    if (!bookingRepo.isValidBookingId(bookingId)) {
      cout << "Error: Booking not found: " << bookingId << "\n";
      return;
    }

    Booking booking = bookingRepo.getBookingInfo(bookingId);
    Service service = serviceRepo.getServiceInfo(booking.getServiceId());

    cout << "\n===== BOOKING DETAILS =====\n";
    cout << "Booking ID: " << booking.getBookingId() << "\n";
    cout << "Client ID: " << booking.getClientId() << "\n";
    cout << "Pet ID: " << booking.getPetId() << "\n";
    cout << "\nService Information:\n";
    cout << "  ID: " << service.getId() << "\n";
    cout << "  Name: " << service.getName() << "\n";
    cout << "  Description: " << service.getDescription() << "\n";
    cout << "  Price: " << service.getPrice() << " VND\n";
    cout << "  Duration: " << service.getDuration() << " minutes\n";
    cout << "\nSchedule:\n";
    cout << "  Date: " << booking.getDate() << "\n";
    cout << "  Time: " << booking.getTime() << "\n";
    cout << "  Status: " << booking.getStatus() << "\n";
    cout << "===========================\n";
  }
  void viewBookingsByDate(const string &date) {
    LinkedList<Booking> bookings = bookingRepo.getBookingsByDate(date);

    if (bookings.isEmpty()) {
      cout << "No bookings found on " << date << "\n";
      return;
    }

    cout << "\n========== BOOKINGS ON " << date << " ==========\n";
    displayBookingTableHeader();

    Node<Booking> *current = bookings.getHead();
    while (current != nullptr) {
      displayBookingRow(current->getData());
      current = current->getNext();
    }
    cout << "\nTotal: " << bookings.getSize() << " bookings\n";
    cout << "================================================\n";
  }
  void viewAllBookings() {
    LinkedList<Booking> bookings = bookingRepo.getAllBookings();

    if (bookings.isEmpty()) {
      cout << "No bookings in the system.\n";
      return;
    }

    cout << "\n========== ALL BOOKINGS ==========\n";
    displayBookingTableHeader();

    Node<Booking> *current = bookings.getHead();
    while (current != nullptr) {
      displayBookingRow(current->getData());
      current = current->getNext();
    }
    cout << "\nTotal: " << bookings.getSize() << " bookings\n";
    cout << "==================================\n";
  }
  bool addSpaService(const string &id, const string &name, const string &desc,
                     long price, int duration) {
    if (serviceRepo.isValidServiceId(id)) {
      cout << "Error: Service ID already exists: " << id << "\n";
      return false;
    }

    Service newService(id, name, desc, price, duration);
    serviceRepo.setServiceInfo(newService);
    cout << "Spa service added successfully: " << name << "\n";
    return true;
  }
  bool updateSpaService(const string &id, const string &name,
                        const string &desc, long price, int duration) {
    if (!serviceRepo.isValidServiceId(id)) {
      cout << "Error: Service not found: " << id << "\n";
      return false;
    }

    Service service(id, name, desc, price, duration);
    serviceRepo.setServiceInfo(service);
    cout << "Service updated successfully: " << name << "\n";
    return true;
  }
  bool deleteSpaService(const string &id) {
    if (!serviceRepo.isValidServiceId(id)) {
      cout << "Error: Service not found: " << id << "\n";
      return false;
    }

    serviceRepo.deleteService(id);
    cout << "Service deleted: " << id << "\n";
    return true;
  }
  bool confirmBooking(const string &bookingId) {
    if (!bookingRepo.isValidBookingId(bookingId)) {
      cout << "Error: Booking not found: " << bookingId << "\n";
      return false;
    }

    bookingRepo.updateStatus(bookingId, "Confirmed");
    cout << "Booking " << bookingId << " has been confirmed.\n";
    return true;
  }
  bool completeBooking(const string &bookingId) {
    if (!bookingRepo.isValidBookingId(bookingId)) {
      cout << "Error: Booking not found: " << bookingId << "\n";
      return false;
    }

    bookingRepo.updateStatus(bookingId, "Completed");
    cout << "Booking " << bookingId << " has been completed.\n";
    return true;
  }
  void showStatistics() {
    int totalServices = serviceRepo.countServices();
    int totalBookings = bookingRepo.countBookings();
    int pendingBookings = bookingRepo.countBookingsByStatus("Pending");
    int confirmedBookings = bookingRepo.countBookingsByStatus("Confirmed");
    int completedBookings = bookingRepo.countBookingsByStatus("Completed");
    int cancelledBookings = bookingRepo.countBookingsByStatus("Cancelled");

    cout << "\n========== SPA STATISTICS ==========\n";
    cout << "Services:\n";
    cout << "  Total Services: " << totalServices << "\n\n";

    cout << "Bookings:\n";
    cout << "  Total Bookings: " << totalBookings << "\n";
    cout << "  Pending: " << pendingBookings << "\n";
    cout << "  Confirmed: " << confirmedBookings << "\n";
    cout << "  Completed: " << completedBookings << "\n";
    cout << "  Cancelled: " << cancelledBookings << "\n";
    cout << "====================================\n";
  }
};