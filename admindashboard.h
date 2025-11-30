#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>
#include <QButtonGroup>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDateEdit>
#include <QMap>

#include "domain/entities/Admin.hpp"
#include "infra/repositories/AccountRepoFile.hpp"
#include "infra/repositories/PetRepoFile.hpp"
#include "infra/repositories/CartRepoFile.hpp"
#include "infra/repositories/ServiceRepoFile.hpp"
#include "infra/repositories/BookingRepoFile.hpp"
#include "infra/repositories/BillRepoFile.hpp"
struct BreedStat {
    QString breedName;
    QString type;
    int quantity = 0;
    long long revenue = 0;
};

struct CustomerStat {
    QString id;
    QString name;
    int buyCount = 0;
    long long totalSpent = 0;
};
namespace Ui {
class AdminDashboard;
}

class AdminDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDashboard(Admin* admin,
                            AccountRepository* accountRepo,
                            PetRepository* petRepo,
                            CartRepository* cartRepo,
                            ServiceRepository* serviceRepo,
                            BookingRepository* bookingRepo,
                            BillRepository* billRepo,
                            QWidget *parent = nullptr);
    ~AdminDashboard();

signals:
    void logoutSignal();

private slots:
    // --- NAVBAR ---
    void handleManageAccountsClick();
    void handleManagePetsClick();
    void handleManageSpaClick();
    void handleHistoryClick();
    void handleStatsClick();
    void handleProfileClick();
    void handleLogoutClick();

    // --- ACCOUNTS
    void on_accSearchButton_clicked();
    void on_addStaffButton_clicked();
    void on_viewAccButton_clicked();
    void on_deleteAccButton_clicked();
    void on_saveAccButton_clicked();
    void on_cancelAccButton_clicked();

    // --- PETS
    void on_petSearchButton_clicked();
    void on_addPetButton_clicked();
    void on_editPetButton_clicked();
    void on_deletePetButton_clicked();
    void on_savePetButton_clicked();
    void on_cancelPetButton_clicked();

    void on_dogRadio_toggled(bool checked);

    // --- SPA
    void on_spaSearchButton_clicked();
    void on_addServiceButton_clicked();
    void on_editServiceButton_clicked();
    void on_deleteServiceButton_clicked();
    void on_saveServiceButton_clicked();
    void on_cancelServiceButton_clicked();
void on_historySearchButton_clicked();
    void on_filterStatsButton_clicked();
void on_saveProfileButton_clicked();
private:
    Ui::AdminDashboard *ui;
    Admin* m_currentAdmin;

    // Pointers Repo
    AccountRepository* m_accountRepo;
    PetRepository* m_petRepo;
    CartRepository* m_cartRepo;
    ServiceRepository* m_serviceRepo;
    BookingRepository* m_bookingRepo;
    BillRepository* m_billRepo;

    QButtonGroup* m_navGroup;

    // --- BIẾN TRẠNG THÁI ---
    bool m_isEditMode = false;    // True = Đang sửa, False = Đang thêm mới
    std::string m_editingId = ""; // Lưu ID của đối tượng đang sửa

    // --- HÀM HỖ TRỢ ---
    void loadAccountsTable(const QString& search = ""); // Giữ nguyên
    void loadPetsTable(const QString& search = "");     // Viết lại
    void loadSpaTable(const QString& search = "");      // Viết lại

    // Hàm backup dữ liệu khi xóa
    void backupData(const QString& folderName, const QString& id, const QString& content);
    void loadHistoryTable(const QString& search = "");

    // Hàm hiển thị chi tiết hóa đơn (đọc file .txt)
    void showBillDetail(const std::string& billId);
    void showBookingDetail(const std::string& bookingId);
    // Hàm khởi tạo UI cho trang History (nếu chưa làm bên Designer)
    void setupHistoryUI();
    bool m_isHistoryUiSetup = false;
    void setupStatsUI();
    bool m_isStatsUiSetup = false;
    void loadBillHistoryTable(const QString& search);    // Bảng Hóa đơn (Cũ)
    void loadBookingHistoryTable(const QString& search);
    // Hàm tính toán
    void calculateAndShowStats();

    // Helper parse file bill để lấy thông tin Pet ID
    void parseBillForStats(const std::string& billId,
                           long long& dogRev, long long& catRev,
                           QMap<QString, BreedStat>& breeds,
                           QMap<QString, CustomerStat>& customers);
    QLineEdit* m_profileNameInput = nullptr;
    QLineEdit* m_profilePassInput = nullptr;
    QLabel* m_profileRoleLabel = nullptr;
    bool m_isProfileUiSetup = false;

    // Hàm khởi tạo giao diện Profile
    void setupProfileUI();
    // Hàm đổ dữ liệu từ Admin vào Form
    void loadProfileData();
    // THÊM DÒNG NÀY:
    void on_petTypeFilter_currentIndexChanged(int index);


};

#endif
