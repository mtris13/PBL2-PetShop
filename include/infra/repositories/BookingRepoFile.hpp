/* Booking Repository - Quản lý đặt lịch spa
 *
 * File format: bookings.txt
 * Format: bookingId|clientId|petId|serviceId|date|time|status
 * Example: BK001|0123456789|d001|SP001|25/10/2025|14:00|Confirmed
 */

#pragma once
#include "domain/entities/Booking.hpp"
#include "ds/LinkedList.hpp"
#include <fstream>
#include <sstream>
#include <string>

struct PetSalesData {
  long revenue = 0;
  int soldCount = 0;
};

class BookingRepository {
private:
  std::string bookingFilePath;
  const std::string invalid = "";

  // --- THÊM MỚI: Biến lưu trữ ---
  LinkedList<Booking> m_bookings;

  // Helper: đọc một dòng dữ liệu theo bookingId
  string readingFile(const string &bookingId);

  // Helper: ghi/cập nhật một dòng dữ liệu
  void writingFile(const string &bookingId, const string &writeLine);

public:
  BookingRepository(const std::string &bookingPath);
  void _loadAllBookingsFromFile();
  // ===== GET Operations =====

  // Lấy thông tin đầy đủ của một booking
  Booking getBookingInfo(const string &bookingId);

  // Lấy tất cả bookings
  // Lấy tất cả bookings
  const LinkedList<Booking> &getAllBookings() const;

  // Lấy bookings của một client
  LinkedList<Booking> getBookingsByClient(const string &clientId);

  // Lấy bookings theo ngày
  LinkedList<Booking> getBookingsByDate(const string &date);

  // Lấy bookings theo trạng thái
  LinkedList<Booking> getBookingsByStatus(const string &status);

  // ===== SET Operations =====

  // Tạo booking mới
  void createBooking(const Booking &booking);

  // Cập nhật trạng thái booking
  void updateStatus(const string &bookingId, const string &newStatus);

  // Cập nhật thông tin booking
  void updateBooking(const Booking &booking);

  // ===== CHECK Operations =====

  // Kiểm tra bookingId có tồn tại không
  bool isValidBookingId(const string &bookingId);

  // Kiểm tra thời gian đã được đặt chưa (tránh trùng lặp)
  bool isTimeSlotAvailable(const string &date, const string &time);

  // ===== DELETE Operation =====

  // Xóa booking (hủy lịch)
  void deleteBooking(const string &bookingId);

  // ===== UTILITY =====

  // Sinh booking ID tự động
  string generateBookingId();

  // Đếm tổng số bookings
  int countBookings();

  // Đếm bookings theo trạng thái
  int countBookingsByStatus(const string &status);
  PetSalesData getPetSalesData();
};