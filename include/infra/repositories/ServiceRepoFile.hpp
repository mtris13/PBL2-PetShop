// ServiceRepoFile.hpp (ĐÃ SỬA)
#pragma once
#include "domain/entities/Service.hpp"
#include "ds/LinkedList.hpp"
#include <fstream>
#include <sstream>
#include <string>

class ServiceRepository {
private:
  std::string serviceFilePath; // <-- SỬA 1: Xóa 'const' và đường dẫn
  const std::string invalid = "";

  // --- THÊM MỚI ---
  LinkedList<Service> m_services; // Biến lưu trữ dữ liệu
  // --- KẾT THÚC THÊM MỚI ---

  std::string readingFile(const std::string &serviceId);
  void writingFile(const std::string &serviceId, const std::string &writeLine);

public:
  // --- SỬA 2: Sửa Constructor ---
  ServiceRepository(const std::string &servicePath);
  // --- KẾT THÚC SỬA ---

  // ===== GET Operations =====
  Service getServiceInfo(const std::string &serviceId);
  const LinkedList<Service> &getAllServices() const;
  long getServicePrice(const std::string &serviceId);
  int getServiceDuration(const std::string &serviceId);

  // --- THÊM MỚI ---
  void _loadAllServicesFromFile(); // Hàm tải dữ liệu
  // --- KẾT THÚC THÊM MỚI ---

  // ... (Các hàm set, check, delete, search, count khác giữ nguyên) ...
  void setServiceInfo(const Service &service);
  void updatePrice(const std::string &serviceId, long newPrice);
  bool isValidServiceId(const std::string &serviceId);
  void deleteService(const std::string &serviceId);
  LinkedList<Service> searchByName(const std::string &keyword);
  LinkedList<Service> searchByPriceRange(long minPrice, long maxPrice);
  int countServices();
};