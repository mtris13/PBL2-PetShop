#include "infra/repositories/AccountRepoFile.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string AccountRepository::filePath(const string &loginCode) {
  int length = loginCode.length();
  if (length == AdminIdLength)
    return AdminAccountFilePath;
  if (length == ClientIdLength)
    return ClientAccountFilePath;
  if (length == StaffIdLength)
    return StaffAccountFilePath;

  return invalid;
}

string AccountRepository::readingFile(const string &loginCode) {
  ifstream file(filePath(loginCode));
  if (!file.is_open()) {
    // cerr << "Error: Cant open file " << filePath(loginCode) << '\n';
    return invalid;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string code;
    getline(ss, code, '|');
    if (code == loginCode) {
      file.close();
      return line;
    }
  }
  return invalid;
}

void AccountRepository::writingFile(const string &loginCode,
                                    const string &writeLine) {
  const string path = filePath(loginCode);
  if (path == invalid) {
    cerr << "Error: Invalid file path for loginCode " << loginCode << '\n';
    return;
  }
  const string tempPath = "temp_account.txt";

  ifstream in(path);
  ofstream out(tempPath);

  if (!out.is_open()) {
    cerr << "Error: Cant create temp file " << tempPath << '\n';
    in.close();
    return;
  }

  string line;
  bool foundAndUpdated = false;

  if (in.is_open()) {
    while (getline(in, line)) {
      if (line.empty())
        continue;

      stringstream ss(line);
      string code;
      getline(ss, code, '|');

      if (code == loginCode) {
        out << writeLine << '\n';
        foundAndUpdated = true;
      } else
        out << line << '\n';
    }
  }

  if (!foundAndUpdated) {
    out << writeLine << '\n';
  }

  in.close();
  out.close();
  if (remove(path.c_str()) != 0 && in.is_open())
    cerr << "Error: Could not remove original file " << path << '\n';

  if (rename(tempPath.c_str(), path.c_str()) != 0)
    cerr << "Error: Could not rename temp file to " << path << '\n';
}

Account *AccountRepository::findAccountById(const string &loginCode) {
  if (!isValidId(loginCode))
    return nullptr;

  int length = loginCode.length();
  if (length == 3) { // Admin
    Admin ad = getAdminInfo(loginCode);
    return new Admin(ad);
  }
  if (length == 5) { // Staff
    Staff st = getStaffInfo(loginCode);
    return new Staff(st);
  }
  if (length == 10) { // Client
    Client cl = getClientInfo(loginCode);
    return new Client(cl);
  }

  return nullptr;
}

string AccountRepository::getAccountPassword(const string &loginCode) {
  string info = readingFile(loginCode);
  if (info == invalid)
    return invalid;
  string code, username, pw;
  stringstream ss(info);
  getline(ss, code, '|');
  getline(ss, username, '|');
  getline(ss, pw, '|');
  return pw;
}

// GET
string AccountRepository::getAccountName(const string &loginCode) {
  string info = readingFile(loginCode);
  if (info == invalid)
    return invalid;
  string code, username;
  stringstream ss(info);
  getline(ss, code, '|');
  getline(ss, username, '|');
  return username;
}

string AccountRepository::getAccountGender(const string &loginCode) {
  string info = readingFile(loginCode);
  if (info == invalid)
    return invalid;
  string dummy, gender;
  stringstream ss(info);
  getline(ss, dummy, '|');
  getline(ss, dummy, '|');
  getline(ss, dummy, '|');
  getline(ss, gender, '|');
  return gender;
}

Admin AccountRepository::getAdminInfo(const string &loginCode) {
  Admin info;
  if (loginCode.length() != AdminIdLength)
    return info;
  string line = readingFile(loginCode);
  if (line == invalid)
    return info; // tra ve thong tin rong
  string code, username, pw, gender;
  stringstream ss(line);
  getline(ss, code, '|');
  getline(ss, username, '|');
  getline(ss, pw, '|');
  getline(ss, gender, '|');
  info = Admin(code, username, pw, gender);
  return info;
}

Staff AccountRepository::getStaffInfo(const string &loginCode) {
  Staff info;
  if (loginCode.length() != StaffIdLength)
    return info;
  string line = readingFile(loginCode);
  if (line == invalid)
    return info; // tra ve thong tin rong
  string code, username, pw, gender, salary;
  stringstream ss(line);
  getline(ss, code, '|');
  getline(ss, username, '|');
  getline(ss, pw, '|');
  getline(ss, gender, '|');
  getline(ss, salary, '|');
  info = Staff(code, username, pw, gender, stol(salary));
  return info;
}

Client AccountRepository::getClientInfo(const string &loginCode) {
  Client info;
  if (loginCode.length() != ClientIdLength)
    return info;
  string line = readingFile(loginCode);
  if (line == invalid)
    return info; // tra ve thong tin rong
  string code, username, pw, gender, street, city;
  stringstream ss(line);
  getline(ss, code, '|');
  getline(ss, username, '|');
  getline(ss, pw, '|');
  getline(ss, gender, '|');
  getline(ss, street, '|');
  getline(ss, city, '|');
  info = Client(code, username, pw, gender, street, city);
  return info;
}

LinkedList<Admin> AccountRepository::getAllAdminInfo() {
  ifstream file(AdminAccountFilePath);
  LinkedList<Admin> admin;
  if (!file.is_open()) {
    cerr << "Error: Cant open file " << AdminAccountFilePath << '\n';
    return admin;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string code, name, pw, gender;
    getline(ss, code, '|');
    getline(ss, name, '|');
    getline(ss, pw, '|');
    getline(ss, gender, '|');
    Admin ad(code, name, pw, gender);
    admin.pushBack(ad);
  }
  file.close();
  return admin;
}

LinkedList<Staff> AccountRepository::getAllStaffInfo() {
  ifstream file(StaffAccountFilePath);
  LinkedList<Staff> staff;
  if (!file.is_open()) {
    cerr << "Error: Cant open file " << StaffAccountFilePath << '\n';
    return staff;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string code, name, pw, gender, sal;
    getline(ss, code, '|');
    getline(ss, name, '|');
    getline(ss, pw, '|');
    getline(ss, gender, '|');
    getline(ss, sal);
    Staff st = Staff(code, name, pw, gender, stol(sal));
    staff.pushBack(st);
  }
  file.close();
  return staff;
}

LinkedList<Client> AccountRepository::getAllClientInfo() {
  ifstream file(ClientAccountFilePath);
  LinkedList<Client> client;
  if (!file.is_open()) {
    cerr << "Error: Cant open file " << ClientAccountFilePath << '\n';
    return client;
  }
  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string code, name, pw, gender, street, city;
    getline(ss, code, '|');
    getline(ss, name, '|');
    getline(ss, pw, '|');
    getline(ss, gender, '|');
    getline(ss, street, '|');
    getline(ss, city);
    Client cl(code, name, pw, gender, street, city);
    client.pushBack(cl);
  }
  file.close();
  return client;
}

// SET
void AccountRepository::setAdminInfo(const Admin &ad) {
  string line = ad.getId() + '|' + ad.getName() + '|' + ad.getPassword() + '|' +
                ad.getGender();
  writingFile(ad.getId(), line);
}

void AccountRepository::setStaffInfo(const Staff &st) {
  string line = st.getId() + '|' + st.getName() + '|' + st.getPassword() + '|' +
                st.getGender() + '|' + to_string(st.getSalary());
  writingFile(st.getId(), line);
}

void AccountRepository::setClientInfo(const Client &cl) {
  string line = cl.getId() + '|' + cl.getName() + '|' + cl.getPassword() + '|' +
                cl.getGender() + '|' + cl.getStreet() + '|' + cl.getCity();
  writingFile(cl.getId(), line);
}

// OTHERS
bool AccountRepository::isValidId(const string &loginCode) {
  string info = readingFile(loginCode);
  if (info == invalid)
    return false;
  return true;
}

// AccountRepoFile.cpp
std::string trim(const std::string &s) {
  auto wsfront = std::find_if_not(s.begin(), s.end(),
                                  [](int c) { return std::isspace(c); });
  auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) {
                  return std::isspace(c);
                }).base();
  return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

bool AccountRepository::isValidPassword(const string &loginCode,
                                        const string &attemptPassword) {

  // BƯỚC 1: LẤY MẬT KHẨU THẬT TỪ FILE
  std::string passwordFromFile = getAccountPassword(loginCode);

  // BƯỚC 3: LÀM SẠCH (TRIM) CẢ HAI CHUỖI
  std::string cleanedPasswordFromFile = trim(passwordFromFile);
  std::string cleanedAttemptPassword = trim(attemptPassword);

  // BƯỚC 5: SO SÁNH CÁC CHUỖI ĐÃ LÀM SẠCH
  if (cleanedPasswordFromFile == cleanedAttemptPassword) {
    return true;
  } else {
    return false;
  }
}

void AccountRepository::deleteAccount(const string &loginCode) {
  const string path = filePath(loginCode);
  if (path == invalid)
    return;

  const string tempPath = "temp_account.txt";
  ifstream in(path);
  ofstream out(tempPath);

  if (!in.is_open() || !out.is_open()) {
    cerr << "Error opening files for removal." << endl;
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
    string code;
    getline(ss, code, '|');

    if (code == loginCode) {
      removed = true;
      backupDeletedLine(line);
    } else
      out << line << '\n';
  }

  in.close();
  out.close();

  if (removed) {
    remove(path.c_str());
    rename(tempPath.c_str(), path.c_str());
  } else {
    // Nếu không tìm thấy, không cần làm gì, chỉ cần xóa file temp
    remove(tempPath.c_str());
  }
}

AccountStats AccountRepository::countAccount() {
  AccountStats stats;
  std::string line;

  std::ifstream adminFile(AdminAccountFilePath);
  if (adminFile.is_open()) {
    while (std::getline(adminFile, line)) {
      if (!line.empty())
        stats.totalAdmin++;
    }
    adminFile.close();
  } else {
    std::cerr << "Error: Cannot open " << AdminAccountFilePath << '\n';
  }

  std::ifstream staffFile(StaffAccountFilePath);
  if (staffFile.is_open()) {
    while (std::getline(staffFile, line)) {
      if (!line.empty())
        stats.totalStaff++;
    }
    staffFile.close();
  } else {
    std::cerr << "Error: Cannot open " << StaffAccountFilePath << '\n';
  }

  std::ifstream clientFile(ClientAccountFilePath);
  if (clientFile.is_open()) {
    while (std::getline(clientFile, line)) {
      if (!line.empty())
        stats.totalClient++;
    }
    clientFile.close();
  } else {
    std::cerr << "Error: Cannot open " << ClientAccountFilePath << '\n';
  }

  return stats;
}
// AccountRepoFile.cpp

// (Dán vào cuối file)
void AccountRepository::registerClient(const Client &newClient) {
  // 1. Kiểm tra trùng ID
  if (isValidId(newClient.getId())) {
    // Nếu ID đã tồn tại, ném ra lỗi để MainWindow bắt được
    throw std::runtime_error("ID (Số điện thoại) này đã tồn tại!");
  }

  // 2. Tạo dòng dữ liệu: id|name|pass|gender|street|city
  // (Lưu ý: ClientAccount.txt của bạn có format là
  // id|name|pass|gender|street|city)
  string line = newClient.getId() + "|" + newClient.getName() + "|" +
                newClient.getPassword() + "|" + newClient.getGender() + "|" +
                newClient.getStreet() + "|" + newClient.getCity();

  // 3. Ghi vào file (Sử dụng hàm writingFile có sẵn của bạn)
  // Hàm writingFile của bạn đã có logic: Nếu không tìm thấy ID cũ, nó sẽ thêm
  // mới xuống cuối. Vì vậy chúng ta có thể tái sử dụng nó.
  writingFile(newClient.getId(), line);
}
// AccountRepoFile.cpp

void AccountRepository::backupDeletedLine(const string &line) {
  // Tạo thư mục backup nếu chưa có (tùy hệ điều hành, ở đây ta cứ ghi thẳng)
  // File backup chung cho tất cả tài khoản bị xóa
  ofstream backupFile("data/deleted_accounts.txt", std::ios::app);

  if (backupFile.is_open()) {
    // Ghi thêm thời gian xóa (Tùy chọn)
    // time_t now = time(0);
    // char* dt = ctime(&now);
    // backupFile << "[" << dt << "] " << line << "\n";

    backupFile << line << "\n"; // Ghi lại dòng dữ liệu cũ
    backupFile.close();
  } else {
    cerr << "Warning: Cannot open backup file." << endl;
  }
}