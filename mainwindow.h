// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "domain/services/AuthService.hpp"
#include "infra/repositories/AccountRepoFile.hpp"

#include "infra/repositories/CartRepoFile.hpp"
#include "infra/repositories/PetRepoFile.hpp"
#include "domain/entities/Client.hpp"
#include "staffdashboard.h"
#include "infra/repositories/ServiceRepoFile.hpp"
#include "infra/repositories/BookingRepoFile.hpp"
#include "infra/repositories/BillRepoFile.hpp"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:

  void on_loginButton_clicked();
    void on_goToCreateAccountButton_clicked();
    void on_backToLoginButton_clicked();
    void on_registerButton_clicked();

private:
  Ui::MainWindow *ui;
  AccountRepository *m_accountRepo;
  AuthService *m_authService;
  PetRepository *m_petRepo;
  CartRepository* m_cartRepo;
  ServiceRepository* m_serviceRepo;
  BookingRepository* m_bookingRepo;
  BillRepository* m_billRepo;

};
#endif
