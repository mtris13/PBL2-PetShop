// ServiceRepoFile.cpp (ĐÃ SỬA TOÀN BỘ)

#include "infra/repositories/ServiceRepoFile.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// --- BẮT ĐẦU THÊM MỚI: HÀM AN TOÀN ---
int safe_stoi_service(const std::string &str) {
  if (str.empty())
    return 0;
  try {
    return std::stoi(str);
  } catch (const std::exception &e) {
    std::cerr << "Loi safe_stoi_service: " << e.what() << std::endl;
    return 0;
  }
}
long safe_stol_service(const std::string &str) {
  if (str.empty())
    return 0L;
  try {
    return std::stol(str);
  } catch (const std::exception &e) {
    std::cerr << "Loi safe_stol_service: " << e.what() << std::endl;
    return 0L;
  }
}
// --- KẾT THÚC THÊM MỚI ---

// --- SỬA 1: HÀM KHỞI TẠO (CONSTRUCTOR) ---
ServiceRepository::ServiceRepository(const std::string &servicePath)
    : serviceFilePath(servicePath) {
  // Tải dữ liệu vào bộ nhớ m_services ngay khi khởi tạo
  _loadAllServicesFromFile();
}

// --- SỬA 2: HÀM TẢI DỮ LIỆU TỪ FILE ---
void ServiceRepository::_loadAllServicesFromFile() {
  ifstream file(serviceFilePath);
  if (!file.is_open()) {
    cerr << "Error: Cannot open file " << serviceFilePath << '\n';
    return;
  }

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id, name, desc, priceStr, durationStr;

    getline(ss, id, '|');
    getline(ss, name, '|');
    getline(ss, desc, '|');
    getline(ss, priceStr, '|');
    getline(ss, durationStr, '|');

    // Dùng hàm "an toàn" để tránh crash
    long price = safe_stol_service(priceStr);
    int duration = safe_stoi_service(durationStr);

    Service service(id, name, desc, price, duration);
    this->m_services.pushBack(service); // Thêm vào bộ nhớ
  }
  file.close();
}

// ===== Private Helper Methods (SỬA LỖI ĐƯỜNG DẪN) =====

string ServiceRepository::readingFile(const string &serviceId) {
  // (Hàm này có thể không cần nữa nếu chúng ta đọc từ m_services,
  //  nhưng tạm thời giữ lại cho các hàm write/delete)
  ifstream file(serviceFilePath);
  if (!file.is_open()) {
    cerr << "Error: Cannot open file " << serviceFilePath << '\n';
    return invalid;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string id;
    getline(ss, id, '|');
    if (id == serviceId) {
      file.close();
      return line;
    }
  }
  file.close();
  return invalid;
}

void ServiceRepository::writingFile(const string &serviceId,
                                    const string &writeLine) {
  // --- SỬA 3: SỬA LỖI ĐƯỜNG DẪN "temp.txt" ---
  const string tempPath = "temp_service.txt"; // Sửa thành đường dẫn tương đối

  ifstream in(serviceFilePath);
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
      if (id == serviceId) {
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
  if (originalFileExisted && remove(serviceFilePath.c_str()) != 0) {
    cerr << "Error: Could not remove original file " << serviceFilePath << '\n';
  }
  if (rename(tempPath.c_str(), serviceFilePath.c_str()) != 0) {
    cerr << "Error: Could not rename temp file to " << serviceFilePath << '\n';
  }
}

// ===== GET Operations (SỬA LẠI ĐỂ ĐỌC TỪ BỘ NHỚ) =====

Service ServiceRepository::getServiceInfo(const string &serviceId) {
  Node<Service> *current = m_services.getHead();
  while (current != nullptr) {
    if (current->getData().getId() == serviceId) {
      return current->getData();
    }
    current = current->getNext();
  }
  return Service(); // Trả về rỗng nếu không tìm thấy
}

const LinkedList<Service> &ServiceRepository::getAllServices() const {
  return m_services;
}

long ServiceRepository::getServicePrice(const string &serviceId) {
  Service service = getServiceInfo(serviceId); // Đã đọc từ bộ nhớ
  return service.getPrice();
}

int ServiceRepository::getServiceDuration(const string &serviceId) {
  Service service = getServiceInfo(serviceId); // Đã đọc từ bộ nhớ
  return service.getDuration();
}

// ===== SET Operations (Giữ nguyên) =====
// infra/repositories/ServiceRepoFile.cpp

void ServiceRepository::setServiceInfo(const Service &service) {
  // 1. CẬP NHẬT RAM (Để bảng cập nhật ngay lập tức)
  bool foundInMemory = false;
  Node<Service> *node = m_services.getHead();
  while (node != nullptr) {
    if (node->getData().getId() == service.getId()) {
      node->setData(service); // Nếu đã có -> Cập nhật
      foundInMemory = true;
      break;
    }
    node = node->getNext();
  }
  if (!foundInMemory) {
    m_services.pushBack(service); // Nếu chưa có -> Thêm mới
  }

  // 2. GHI FILE (Logic cũ)
  string line = service.getId() + "|" + service.getName() + "|" +
                service.getDescription() + "|" + to_string(service.getPrice()) +
                "|" + to_string(service.getDuration());
  writingFile(service.getId(), line);
}
void ServiceRepository::updatePrice(const string &serviceId, long newPrice) {
  Service service = getServiceInfo(serviceId);
  if (service.getId().empty()) {
    cerr << "Error: Service not found " << serviceId << '\n';
    return;
  }
  service.setPrice(newPrice);
  setServiceInfo(service);
}

// ===== CHECK Operations (Đọc từ bộ nhớ) =====
bool ServiceRepository::isValidServiceId(const string &serviceId) {
  Node<Service> *current = m_services.getHead();
  while (current != nullptr) {
    if (current->getData().getId() == serviceId) {
      return true;
    }
    current = current->getNext();
  }
  return false;
}

// ===== DELETE Operations (SỬA LỖI ĐƯỜNG DẪN) =====
void ServiceRepository::deleteService(const string &serviceId) {
  const string tempPath = "temp_service_del.txt";
  const string backupPath = "deleted_services.txt"; // File lưu dịch vụ đã xóa

  ifstream in(serviceFilePath);
  ofstream out(tempPath);

  if (!in.is_open() || !out.is_open()) {
    cerr << "Error opening files for removal.\n";
    if (in.is_open())
      in.close();
    if (out.is_open())
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

    if (id == serviceId) {
      removed = true;

      // --- LOGIC BACKUP: Ghi dòng bị xóa vào file backup ---
      ofstream backup(backupPath, std::ios::app);
      if (backup.is_open()) {
        backup << line << endl;
        backup.close();
      }
      // -----------------------------------------------------

    } else {
      out << line << '\n';
    }
  }
  in.close();
  out.close();

  if (removed) {
    remove(serviceFilePath.c_str());
    rename(tempPath.c_str(), serviceFilePath.c_str());

    // --- CẬP NHẬT RAM NGAY LẬP TỨC (Để bảng cập nhật luôn) ---
    this->m_services = LinkedList<Service>(); // Xóa sạch RAM cũ
    _loadAllServicesFromFile();               // Load lại file mới
  } else {
    remove(tempPath.c_str());
  }
}

// ===== SEARCH Operations (Đọc từ bộ nhớ) =====
LinkedList<Service> ServiceRepository::searchByName(const string &keyword) {
  LinkedList<Service> results;
  const LinkedList<Service> &allServices = getAllServices(); // Đọc từ bộ nhớ
  string lowerKeyword = keyword;
  transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(),
            ::tolower);
  Node<Service> *current = allServices.getHead();
  while (current != nullptr) {
    string serviceName = current->getData().getName();
    transform(serviceName.begin(), serviceName.end(), serviceName.begin(),
              ::tolower);
    if (serviceName.find(lowerKeyword) != string::npos) {
      results.pushBack(current->getData());
    }
    current = current->getNext();
  }
  return results;
}
LinkedList<Service> ServiceRepository::searchByPriceRange(long minPrice,
                                                          long maxPrice) {
  LinkedList<Service> results;
  const LinkedList<Service> &allServices = getAllServices(); // Đọc từ bộ nhớ
  Node<Service> *current = allServices.getHead();
  while (current != nullptr) {
    long price = current->getData().getPrice();
    if (price >= minPrice && price <= maxPrice) {
      results.pushBack(current->getData());
    }
    current = current->getNext();
  }
  return results;
}
int ServiceRepository::countServices() {
  return m_services.getSize(); // Đọc từ bộ nhớ
}