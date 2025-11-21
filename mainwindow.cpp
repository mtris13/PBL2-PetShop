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
    , m_accountRepo(nullptr) // Khởi tạo con trỏ là nullptr
    , m_authService(nullptr)
    , m_petRepo(nullptr)
    , m_cartRepo(nullptr)

    , m_serviceRepo(nullptr) // <-- THÊM MỚI
    , m_bookingRepo(nullptr)
    , m_billRepo(nullptr)
{
    ui->setupUi(this);
ui->mainStackedWidget->setCurrentWidget(ui->loginPage);
    qDebug() << "Thu muc lam viec hien tai (Working Dir):" << QDir::currentPath();
    qDebug() << "Dang tim Dog.txt tai (Looking for Dog.txt at):"
             << QDir::current().absoluteFilePath("data/Dog.txt");
    // --- KHỞI TẠO BACKEND ---
    try {
        // 1. Tạo Repository (Lớp thật)
        // LƯU Ý: Đường dẫn "data/" là tương đối với file thực thi
        // (thường nằm trong thư mục build/...)
        // Bạn có thể cần copy thư mục "data/" vào thư mục build
        m_accountRepo = new AccountRepository(
            "data/AdminAccount.txt",   // <-- Sửa lại 1
            "data/ClientAccount.txt",  // <-- Sửa lại 2
            "data/StaffAccount.txt"
            );

        // 2. "Tiêm" Repo vào Service
        m_authService = new AuthService(m_accountRepo);
        m_petRepo = new PetRepository(
            "data/Dog.txt",
            "data/Cat.txt"
            );
        m_cartRepo = new CartRepository(m_petRepo, "data/Cart/");
        // Sửa dòng khởi tạo BillRepo
        m_billRepo = new BillRepository("data/bills/", "data/bill_summary.txt");
        m_serviceRepo = new ServiceRepository("data/services.txt");
        m_bookingRepo = new BookingRepository("data/bookings.txt");
        // (Khởi tạo các repo và service khác ở đây...)

    } catch (const std::exception& e) {
        // Hiển thị lỗi nghiêm trọng nếu không thể tải file .txt
        QMessageBox::critical(this, "Lỗi khởi tạo",
                              QString("Không thể tải file dữ liệu: %1").arg(e.what()));
    }
    qDebug() << "[MainWindow] KIEM TRA: So luong Dog CO SAN (Available) trong Repo:"
             << m_petRepo->getAllDogInfoAvailable().getSize(); // <-- Lỗi ở 'getAllDogInfo'
}

MainWindow::~MainWindow()
{
    delete ui;

    // Giải phóng bộ nhớ (rất quan trọng!)
    delete m_authService;
    delete m_accountRepo;
    delete m_cartRepo;
    delete m_petRepo;
    delete m_serviceRepo;
    delete m_bookingRepo;
    // (delete các đối tượng khác ở đây)
}

// --- HÀNH ĐỘNG KHI NHẤN NÚT ---
void MainWindow::on_loginButton_clicked()
{
    // ... (code lấy username, password) ...
    QString username_qstr = ui->usernameLineEdit->text();
    QString password_qstr = ui->passwordLineEdit->text();

    // 2. Chuyển đổi sang std::string
    std::string username = username_qstr.toStdString();
    std::string password = password_qstr.toStdString();
    // === KẾT THÚC THÊM LẠI ===

    // 3. Gọi hàm `validateLogin`
    // Bây giờ các biến 'username' và 'password' đã tồn tại
    Account* userAccount = m_authService->validateLogin(username, password);


    if (userAccount != nullptr)
    {
        std::string userId = userAccount->getId();

        if (userId.length() == 10) // 10 ký tự = Client
        {
            qDebug() << "Login successful. Creating ClientDashboard...";

            // --- THAY ĐỔI LỚN ---
            // 1. Ép kiểu (cast) Account* sang Client*
            Client* client = dynamic_cast<Client*>(userAccount);

            if (client == nullptr) {
                QMessageBox::critical(this, "Lỗi nghiêm trọng", "Không thể ép kiểu tài khoản!");
                return;
            }

            // 2. Gọi constructor MỚI và truyền "công cụ" vào
            //    Truyền client và m_petRepo
            ClientDashboard *clientDash = new ClientDashboard(client,
                                                              m_accountRepo, // <-- TRUYỀN VÀO ĐÂY
                                                              m_petRepo,
                                                              m_cartRepo,
                                                              m_serviceRepo,
                                                              m_bookingRepo,
                                                              m_billRepo,
                                                              nullptr);
            // --- KẾT THÚC THAY ĐỔI ---

            clientDash->setAttribute(Qt::WA_DeleteOnClose);
            connect(clientDash, &ClientDashboard::logoutSignal, this, &MainWindow::show);
            clientDash->resize(800, 600);
            clientDash->show();

            this->hide();
        }
        // ...
        else if (userId.length() == 3) // Admin
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
        else if (userId.length() == 5) // Staff
        {
            Staff* staff = dynamic_cast<Staff*>(userAccount);
            if (staff) {
                // Khởi tạo Staff Dashboard và truyền tất cả Repo vào
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
            // Trường hợp không xác định (dù hiếm)
            QMessageBox::warning(nullptr, "Lỗi", "Không thể xác định loại tài khoản.");
            this->show(); // Hiển thị lại cửa sổ login
        }

    } else {
        // Thất bại! (Giữ nguyên code cũ của bạn)
        QMessageBox::warning(this, "Đăng nhập thất bại",
                             "Sai tên đăng nhập hoặc mật khẩu. Vui lòng thử lại.");
        ui->passwordLineEdit->clear();
    }
}
void MainWindow::on_goToCreateAccountButton_clicked()
{
    // Ra lệnh cho Stacked Widget lật sang trang createAccountPage
    ui->mainStackedWidget->setCurrentWidget(ui->createAccountPage);
}

// Hàm này được gọi khi bấm nút "<- Quay lại Đăng nhập"
void MainWindow::on_backToLoginButton_clicked()
{
    // Ra lệnh cho Stacked Widget lật về trang loginPage
    ui->mainStackedWidget->setCurrentWidget(ui->loginPage);

    // (Tùy chọn: Xóa các trường đã nhập trên form tạo tài khoản)
    ui->idLineEdit_create->clear();
    ui->passwordLineEdit_create->clear();
    ui->nameLineEdit_create->clear();
    ui->streetLineEdit_create->clear();
    ui->cityLineEdit_create->clear();
}
// mainwindow.cpp (Tiếp tục thêm hàm này vào)

// Hàm này được gọi khi bấm nút "TẠO TÀI KHOẢN"
void MainWindow::on_registerButton_clicked()
{
    // 1. Lấy dữ liệu từ các QLineEdit (với tên _create)
    std::string id = ui->idLineEdit_create->text().toStdString();
    std::string password = ui->passwordLineEdit_create->text().toStdString();
    std::string name = ui->nameLineEdit_create->text().toStdString();
    std::string street = ui->streetLineEdit_create->text().toStdString();
    std::string city = ui->cityLineEdit_create->text().toStdString();

    // 2. Lấy dữ liệu từ QRadioButton
    std::string gender;
    if (ui->maleRadioButton->isChecked()) {
        gender = "male"; // (Hoặc "Nam", tùy bạn muốn lưu)
    } else if (ui->femaleRadioButton->isChecked()) {
        gender = "female"; // (Hoặc "Nữ")
    }

    // 3. Kiểm tra dữ liệu (Validation)
    if (id.empty() || password.empty() || name.empty() || street.empty() || city.empty() || gender.empty()) {
        QMessageBox::warning(this, "Lỗi Đăng Ký", "Vui lòng điền đầy đủ tất cả các trường.");
        return; // Dừng lại
    }

    if (id.length() != 10) { // Giả định Client ID là 10 số
        QMessageBox::warning(this, "Lỗi Đăng Ký", "ID (Số điện thoại) phải có đúng 10 ký tự.");
        return; // Dừng lại
    }

    // 4. Kiểm tra xem ID đã tồn tại chưa (Rất quan trọng!)
    // (Chúng ta dùng lại hàm isValidId của AccountRepo)
    if (m_accountRepo->isValidId(id)) {
        QMessageBox::warning(this, "Lỗi Đăng Ký", "ID (Số điện thoại) này đã được sử dụng. Vui lòng chọn ID khác.");
        return; // Dừng lại
    }

    // 5. Nếu mọi thứ OK -> Gọi Backend để tạo
    try {
        // Tạo một đối tượng Client mới
        Client newClient(id, name, password, gender, street, city);

        // Dùng hàm setClientInfo để lưu (hàm này sẽ tự động thêm mới nếu ID chưa có)
        m_accountRepo->setClientInfo(newClient);

        // 6. Thông báo thành công và quay lại
        QMessageBox::information(this, "Thành Công", "Tạo tài khoản thành công! Vui lòng đăng nhập.");
        on_backToLoginButton_clicked(); // Tự động gọi hàm quay lại

    } catch (const std::exception& e) {
        // Bắt lỗi nếu file .txt không ghi được
        QMessageBox::critical(this, "Lỗi Hệ Thống", QString("Lỗi khi lưu tài khoản: %1").arg(e.what()));
    }
}
// --- KẾT THÚC THÊM CODE MỚI ---
