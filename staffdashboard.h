#ifndef STAFFDASHBOARD_H
#define STAFFDASHBOARD_H

#include <QWidget>
#include <QButtonGroup>
#include "domain/entities/Staff.hpp"

#include "infra/repositories/AccountRepoFile.hpp"
#include "infra/repositories/PetRepoFile.hpp"
#include "infra/repositories/CartRepoFile.hpp"
#include "infra/repositories/ServiceRepoFile.hpp"
#include "infra/repositories/BookingRepoFile.hpp"
#include "infra/repositories/BillRepoFile.hpp"
#include <QTableWidgetItem>
namespace Ui {
class StaffDashboard;
}

class StaffDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit StaffDashboard(Staff* staff,
                            AccountRepository* accountRepo,
                            PetRepository* petRepo,
                            CartRepository* cartRepo,
                            ServiceRepository* serviceRepo,
                            BookingRepository* bookingRepo,
                            BillRepository* billRepo,
                            QWidget *parent = nullptr);
    ~StaffDashboard();

signals:
    void logoutSignal();

private slots:
    // Các nút điều hướng
    void handlePosPetsClick();
    void handlePosSpaClick();
    void handleManageBookingsClick();
    void handleHistoryClick();
    void handleCreateClientClick();
    void handleProfileClick();
    void handleLogoutClick();
    void on_saveClientButton_clicked();
    void on_posCheckButton_clicked();
    void on_posPayButton_clicked();
    void on_posPetsTable_itemChanged(QTableWidgetItem *item);
    void on_posDogButton_clicked();
    void on_posCatButton_clicked();
    void on_posSearchInput_textChanged(const QString &text);
    void on_spaCheckButton_clicked();
    void on_posSpaTable_itemChanged(QTableWidgetItem *item); // Tính tiền khi chọn dịch vụ
    void on_spaBookButton_clicked();
    void on_cancelBookingButton_clicked();
    void on_confirmBookingButton_clicked();
    void on_completeBookingButton_clicked();
    void on_historySearchButton_clicked();
    void on_editProfileButton_clicked();
    void on_saveProfileButton_clicked();
    void on_cancelProfileButton_clicked();
private:
    Ui::StaffDashboard *ui;

    Staff* m_currentStaff;
    Client* m_currentPosClient; // Lưu khách hàng đang được chọn để thanh toán
    void loadPosPetsTable();    // Hàm tải danh sách Pet lên bảng
    void calculatePosTotal();
    void loadHistoryTables(); // Tải dữ liệu cho cả 2 bảng
    void loadBookingHistoryTable(const std::string& phoneFilter = "");
    void loadSalesHistoryTable(const std::string& phoneFilter = "");
    void loadProfileData();
    // Lưu trữ các bộ não
    AccountRepository* m_accountRepo;
    PetRepository* m_petRepo;
    CartRepository* m_cartRepo;
    ServiceRepository* m_serviceRepo;
    BookingRepository* m_bookingRepo;
    BillRepository* m_billRepo;
    Client* m_currentSpaClient;       // Khách hàng đang đặt lịch
    QSet<QString> m_selectedServiceIds; // Danh sách ID dịch vụ đang chọn

    void loadSpaTable();              // Hàm tải danh sách dịch vụ lên bảng
    void calculateSpaTotal();
void loadBookingTables();
    QButtonGroup* m_navGroup;
    QButtonGroup* m_posTypeGroup;   // Nhóm nút Dog/Cat
    QString m_currentPosFilter;     // "Dog" hoặc "Cat"
    QString m_currentPosKeyword;
    QSet<QString> m_selectedPetIds;
};

#endif // STAFFDASHBOARD_H
