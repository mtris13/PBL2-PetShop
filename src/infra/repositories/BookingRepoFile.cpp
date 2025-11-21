#include "infra/repositories/BookingRepoFile.hpp"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
// ===== Private Helper Methods =====
BookingRepository::BookingRepository(const std::string &bookingPath)
    : bookingFilePath(bookingPath) {
  // Tải dữ liệu vào bộ nhớ m_bookings ngay khi khởi tạo
  _loadAllBookingsFromFile();
}
string BookingRepository::readingFile(const string &bookingId) {
  ifstream file(bookingFilePath);
  if (!file.is_open()) {
    cerr << "Error: Cannot open file " << bookingFilePath << '\n';
    return invalid;
  }

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id;
    getline(ss, id, '|');

    if (id == bookingId) {
      file.close();
      return line;
    }
  }

  file.close();
  return invalid;
}

void BookingRepository::writingFile(const string &bookingId,
                                    const string &writeLine) {
  const string tempPath = "temp_booking.txt";

  ifstream in(bookingFilePath);
  bool originalFileExisted = in.is_open();

  ofstream out(tempPath);
  if (!out.is_open()) {
    cerr << "Error: Cannot create temp file " << tempPath << '\n';
    if (originalFileExisted)
      in.close();
    return;
  }

  string line;
  bool foundAndUpdated = false;

  if (originalFileExisted) {
    while (getline(in, line)) {
      if (line.empty())
        continue;

      stringstream ss(line);
      string id;
      getline(ss, id, '|');

      if (id == bookingId) {
        out << writeLine << '\n';
        foundAndUpdated = true;
      } else {
        out << line << '\n';
      }
    }
  }

  if (!foundAndUpdated) {
    out << writeLine << '\n';
  }

  if (originalFileExisted)
    in.close();
  out.close();

  if (originalFileExisted && remove(bookingFilePath.c_str()) != 0) {
    cerr << "Error: Could not remove original file " << bookingFilePath << '\n';
  }

  if (rename(tempPath.c_str(), bookingFilePath.c_str()) != 0) {
    cerr << "Error: Could not rename temp file to " << bookingFilePath << '\n';
  }
}

// ===== GET Operations =====

Booking BookingRepository::getBookingInfo(const string &bookingId) {
  Node<Booking> *current = m_bookings.getHead();
  while (current != nullptr) {
    if (current->getData().getBookingId() == bookingId) {
      return current->getData();
    }
    current = current->getNext();
  }
  return Booking(); // Trả về rỗng nếu không tìm thấy
}

const LinkedList<Booking> &BookingRepository::getAllBookings() const {
  return this->m_bookings;
}

LinkedList<Booking>
BookingRepository::getBookingsByClient(const string &clientId) {
  LinkedList<Booking> results;
  const LinkedList<Booking> &allBookings = getAllBookings();

  Node<Booking> *current = allBookings.getHead();
  while (current != nullptr) {
    if (current->getData().getClientId() == clientId) {
      results.pushBack(current->getData());
    }
    current = current->getNext();
  }

  return results;
}

LinkedList<Booking> BookingRepository::getBookingsByDate(const string &date) {
  LinkedList<Booking> results;
  const LinkedList<Booking> &allBookings = getAllBookings();

  Node<Booking> *current = allBookings.getHead();
  while (current != nullptr) {
    if (current->getData().getDate() == date) {
      results.pushBack(current->getData());
    }
    current = current->getNext();
  }

  return results;
}

LinkedList<Booking>
BookingRepository::getBookingsByStatus(const string &status) {
  LinkedList<Booking> results;
  const LinkedList<Booking> &allBookings = getAllBookings();

  Node<Booking> *current = allBookings.getHead();
  while (current != nullptr) {
    if (current->getData().getStatus() == status) {
      results.pushBack(current->getData());
    }
    current = current->getNext();
  }

  return results;
}

// ===== SET Operations =====

// BookingRepoFile.cpp

void BookingRepository::createBooking(const Booking &booking) {
    // 1. Ghi vào file (như cũ)
    string line = booking.getBookingId() + "|" + booking.getClientId() + "|" +
                  booking.getPetId() + "|" + booking.getServiceId() + "|" +
                  booking.getDate() + "|" + booking.getTime() + "|" +
                  booking.getStatus();
    writingFile(booking.getBookingId(), line);

    // 2. CẬP NHẬT BỘ NHỚ (SỬA LỖI TRÙNG LẶP)
    bool exists = false;
    Node<Booking>* current = m_bookings.getHead();

    // Duyệt qua danh sách để xem ID này đã có chưa
    while (current != nullptr) {
        if (current->getData().getBookingId() == booking.getBookingId()) {
            // Nếu đã có, CẬP NHẬT dữ liệu mới vào vị trí cũ
            current->setData(booking);
            exists = true;
            break;
        }
        current = current->getNext();
    }

    // Chỉ thêm mới nếu chưa tồn tại
    if (!exists) {
        this->m_bookings.pushBack(booking);
    }
}

void BookingRepository::updateStatus(const string &bookingId,
                                     const string &newStatus) {
  // 1. Cập nhật trong bộ nhớ (m_bookings)
  Node<Booking> *current = m_bookings.getHead();
  Booking *bookingToUpdate = nullptr;
  while (current != nullptr) {
    if (current->getData().getBookingId() == bookingId) {
      bookingToUpdate = &(current->getData()); // Lấy địa chỉ của booking
      break;
    }
    current = current->getNext();
  }

  if (bookingToUpdate == nullptr) {
    cerr << "Error: Booking not found in memory " << bookingId << '\n';
    return;
  }

  bookingToUpdate->setStatus(newStatus); // Cập nhật bộ nhớ

  // 2. Ghi đè vào file (dùng hàm createBooking đã được sửa)
  updateBooking(*bookingToUpdate);
}

void BookingRepository::updateBooking(const Booking &booking) {
  createBooking(booking);
}

// ===== CHECK Operations =====

bool BookingRepository::isValidBookingId(const string &bookingId) {
  string info = readingFile(bookingId);
  return (info != invalid);
}

bool BookingRepository::isTimeSlotAvailable(const string &date,
                                            const string &time) {
  // DUYỆT trên bản sao tạo ra từ bộ nhớ nội bộ bằng tham chiếu (KHÔNG COPY)
  LinkedList<Booking> bookingsOnDate = getBookingsByDate(
      date); // getBookingsByDate trả results by value, that's fine
  Node<Booking> *current = bookingsOnDate.getHead();
  while (current != nullptr) {
    Booking booking = current->getData();
    if (booking.getTime() == time && booking.getStatus() != "Cancelled") {
      return false;
    }
    current = current->getNext();
  }
  return true;
}

// ===== DELETE Operation =====

void BookingRepository::deleteBooking(const string &bookingId) {
  const string tempPath = "temp_booking_del.txt";

  ifstream in(bookingFilePath);
  ofstream out(tempPath);

  if (!in.is_open() || !out.is_open()) {
    cerr << "Error opening files for removal.\n";
    in.close();
    out.close();
    return;
  }

  string line;
  bool removed = false;

  while (getline(in, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id;
    getline(ss, id, '|');

    if (id == bookingId) {
      removed = true;
    } else {
      out << line << '\n';
    }
  }

  in.close();
  out.close();

  if (removed) {
    remove(bookingFilePath.c_str());
    rename(tempPath.c_str(), bookingFilePath.c_str());
  } else {
    remove(tempPath.c_str());
  }
}

// ===== UTILITY =====

string BookingRepository::generateBookingId() {
  const LinkedList<Booking> &allBookings = getAllBookings(); // use ref, no copy
  int maxNum = 0;

  Node<Booking> *current = allBookings.getHead();
  while (current != nullptr) {
    string id = current->getData().getBookingId();
    if (id.length() >= 2 && id.substr(0, 2) == "BK") {
      try {
        int num = stoi(id.substr(2));
        maxNum = max(maxNum, num);
      } catch (...) {
        // nếu id không parse được, bỏ qua
      }
    }
    current = current->getNext();
  }

  maxNum++;
  string newId = "BK";
  if (maxNum < 10)
    newId += "00";
  else if (maxNum < 100)
    newId += "0";
  newId += to_string(maxNum);

  return newId;
}

int BookingRepository::countBookings() {
  const LinkedList<Booking> &allBookings = getAllBookings();
  return allBookings.getSize();
}

int BookingRepository::countBookingsByStatus(const string &status) {
  LinkedList<Booking> bookings =
      getBookingsByStatus(status); // results by value ok
  return bookings.getSize();
}
// --- HÀM TẢI DỮ LIỆU MỚI ---
void BookingRepository::_loadAllBookingsFromFile() {
  ifstream file(bookingFilePath);
  if (!file.is_open()) {
    cerr << "Error: Cannot open file " << bookingFilePath << '\n';
    return;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string bookingId, clientId, petId, serviceId, date, time, status;

    getline(ss, bookingId, '|');
    getline(ss, clientId, '|');
    getline(ss, petId, '|');
    getline(ss, serviceId, '|');
    getline(ss, date, '|');
    getline(ss, time, '|');
    getline(ss, status, '|');

    // (Giả định Booking constructor của bạn là (string, string, ...))
    Booking booking(bookingId, clientId, petId, serviceId, date, time, status);
    this->m_bookings.pushBack(booking); // Thêm vào bộ nhớ
  }
  file.close();
}
