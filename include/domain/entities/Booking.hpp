#pragma once
#include <iostream>
#include <string>
using namespace std;

class Booking {
private:
    string bookingId; // Mã đặt lịch (vd: BK001)
    string clientId;  // ID khách hàng (số điện thoại)
    string petId;     // ID thú cưng
    string serviceId; // ID dịch vụ spa
    string date;      // Ngày đặt (dd/mm/yyyy)
    string time;      // Giờ đặt (hh:mm)
    string status;    // Trạng thái: "Pending", "Confirmed", "Completed", "Cancelled"

public:
    // Constructor
    Booking()
        : bookingId(""), clientId(""), petId(""), serviceId(""), date(""),
          time(""), status("Pending") {}

    Booking(const string &bookingId, const string &clientId, const string &petId,
            const string &serviceId, const string &date, const string &time,
            const string &status = "Pending")
        : bookingId(bookingId), clientId(clientId), petId(petId),
          serviceId(serviceId), date(date), time(time), status(status) {}

    // Getters
    string getBookingId() const { return bookingId; }
    string getClientId() const { return clientId; }
    string getPetId() const { return petId; }
    string getServiceId() const { return serviceId; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    string getStatus() const { return status; }

    // Setters
    void setBookingId(const string &id) { bookingId = id; }
    void setClientId(const string &id) { clientId = id; }
    void setPetId(const string &id) { petId = id; }
    void setServiceId(const string &id) { serviceId = id; }
    void setDate(const string &d) { date = d; }
    void setTime(const string &t) { time = t; }
    void setStatus(const string &s) { status = s; }

    // Display info
    // void showInfo() const {
    //     cout << "=== BOOKING INFO ===\n";
    //     cout << "Booking ID: " << bookingId << "\n";
    //     cout << "Client ID: " << clientId << "\n";
    //     cout << "Pet ID: " << petId << "\n";
    //     cout << "Service ID: " << serviceId << "\n";
    //     cout << "Date: " << date << "\n";
    //     cout << "Time: " << time << "\n";
    //     cout << "Status: " << status << "\n";
    //     cout << "====================\n";
    // }

    // Hiển thị dạng ngắn gọn
    void showBrief() const {
        cout << "[" << bookingId << "] " << date << " " << time
             << " - Pet: " << petId << " - " << status << "\n";
    }
    friend std::ostream &operator<<(std::ostream &out, Booking &book);
};

inline std::ostream &operator<<(std::ostream &out, Booking &book) {
    out << "=== BOOKING INFO ===\n";
    out << "Booking ID: " << book.bookingId << "\n";
    out << "Client ID: " << book.clientId << "\n";
    out << "Pet ID: " << book.petId << "\n";
    out << "Service ID: " << book.serviceId << "\n";
    out << "Date: " << book.date << "\n";
    out << "Time: " << book.time << "\n";
    out << "Status: " << book.status << "\n";
    out << "====================\n";
    return out;
}