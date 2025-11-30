#ifndef CLIENTDASHBOARD_H
#define CLIENTDASHBOARD_H

#include <QWidget>
#include <QDebug>
#include <QButtonGroup>

#include "domain/entities/Client.hpp"
#include "infra/repositories/PetRepoFile.hpp"
#include "infra/repositories/CartRepoFile.hpp"
#include "infra/repositories/ServiceRepoFile.hpp"
#include "infra/repositories/BookingRepoFile.hpp"
#include "infra/repositories/BillRepoFile.hpp"
#include "domain/entities/Bill.hpp"
#include "domain/entities/Bill_Item.hpp"
#include "infra/repositories/AccountRepoFile.hpp"
#include <QSet>
namespace Ui {
class ClientDashboard;
}

class ClientDashboard : public QWidget
{
    Q_OBJECT

public:
    // Constructor
    explicit ClientDashboard(Client* client,
                             AccountRepository* accountRepo,
                             PetRepository* petRepo,
                             CartRepository* cartRepo,
                             ServiceRepository* serviceRepo,
                             BookingRepository* bookingRepo,
                             BillRepository* billRepo,
                             QWidget *parent = nullptr);
    ~ClientDashboard();
signals:
    void logoutSignal();
private slots:
    void handlePetsButtonClick();
    void handleSpaButtonClick();
    void handleBookingsButtonClick();
    void handleCartButtonClick();
    void handleHistoryButtonClick();
    void handleProfileButtonClick();
    void handleCallButtonClick();
    void handleLogoutButtonClick();
    void handleRemoveFromCart();
    void handleBuy();
    void handleCancelBookingClick();

    void on_dogToggleButton_clicked();
    void on_catToggleButton_clicked();
    void on_searchLineEdit_textChanged(const QString &text);
    void handleBookSpaClick();
    void on_purchaseHistoryButton_clicked();
    void on_bookingHistoryButton_clicked();
    void on_editProfileButton_clicked();
    void on_saveProfileButton_clicked();
    void on_cancelProfileButton_clicked();

void on_addToCartButton_clicked();

private:
    Ui::ClientDashboard *ui;
    void setupHelpPage();


    Client* m_currentClient;
    PetRepository* m_petRepo;
    CartRepository* m_cartRepo;
    ServiceRepository* m_serviceRepo;
    BookingRepository* m_bookingRepo;
    BillRepository* m_billRepo;
    AccountRepository* m_accountRepo;
    void loadCartDataToTable();

    void updatePetsTable();
    void loadSpaDataToTable();
    void loadBookingsDataToTable();
    void loadPurchaseHistory();
    void loadBookingHistory();
    void loadProfileData();

    QButtonGroup* m_petTypeGroup;
    QButtonGroup* m_historyGroup;
    QButtonGroup* m_navGroup;
    QString m_currentPetTypeFilter;
    QString m_currentSearchKeyword;

};

#endif
