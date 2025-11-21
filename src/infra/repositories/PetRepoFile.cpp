#include "infra/repositories/PetRepoFile.hpp"
#include <QDebug>
using namespace std;
int safe_stoi(const std::string &str) {
  if (str.empty()) {
    return 0; // Trả về 0 nếu chuỗi rỗng
  }
  try {
    return std::stoi(str);
  } catch (const std::exception &e) {
    // In ra cửa sổ Application Output để chúng ta biết file .txt bị lỗi
    std::cerr << "Safe_stoi: Cant change '" << str << "' to number. "
              << e.what() << std::endl;
    return 0; // Trả về 0 nếu có lỗi
  }
}
// PetRepoFile.cpp
// ... (Hàm safe_stoi(...) của bạn ở đây) ...

// --- BẮT ĐẦU THÊM MỚI ---

// Hàm helper "an toàn" để đổi string sang long (cho price)
long safe_stol(const std::string &str) {
  if (str.empty()) {
    return 0L; // Trả về 0 (kiểu long) nếu chuỗi rỗng
  }
  try {
    return std::stol(str);
  } catch (const std::exception &e) {
    // In ra cửa sổ Application Output để chúng ta biết file .txt bị lỗi
    std::cerr << "Loi safe_stol: Khong the doi '" << str << "' sang so long. "
              << e.what() << std::endl;
    return 0L; // Trả về 0 (kiểu long) nếu có lỗi
  }
}

// --- KẾT THÚC THÊM MỚI ---

// ... (các hàm PetRepository::filePath, PetRepository::readingFile...)
string PetRepository::filePath(const string &petCode) {
  if (petCode.length() != petIdLength)
    return invalid;
  if (petCode[0] == 'd')
    return dogFilePath;
  return catFilePath;
}

string PetRepository::readingFile(const string &petCode) {
  ifstream file(filePath(petCode));
  if (!file.is_open()) {
    cerr << "Error: Cant open file " << filePath(petCode) << '\n';
    return invalid;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string code;
    getline(ss, code, '|');
    if (code == petCode) {
      file.close();
      return line;
    }
  }
  return invalid; // not found
}

void PetRepository::writingFile(const string &petCode,
                                const string &writeLine) {
  const string path = filePath(petCode);
  if (path == invalid) {
    cerr << "Error: Invalid file path for petCode " << petCode << '\n';
    return;
  }
  const string tempPath = "temp.txt";

  ifstream in(path);
  bool originalFileExisted = in.is_open();

  ofstream out(tempPath);
  if (!out.is_open()) {
    cerr << "Error: Cant create temp file " << tempPath << '\n';
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
      string code;
      getline(ss, code, '|');

      if (code == petCode) {
        out << writeLine << '\n';
        foundAndUpdated = true;
      } else
        out << line << '\n';
    }
  }

  if (!foundAndUpdated) {
    out << writeLine << '\n';
  }

  if (originalFileExisted)
    in.close();
  out.close();

  if (originalFileExisted && remove(path.c_str()) != 0)
    cerr << "Error: Could not remove original file " << path << '\n';

  if (rename(tempPath.c_str(), path.c_str()) != 0)
    cerr << "Error: Could not rename temp file to " << path << '\n';
}

bool PetRepository::isDogId(const string &petCode) {
  if (petCode.length() == petIdLength && petCode[0] == 'd')
    return true;
  return false;
}
string PetRepository::toLowercase(string &str) {
  for (int i = 0; i < str.length(); i++)
    str[i] = tolower(str[i]);
  return str;
}

// --- THÊM HÀM HELPER ĐỂ XÓA DẤU NGOẶC KÉP ---
string PetRepository::trimQuotes(string s) {
  if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
    return s.substr(1, s.length() - 2);
  }
  return s;
}

// GET
string PetRepository::getPetStatus(const string &petCode) {
  string info = readingFile(petCode);
  if (info == invalid)
    return invalid;

  string code, status;
  stringstream ss(info);
  getline(ss, code, '|');
  getline(ss, status, '|');
  if (status == "1")
    return "available";
  return "unavailable";
}

LinkedList<string> PetRepository::getAllCatId() {
  LinkedList<string> catIds;
  ifstream file(catFilePath);

  if (!file.is_open()) {
    cerr << "Error: Cant open file " << catFilePath << '\n';
    return catIds;
  }

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id;

    if (getline(ss, id, '|')) {
      catIds.pushBack(id);
    }
  }
  file.close();
  return catIds;
}

LinkedList<string> PetRepository::getAllDogId() {
  LinkedList<string> dogIds;
  ifstream file(dogFilePath);

  if (!file.is_open()) {
    cerr << "Error: Cant open file " << dogFilePath << '\n';
    return dogIds;
  }

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string id;

    if (getline(ss, id, '|')) {
      dogIds.pushBack(id);
    }
  }
  file.close();
  return dogIds;
}

// PetRepoFile.cpp
Dog PetRepository::getDogInfo(const string &petCode) {
  Node<Dog> *current = m_dogs.getHead();
  while (current != nullptr) {
    if (current->getData().getId() == petCode) { // <-- SỬA: Dùng getData()
      return current->getData();
    }
    current = current->getNext(); // <-- SỬA: Dùng getNext()
  }
  return Dog(); // Trả về Dog rỗng nếu không tìm thấy
}

// PetRepoFile.cpp
Cat PetRepository::getCatInfo(const string &petCode) {
  Node<Cat> *current = m_cats.getHead();
  while (current != nullptr) {
    if (current->getData().getId() == petCode) { // <-- SỬA: Dùng getData()
      return current->getData();
    }
    current = current->getNext(); // <-- SỬA: Dùng getNext()
  }
  return Cat(); // Trả về Cat rỗng nếu không tìm thấy
}

// PetRepoFile.cpp

// PetRepoFile.cpp

void PetRepository::_loadAllCatInfoFromFile() { // <-- 1. Sửa thành void
  // (KHÔNG TẠO: LinkedList<Cat> cats;)

  ifstream file(catFilePath);
  if (!file.is_open()) {
    cerr << "Error: Cant open file " << catFilePath << '\n';
    return; // <-- 2. Sửa: return (vì là hàm void)
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    // ... (toàn bộ code parse (phân tích) string của bạn giữ nguyên) ...
    string code, status, name, breed, age, price, fur, description;
    stringstream ss(line);
    getline(ss, code, '|');
    getline(ss, status, '|');
    getline(ss, name, '|');
    getline(ss, breed, '|');
    getline(ss, age, '|');
    getline(ss, price, '|');
    getline(ss, fur, '|');
    getline(ss, description);
    description = trimQuotes(description);

    Cat cat = Cat(code, name, breed, safe_stoi(age), safe_stol(price),
                  safe_stoi(status), fur, description);

    this->m_cats.pushBack(cat); // <-- 3. Sửa: Thêm trực tiếp vào 'm_cats'
  }
  file.close();
  // (KHÔNG CÓ: return;)
}

// PetRepoFile.cpp

// PetRepoFile.cpp

void PetRepository::_loadAllDogInfoFromFile() { // <-- 1. Sửa thành void
  // (KHÔNG TẠO: LinkedList<Dog> dogs;)
  qDebug() << "[PetRepo] Dang mo file Dog tai:"
           << QString::fromStdString(dogFilePath);
  ifstream file(dogFilePath);
  if (!file.is_open()) {
    cerr << "Error: Cant open file " << dogFilePath << '\n';
    qDebug() << "[PetRepo] LOI: MO FILE DOG THAT BAI!";
    return; // <-- 2. Sửa: return (vì là hàm void)
  }
  qDebug() << "[PetRepo] MO FILE DOG THANH CONG. Bat dau doc...";
  string line;
  int dogCount = 0;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    // ... (toàn bộ code parse (phân tích) string của bạn giữ nguyên) ...
    string code, status, name, breed, age, price, energy, description;
    stringstream ss(line);
    getline(ss, code, '|');
    getline(ss, status, '|');
    getline(ss, name, '|');
    getline(ss, breed, '|');
    getline(ss, age, '|');
    getline(ss, price, '|');
    getline(ss, energy, '|');
    getline(ss, description);
    description = trimQuotes(description);

    Dog dog = Dog(code, name, breed, safe_stoi(age), safe_stol(price),
                  safe_stoi(status), safe_stoi(energy), description);

    this->m_dogs.pushBack(dog); // <-- 3. Sửa: Thêm trực tiếp vào 'm_dogs'
    dogCount++;
  }
  file.close();
  // (KHÔNG CÓ: return;)
  qDebug() << "[PetRepo] Load Dog ket thuc. Tong so Dog da them:" << dogCount;
}

// PetRepoFile.cpp
LinkedList<Dog> PetRepository::getAllDogInfoAvailable() {
  LinkedList<Dog> availableDogs;
  Node<Dog> *current = m_dogs.getHead();
  while (current != nullptr) {
    if (current->getData().getStatus() ==
        "available") {                            // <-- SỬA: Dùng getData()
      availableDogs.pushBack(current->getData()); // <-- SỬA: Dùng getData()
    }
    current = current->getNext(); // <-- SỬA: Dùng getNext()
  }
  return availableDogs;
}

// PetRepoFile.cpp
LinkedList<Cat> PetRepository::getAllCatInfoAvailable() {
  LinkedList<Cat> availableCats;
  Node<Cat> *current = m_cats.getHead();
  while (current != nullptr) {
    if (current->getData().getStatus() ==
        "available") {                            // <-- SỬA: Dùng getData()
      availableCats.pushBack(current->getData()); // <-- SỬA: Dùng getData()
    }
    current = current->getNext(); // <-- SỬA: Dùng getNext()
  }
  return availableCats;
}

// SET
// infra/repositories/PetRepoFile.cpp

void PetRepository::setDogInfo(const Dog &d) {
  // 1. CẬP NHẬT RAM
  bool found = false;
  Node<Dog> *node = m_dogs.getHead();
  while (node) {
    if (node->getData().getId() == d.getId()) {
      node->setData(d);
      found = true;
      break;
    }
    node = node->getNext();
  }
  if (!found)
    m_dogs.pushBack(d);

  // 2. GHI FILE
  string status =
      d.getStatus() == "available" ? "1" : "0"; // Fix logic so sánh string
  if (d.getStatus() == "1")
    status = "1"; // Double check nếu input là "1"

  string line = d.getId() + "|" + status + "|" + d.getName() + "|" +
                d.getBreed() + "|" + to_string(d.getAge()) + "|" +
                to_string(d.getPrice()) + "|" + to_string(d.getEnergyLevel()) +
                "|\"" + d.getDescription() + "\"";
  writingFile(d.getId(), line);
}

void PetRepository::setCatInfo(const Cat &c) {
  // 1. CẬP NHẬT RAM
  bool found = false;
  Node<Cat> *node = m_cats.getHead();
  while (node) {
    if (node->getData().getId() == c.getId()) {
      node->setData(c);
      found = true;
      break;
    }
    node = node->getNext();
  }
  if (!found)
    m_cats.pushBack(c);

  // 2. GHI FILE
  string status = c.getStatus() == "available" ? "1" : "0";
  if (c.getStatus() == "1")
    status = "1";

  string line = c.getId() + "|" + status + "|" + c.getName() + "|" +
                c.getBreed() + "|" + to_string(c.getAge()) + "|" +
                to_string(c.getPrice()) + "|" + c.getFurLength() + "|\"" +
                c.getDescription() + "\"";
  writingFile(c.getId(), line);
}

// PetRepoFile.cpp

void PetRepository::setStatusUnavailable(const string &petCode) {
  // 1. CẬP NHẬT TRONG BỘ NHỚ (RAM) - QUAN TRỌNG NHẤT
  bool foundInMemory = false;

  // Kiểm tra trong danh sách Chó
  Node<Dog> *dogNode = m_dogs.getHead();
  while (dogNode != nullptr) {
    if (dogNode->getData().getId() == petCode) {
      // Lấy dữ liệu ra, sửa, và gán lại (vì LinkedList lưu bản sao hoặc tham
      // chiếu tùy cài đặt) Cách an toàn nhất là dùng hàm setter của đối tượng
      // Tuy nhiên, Node của bạn lưu dữ liệu T. Chúng ta cần sửa trực tiếp dữ
      // liệu đó. Giả sử Node cho phép sửa data qua tham chiếu hoặc pointer.
      // Nhưng để đơn giản và an toàn với cấu trúc hiện tại, ta sẽ cập nhật bản
      // ghi trong file trước.

      // Sửa trạng thái trong bộ nhớ
      // (Lưu ý: Cần đảm bảo phương thức setStatus có tồn tại trong Dog/Pet)
      // Để an toàn, ta dùng thủ thuật này nếu Node không trả về tham chiếu:
      Dog d = dogNode->getData();
      d.setStatus(0);      // 0 = unavailable
      dogNode->setData(d); // Cập nhật lại vào Node

      foundInMemory = true;
      break;
    }
    dogNode = dogNode->getNext();
  }

  // Nếu chưa thấy, kiểm tra trong danh sách Mèo
  if (!foundInMemory) {
    Node<Cat> *catNode = m_cats.getHead();
    while (catNode != nullptr) {
      if (catNode->getData().getId() == petCode) {
        Cat c = catNode->getData();
        c.setStatus(0);
        catNode->setData(c);
        foundInMemory = true;
        break;
      }
      catNode = catNode->getNext();
    }
  }

  // 2. CẬP NHẬT TRONG FILE (HDD) - Code cũ của bạn
  string info = readingFile(petCode);
  if (info == invalid)
    return;

  stringstream ss(info);
  string code, status, rest;
  getline(ss, code, '|');
  getline(ss, status, '|');
  getline(ss, rest);

  if (status == "1") {
    string newLine = code + "|0|" + rest;
    writingFile(petCode, newLine);
  }
}

void PetRepository::setStatusAvailable(const string &petCode) {
  string info = readingFile(petCode);
  if (info == invalid)
    return;

  stringstream ss(info);
  string code, status, rest;
  getline(ss, code, '|');
  getline(ss, status, '|');
  getline(ss, rest); // 'rest' sẽ tự động chứa cả description

  if (status == "0") {
    string newLine = code + "|1|" + rest;
    writingFile(petCode, newLine);
  }
}
// use when pet is restored

// OTHERS
bool PetRepository::isAvailablePet(const string &petCode) {
  string info = readingFile(petCode);
  if (info == invalid)
    return false;

  string code, status;
  stringstream ss(info);
  getline(ss, code, '|');
  getline(ss, status, '|');
  return (status == "1");
}

bool PetRepository::isValidPetId(const string &petCode) {
  string info = readingFile(petCode);
  return (info == invalid ? false : true);
}

void PetRepository::deletePet(const string &petCode) {
  const string path = filePath(petCode);
  if (path == invalid)
    return;

  const string tempPath = "temp.txt";
  const string backupPath = "deleted_pets.txt"; // File lưu dữ liệu đã xóa

  ifstream in(path);
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
    string code;
    getline(ss, code, '|');

    if (code == petCode) {
      removed = true;

      // --- LOGIC BACKUP: Ghi dòng bị xóa vào file backup ---
      ofstream backup(backupPath, std::ios::app); // ios::app để ghi nối tiếp
      if (backup.is_open()) {
        // Có thể thêm timestamp nếu muốn: backup << "Deleted at ...: " << line
        // << endl;
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
    remove(path.c_str());
    rename(tempPath.c_str(), path.c_str());

    // --- CẬP NHẬT RAM NGAY LẬP TỨC (Để bảng cập nhật luôn) ---
    if (petCode[0] == 'd') {
      this->m_dogs = LinkedList<Dog>(); // Xóa sạch RAM cũ
      _loadAllDogInfoFromFile();        // Load lại file mới
    } else {
      this->m_cats = LinkedList<Cat>(); // Xóa sạch RAM cũ
      _loadAllCatInfoFromFile();        // Load lại file mới
    }
  } else {
    remove(tempPath.c_str());
  }
}

string PetRepository::generatePetId(string &type) {
  int maxId = 0;
  string newId;
  toLowercase(type);
  if (type == "dog") {
    LinkedList<string> dogId = getAllDogId();
    Node<string> *dog = dogId.getHead();
    while (dog != nullptr) {
      string id = dog->getData();
      int num = stoi(id.substr(1, 3));
      maxId = max(num, maxId);
      dog = dog->getNext();
    }
    newId = "d";
  } else {
    LinkedList<string> catId = getAllCatId();
    Node<string> *cat = catId.getHead();
    while (cat != nullptr) {
      string id = cat->getData();
      int num = stoi(id.substr(1, 3));
      maxId = max(num, maxId);
      cat = cat->getNext();
    }
    newId = "c";
  }
  // Tạo ID mới
  maxId++;
  if (maxId < 10)
    newId += "00";
  else if (maxId < 100)
    newId += "0";
  newId += to_string(maxId);

  return newId;
}

// PetRepoFile.cpp

// ... (các phần trên giữ nguyên)

// --- SỬA LẠI HÀM SEARCH DOG ---
LinkedList<Dog> PetRepository::searchDog(string, string &keyword) {
  LinkedList<Dog> result;
  string key = keyword;
  // Đảm bảo từ khóa tìm kiếm là chữ thường
  transform(key.begin(), key.end(), key.begin(), ::tolower);

  Node<Dog> *node = m_dogs.getHead();
  while (node) {
    Dog d = node->getData();

    // 1. Lấy dữ liệu các trường
    string id = d.getId();
    string name = d.getName();
    string breed = d.getBreed();

    // 2. Chuyển tất cả sang chữ thường để so sánh chính xác
    transform(id.begin(), id.end(), id.begin(),
              ::tolower); // <--- QUAN TRỌNG: Chuyển ID sang thường
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    transform(breed.begin(), breed.end(), breed.begin(), ::tolower);

    // 3. Kiểm tra: Nếu ID hoặc Tên hoặc Giống chứa từ khóa -> Lấy
    if (id.find(key) != string::npos || // <--- ĐÃ THÊM DÒNG NÀY ĐỂ TÌM ID
        name.find(key) != string::npos || breed.find(key) != string::npos) {

      result.pushBack(d);
    }
    node = node->getNext();
  }
  return result;
}

// --- SỬA LẠI HÀM SEARCH CAT ---
LinkedList<Cat> PetRepository::searchCat(string, string &keyword) {
  LinkedList<Cat> result;
  string key = keyword;
  transform(key.begin(), key.end(), key.begin(), ::tolower);

  Node<Cat> *node = m_cats.getHead();
  while (node) {
    Cat c = node->getData();

    string id = c.getId();
    string name = c.getName();
    string breed = c.getBreed();

    transform(id.begin(), id.end(), id.begin(), ::tolower); // <--- QUAN TRỌNG
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    transform(breed.begin(), breed.end(), breed.begin(), ::tolower);

    if (id.find(key) != string::npos || // <--- ĐÃ THÊM DÒNG NÀY ĐỂ TÌM ID
        name.find(key) != string::npos || breed.find(key) != string::npos) {

      result.pushBack(c);
    }
    node = node->getNext();
  }
  return result;
}

PetStats PetRepository::countPet() {
  PetStats stats; // Khởi tạo tất cả giá trị = 0
  string line;

  // ===== DOGS =====
  // Giả định bạn có biến thành viên "dogFilePath"
  ifstream dogFile(dogFilePath);
  if (dogFile.is_open()) {
    while (getline(dogFile, line)) {
      if (line.empty())
        continue;

      stats.totalDogs++;

      stringstream ss(line);
      string id, status;
      getline(ss, id, '|');
      getline(ss, status, '|');

      // Logic đếm của bạn
      if (status == "1")
        stats.availableDogs++;
      else if (status == "0")
        stats.soldDogs++;
    }
    dogFile.close();
  } else {
    cerr << "Error: Cannot open " << dogFilePath << '\n';
  }

  // ===== CATS =====
  // Giả định bạn có biến thành viên "catFilePath"
  ifstream catFile(catFilePath);
  if (catFile.is_open()) {
    while (getline(catFile, line)) {
      if (line.empty())
        continue;

      stats.totalCats++;

      stringstream ss(line);
      string id, status;
      getline(ss, id, '|');
      getline(ss, status, '|');

      // Logic đếm của bạn
      if (status == "0")
        stats.soldCats++;
      else
        stats.availableCats++;
    }
    catFile.close();
  } else {
    cerr << "Error: Cannot open " << catFilePath << '\n';
  }

  return stats;
}
// PetRepoFile.cpp

// SỬA HÀM NÀY
PetRepository::PetRepository(const string &dogPath, const string &catPath)
    : dogFilePath(dogPath), catFilePath(catPath) {
  // --- THÊM MỚI ---
  // Tải toàn bộ dữ liệu vào bộ nhớ ngay khi khởi tạo
  _loadAllDogInfoFromFile(); // <-- KHÔNG CÓ "this->m_dogs ="
  _loadAllCatInfoFromFile(); // <-- KHÔNG CÓ "this->m_cats ="
  // --- KẾT THÚC THÊM MỚI ---
}
