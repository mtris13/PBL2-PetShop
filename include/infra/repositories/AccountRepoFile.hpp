/* đọc và ghi các file Accounts:
Service:
    check password
    check available id
    get information from all type of accounts
    set information form all type of accounts
    delete account
    !!: there isn't setting password/name/etc functions for accounts. Only
change info via setAdminInfo/setClientInfo/setStaffInfo. generate them if
needed.
*/

/* một số tính năng có thể bổ sung: mã hóa mật khẩu */
#pragma once
#include "domain/entities/Account.hpp"
#include "domain/entities/Admin.hpp"
#include "domain/entities/Client.hpp"
#include "domain/entities/Staff.hpp"
#include "ds/LinkedList.hpp"
#include <fstream>
#include <sstream>
#include <string>

struct AccountStats {
  int totalAdmin = 0;
  int totalStaff = 0;
  int totalClient = 0;
};

class AccountRepository {
private:
  string AdminAccountFilePath;
  string ClientAccountFilePath;
  string StaffAccountFilePath;
  const int AdminIdLength = 3;
  const int ClientIdLength = 10; // so dien thoai
  const int StaffIdLength =
      5; // che giấu những const để các file khác không nhìn thấy được
  const string invalid = "";
  void backupDeletedLine(const string &line);
  string filePath(const string &loginCode);
  string readingFile(const string &loginCode);
  void writingFile(const string &loginCode, const string &writeLine);
  string getAccountPassword(const string &loginCode);

public:
  AccountRepository() {}
  AccountRepository(const string &adminPath, const string &clientPath,
                    const string &staffPath)
      : AdminAccountFilePath(adminPath), ClientAccountFilePath(clientPath),
        StaffAccountFilePath(staffPath) {
    // Khởi tạo ở đây
    // (Bạn có thể thêm code kiểm tra file tồn tại nếu muốn)
  }
  void registerClient(const Client &newClient);
  // GET
  string getAccountName(const string &loginCode);
  string getAccountGender(const string &loginCode);
  Admin getAdminInfo(const string &loginCode);
  Staff getStaffInfo(const string &loginCode);
  Client getClientInfo(const string &loginCode);
  LinkedList<Admin> getAllAdminInfo();
  LinkedList<Staff> getAllStaffInfo();
  LinkedList<Client> getAllClientInfo();

  // SET
  void setAdminInfo(const Admin &ad);
  void setStaffInfo(const Staff &st);
  void setClientInfo(const Client &cl);

  // ANOTHER
  bool isValidId(const string &loginCode);
  bool isValidPassword(const string &loginCode, const string &attemptPassword);
  void deleteAccount(const string &loginCode);
  Account *findAccountById(const string &loginCode);
  AccountStats countAccount();
};