// mainwindow.cpp
#include "domain/entities/Client.hpp"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox> // Thêm thư viện này để hiển thị thông báo
#include <exception>   // Để bắt lỗi
#include "clientdashboard.h"
#include "admindashboard.h"
#include "domain/entities/Client.hpp"
#include "domain/entities/Dog.hpp"
#include "domain/entities/Cat.hpp"

#include <QDebug>
#include <QDir>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_accountRepo(nullptr)
    , m_authService(nullptr)
    , m_petRepo(nullptr)
    , m_cartRepo(nullptr)

    , m_serviceRepo(nullptr)
    , m_bookingRepo(nullptr)
    , m_billRepo(nullptr)
{
    ui->setupUi(this);
ui->mainStackedWidget->setCurrentWidget(ui->loginPage);
    qDebug() << "Thu muc lam viec hien tai (Working Dir):" << QDir::currentPath();
    qDebug() << "Dang tim Dog.txt tai (Looking for Dog.txt at):"
             << QDir::current().absoluteFilePath("data/Dog.txt");
    try {
        m_accountRepo = new AccountRepository(
            "data/AdminAccount.txt",
            "data/ClientAccount.txt",
            "data/StaffAccount.txt"
            );

        m_authService = new AuthService(m_accountRepo);
        m_petRepo = new PetRepository(
            "data/Dog.txt",
            "data/Cat.txt"
            );
        m_cartRepo = new CartRepository(m_petRepo, "data/Cart/");
        m_billRepo = new BillRepository("data/bills/", "data/bill_summary.txt");
        m_serviceRepo = new ServiceRepository("data/services.txt");
        m_bookingRepo = new BookingRepository("data/bookings.txt");

    } catch (const std::exception& e) {

        QMessageBox::critical(this, "Initialization error",
                              QString("Unable to load data file: %1").arg(e.what()));
    }
    qDebug() << "[MainWindow] KIEM TRA: So luong Dog CO SAN (Available) trong Repo:"
             << m_petRepo->getAllDogInfoAvailable().getSize();
}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_authService;
    delete m_accountRepo;
    delete m_cartRepo;
    delete m_petRepo;
    delete m_serviceRepo;
    delete m_bookingRepo;
}

void MainWindow::on_loginButton_clicked()
{
    QString username_qstr = ui->usernameLineEdit->text();
    QString password_qstr = ui->passwordLineEdit->text();

    std::string username = username_qstr.toStdString();
    std::string password = password_qstr.toStdString();

    Account* userAccount = m_authService->validateLogin(username, password);


    if (userAccount != nullptr)
    {
        std::string userId = userAccount->getId();

        if (userId.length() == 10)
        {
            qDebug() << "Login successful. Creating ClientDashboard...";

            Client* client = dynamic_cast<Client*>(userAccount);

            if (client == nullptr) {
                QMessageBox::critical(this, "Fatal error", "Cannot force account type!");
                return;
            }

            ClientDashboard *clientDash = new ClientDashboard(client,
                                                              m_accountRepo, // <-- TRUYỀN VÀO ĐÂY
                                                              m_petRepo,
                                                              m_cartRepo,
                                                              m_serviceRepo,
                                                              m_bookingRepo,
                                                              m_billRepo,
                                                              nullptr);

            clientDash->setAttribute(Qt::WA_DeleteOnClose);
            connect(clientDash, &ClientDashboard::logoutSignal, this, &MainWindow::show);
            clientDash->resize(800, 600);
            clientDash->show();

            this->hide();
        }

        else if (userId.length() == 3)
        {
            Admin* admin = dynamic_cast<Admin*>(userAccount);
            if (admin) {
                AdminDashboard *adminDash = new AdminDashboard(admin,
                                                               m_accountRepo,
                                                               m_petRepo,
                                                               m_cartRepo,
                                                               m_serviceRepo,
                                                               m_bookingRepo,
                                                               m_billRepo,
                                                               nullptr);

                adminDash->setAttribute(Qt::WA_DeleteOnClose);
                connect(adminDash, &AdminDashboard::logoutSignal, this, &MainWindow::show);

                adminDash->resize(1100, 750);
                adminDash->show();
                this->hide();
            }
        }
        else if (userId.length() == 5)
        {
            Staff* staff = dynamic_cast<Staff*>(userAccount);
            if (staff) {
                StaffDashboard *staffDash = new StaffDashboard(staff,
                                                               m_accountRepo,
                                                               m_petRepo,
                                                               m_cartRepo,
                                                               m_serviceRepo,
                                                               m_bookingRepo,
                                                               m_billRepo,
                                                               nullptr);

                staffDash->setAttribute(Qt::WA_DeleteOnClose);
                connect(staffDash, &StaffDashboard::logoutSignal, this, &MainWindow::show);

                staffDash->resize(1000, 700);
                staffDash->show();
                this->hide();
            }
        }
        else
        {
            QMessageBox::warning(nullptr, "Error", "Account type could not be determined.");
            this->show();
        }

    } else {
        QMessageBox::warning(this, "Login failed",
                             "Incorrect username or password. Please try again.");
        ui->passwordLineEdit->clear();
    }
}
void MainWindow::on_goToCreateAccountButton_clicked()
{
    ui->mainStackedWidget->setCurrentWidget(ui->createAccountPage);
}

void MainWindow::on_backToLoginButton_clicked()
{
    ui->mainStackedWidget->setCurrentWidget(ui->loginPage);

    ui->idLineEdit_create->clear();
    ui->passwordLineEdit_create->clear();
    ui->nameLineEdit_create->clear();
    ui->streetLineEdit_create->clear();
    ui->cityLineEdit_create->clear();
}

void MainWindow::on_registerButton_clicked()
{
    std::string id = ui->idLineEdit_create->text().toStdString();
    std::string password = ui->passwordLineEdit_create->text().toStdString();
    std::string name = ui->nameLineEdit_create->text().toStdString();
    std::string street = ui->streetLineEdit_create->text().toStdString();
    std::string city = ui->cityLineEdit_create->text().toStdString();

    std::string gender;
    if (ui->maleRadioButton->isChecked()) {
        gender = "male";
    } else if (ui->femaleRadioButton->isChecked()) {
        gender = "female";
    }

    if (id.empty() || password.empty() || name.empty() || street.empty() || city.empty() || gender.empty()) {
        QMessageBox::warning(this, "Registration Error", "Please fill in all fields.");
        return;
    }

    if (id.length() != 10) {
        QMessageBox::warning(this, "Registration Error", "ID (Phone Number) must be exactly 10 characters.");
        return;
    }

    if (m_accountRepo->isValidId(id)) {
        QMessageBox::warning(this, "Registration Error", "This ID (Phone Number) is already in use. Please select another ID.");
        return;
    }

    try {
        Client newClient(id, name, password, gender, street, city);

        m_accountRepo->setClientInfo(newClient);

        QMessageBox::information(this, "Success", "Account created successfully! Please log in.");
        on_backToLoginButton_clicked();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "System Error", QString("Error saving account: %1").arg(e.what()));
    }
}
