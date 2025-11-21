#include "admindashboard.h"
#include "ui_admindashboard.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
AdminDashboard::AdminDashboard(Admin* admin,
                               AccountRepository* accountRepo,
                               PetRepository* petRepo,
                               CartRepository* cartRepo,
                               ServiceRepository* serviceRepo,
                               BookingRepository* bookingRepo,
                               BillRepository* billRepo,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminDashboard),
    m_currentAdmin(admin),
    m_accountRepo(accountRepo),
    m_petRepo(petRepo),
    m_cartRepo(cartRepo),
    m_serviceRepo(serviceRepo),
    m_bookingRepo(bookingRepo),
    m_billRepo(billRepo)
{
    ui->setupUi(this);

    // 1. Hiển thị tên Admin
    ui->userNameLabel->setText(QString::fromStdString(m_currentAdmin->getName()));

    // 2. Nhóm các nút NavBar
    m_navGroup = new QButtonGroup(this);
    m_navGroup->addButton(ui->manageAccountsButton);
    m_navGroup->addButton(ui->managePetsButton);
    m_navGroup->addButton(ui->manageSpaButton);
    m_navGroup->addButton(ui->historyButton);
    m_navGroup->addButton(ui->statsButton);
    m_navGroup->addButton(ui->profileButton);
    m_navGroup->setExclusive(true);

    // 3. Kết nối nút bấm
    connect(ui->manageAccountsButton, &QPushButton::clicked, this, &AdminDashboard::handleManageAccountsClick);
    connect(ui->managePetsButton, &QPushButton::clicked, this, &AdminDashboard::handleManagePetsClick);
    connect(ui->manageSpaButton, &QPushButton::clicked, this, &AdminDashboard::handleManageSpaClick);
    connect(ui->historyButton, &QPushButton::clicked, this, &AdminDashboard::handleHistoryClick);
    connect(ui->statsButton, &QPushButton::clicked, this, &AdminDashboard::handleStatsClick);
    connect(ui->profileButton, &QPushButton::clicked, this, &AdminDashboard::handleProfileClick);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminDashboard::handleLogoutClick);

    // Mặc định vào trang Quản lý tài khoản
    ui->manageAccountsButton->setChecked(true);
    ui->pageStackedWidget->setCurrentWidget(ui->manageAccountsPage);

    ui->accountsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->accountsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    loadAccountsTable();
}

AdminDashboard::~AdminDashboard()
{
    delete ui;
}

// --- CÁC HÀM CHUYỂN TRANG ---
void AdminDashboard::handleManageAccountsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->manageAccountsPage);
    ui->accStackedWidget->setCurrentWidget(ui->accListPage);
    // ------------------------

    // 3. Tải dữ liệu bảng
    loadAccountsTable();
}
void AdminDashboard::handleManagePetsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->managePetsPage);
    ui->petStackedWidget->setCurrentWidget(ui->petListPage);

    // 3. Tải dữ liệu lên bảng ngay lập tức
    loadPetsTable();
}
void AdminDashboard::handleManageSpaClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->manageSpaPage);

    // Reset về trang danh sách Spa
    ui->spaStackedWidget->setCurrentWidget(ui->spaListPage);

    // Tải dữ liệu Spa
    loadSpaTable();
}
void AdminDashboard::handleHistoryClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->historyPage);

    // Đảm bảo UI đã được tạo
    setupHistoryUI();

    // Tải dữ liệu
    loadHistoryTable("");
}
void AdminDashboard::handleStatsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->statsPage);
    setupStatsUI();
    // Tự động thống kê lần đầu (All time hoặc theo date mặc định)
    calculateAndShowStats();
}
void AdminDashboard::handleProfileClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->profilePage);
    setupProfileUI();
    loadProfileData(); // Tải dữ liệu hiện tại lên form
}
void AdminDashboard::handleLogoutClick() {
    emit logoutSignal();
    this->close();
}
// admindashboard.cpp

#include <QMessageBox>

// --- 1. HÀM TẢI DANH SÁCH TÀI KHOẢN ---
void AdminDashboard::loadAccountsTable(const QString& search)
{
    ui->accountsTable->setRowCount(0);
    ui->accountsTable->setColumnCount(5);
    QStringList headers = {"ID", "Họ tên", "Mật khẩu", "Giới tính", "Vai trò"};
    ui->accountsTable->setHorizontalHeaderLabels(headers);

    int row = 0;
    std::string keyword = search.toLower().toStdString();

    // A. Load STAFF trước
    LinkedList<Staff> staffs = m_accountRepo->getAllStaffInfo();
    Node<Staff>* sNode = staffs.getHead();
    while(sNode != nullptr) {
        Staff s = sNode->getData();

        // Tìm kiếm
        string id = s.getId();
        string name = s.getName();
        // Chuyển thường để so sánh
        string lowerId = id; std::transform(lowerId.begin(), lowerId.end(), lowerId.begin(), ::tolower);
        string lowerName = name; std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (search.isEmpty() || lowerId.find(keyword) != string::npos || lowerName.find(keyword) != string::npos) {
            ui->accountsTable->insertRow(row);
            ui->accountsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(id)));
            ui->accountsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(name)));
            ui->accountsTable->setItem(row, 2, new QTableWidgetItem("********")); // Ẩn mật khẩu
            ui->accountsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(s.getGender())));

            QTableWidgetItem* roleItem = new QTableWidgetItem("Staff");
            roleItem->setForeground(QBrush(QColor("blue")));
            roleItem->setFont(QFont("Arial", 9, QFont::Bold));
            ui->accountsTable->setItem(row, 4, roleItem);
            row++;
        }
        sNode = sNode->getNext();
    }

    // B. Load CLIENT sau
    LinkedList<Client> clients = m_accountRepo->getAllClientInfo();
    Node<Client>* cNode = clients.getHead();
    while(cNode != nullptr) {
        Client c = cNode->getData();
        // (Logic tìm kiếm tương tự như trên...)
        string id = c.getId();
        string name = c.getName();
        string lowerId = id; std::transform(lowerId.begin(), lowerId.end(), lowerId.begin(), ::tolower);
        string lowerName = name; std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (search.isEmpty() || lowerId.find(keyword) != string::npos || lowerName.find(keyword) != string::npos) {
            ui->accountsTable->insertRow(row);
            ui->accountsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(id)));
            ui->accountsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(name)));
            ui->accountsTable->setItem(row, 2, new QTableWidgetItem("********"));
            ui->accountsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(c.getGender())));
            ui->accountsTable->setItem(row, 4, new QTableWidgetItem("Client"));
            row++;
        }
        cNode = cNode->getNext();
    }
    ui->accountsTable->resizeColumnsToContents();
}

// --- 2. CÁC NÚT CHỨC NĂNG ---

void AdminDashboard::on_accSearchButton_clicked() {
    loadAccountsTable(ui->accSearchInput->text());
}

void AdminDashboard::on_addStaffButton_clicked() {
    // Reset form
    ui->accIdInput->clear(); ui->accIdInput->setReadOnly(false); // Cho phép nhập ID mới
    ui->accNameInput->clear();
    ui->accPassInput->clear();
    ui->maleRadio->setChecked(true);
    ui->accSalaryInput->clear(); ui->accSalaryInput->setEnabled(true); // Staff có lương

    ui->accStackedWidget->setCurrentWidget(ui->accFormPage);
}

void AdminDashboard::on_viewAccButton_clicked() {
    int row = ui->accountsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn một tài khoản để xem.");
        return;
    }

    // Lấy ID từ bảng
    std::string id = ui->accountsTable->item(row, 0)->text().toStdString();
    std::string role = ui->accountsTable->item(row, 4)->text().toStdString();

    // Tải thông tin chi tiết lên Form
    if (role == "Staff") {
        Staff s = m_accountRepo->getStaffInfo(id);
        ui->accIdInput->setText(QString::fromStdString(s.getId()));
        ui->accNameInput->setText(QString::fromStdString(s.getName()));
        ui->accPassInput->setText(QString::fromStdString(s.getPassword())); // Hiện pass thật
        if(s.getGender() == "Male") ui->maleRadio->setChecked(true); else ui->femaleRadio->setChecked(true);
        ui->accSalaryInput->setText(QString::number(s.getSalary()));
        ui->accSalaryInput->setEnabled(true);
    } else { // Client
        Client c = m_accountRepo->getClientInfo(id);
        ui->accIdInput->setText(QString::fromStdString(c.getId()));
        ui->accNameInput->setText(QString::fromStdString(c.getName()));
        ui->accPassInput->setText(QString::fromStdString(c.getPassword()));
        if(c.getGender() == "Male") ui->maleRadio->setChecked(true); else ui->femaleRadio->setChecked(true);
        ui->accSalaryInput->setText("");
        ui->accSalaryInput->setEnabled(false); // Client không có lương
    }

    ui->accIdInput->setReadOnly(true); // Không cho sửa ID khi Edit
    ui->accStackedWidget->setCurrentWidget(ui->accFormPage);
}

void AdminDashboard::on_deleteAccButton_clicked() {
    int row = ui->accountsTable->currentRow();
    if (row < 0) return;

    std::string id = ui->accountsTable->item(row, 0)->text().toStdString();
    std::string name = ui->accountsTable->item(row, 1)->text().toStdString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa",
                                  QString("Bạn có chắc muốn xóa tài khoản: %1 (%2)?\nDữ liệu sẽ được sao lưu vào folder backup.").arg(QString::fromStdString(name), QString::fromStdString(id)),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_accountRepo->deleteAccount(id); // Hàm này giờ đã có Backup!
        loadAccountsTable(); // Tải lại bảng
        QMessageBox::information(this, "Thành công", "Đã xóa tài khoản thành công.");
    }
}

void AdminDashboard::on_saveAccButton_clicked() {
    // Lấy dữ liệu
    std::string id = ui->accIdInput->text().toStdString();
    std::string name = ui->accNameInput->text().toStdString();
    std::string pass = ui->accPassInput->text().toStdString();
    std::string gender = ui->maleRadio->isChecked() ? "Male" : "Female";

    if (id.empty() || name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập đủ ID, Tên và Mật khẩu.");
        return;
    }

    //5 ký tự -> Staff, 10 ký tự -> Client
    if (id.length() == 5) {
        long salary = ui->accSalaryInput->text().toLong();
        Staff s(id, name, pass, gender, salary);
        m_accountRepo->setStaffInfo(s); // Thêm mới hoặc Cập nhật
    }
    else if (id.length() == 10) {
        // Client
        Client oldC = m_accountRepo->getClientInfo(id);
        Client newC(id, name, pass, gender, oldC.getStreet(), oldC.getCity());
        m_accountRepo->setClientInfo(newC);
    }
    else {
        QMessageBox::warning(this, "Sai ID", "ID Staff phải 5 ký tự, Client phải 10 ký tự.");
        return;
    }

    QMessageBox::information(this, "Thành công", "Đã lưu thông tin tài khoản.");
    ui->accStackedWidget->setCurrentWidget(ui->accListPage); // Quay về danh sách
    loadAccountsTable();
}

void AdminDashboard::on_cancelAccButton_clicked() {
    ui->accStackedWidget->setCurrentWidget(ui->accListPage);
}
void AdminDashboard::backupData(const QString& folderName, const QString& id, const QString& content) {
    // Tạo đường dẫn: BackupData/Pets hoặc BackupData/Spa
    QString path = "BackupData/" + folderName;
    QDir dir;
    if (!dir.exists(path)) {
        dir.mkpath(path);
    }

    // Tạo tên file có timestamp để không bị trùng
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString fileName = path + "/Deleted_" + id + "_" + timestamp + ".txt";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "--- BACKUP DELETED DATA ---\n";
        out << "Deleted Time: " << QDateTime::currentDateTime().toString() << "\n";
        out << "ID: " << id << "\n";
        out << "Content:\n" << content;
        file.close();
    }
}


void AdminDashboard::loadPetsTable(const QString& search) {
    ui->petsTable->setRowCount(0);
    // Cột 7 là cột ẩn để lưu loại "dog" hay "cat" giúp việc Sửa/Xóa dễ dàng
    ui->petsTable->setColumnCount(8);
    QStringList headers = {"ID", "Name", "Type", "Breed", "Price (VND)", "Status", "Characteristic", "HiddenType"};
    ui->petsTable->setHorizontalHeaderLabels(headers);
    ui->petsTable->setColumnHidden(7, true); // Ẩn cột loại đi

    int row = 0;
    std::string keyword = search.toLower().toStdString();

    // Load Dogs
    LinkedList<Dog> dogs = m_petRepo->searchDog("all", keyword);
    Node<Dog>* dNode = dogs.getHead();
    while(dNode != nullptr) {
        Dog p = dNode->getData();
        ui->petsTable->insertRow(row);
        ui->petsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p.getId())));
        ui->petsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.getName())));
        ui->petsTable->setItem(row, 2, new QTableWidgetItem("Dog"));
        ui->petsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p.getBreed())));
        ui->petsTable->setItem(row, 4, new QTableWidgetItem(QString::number(p.getPrice())));


        bool isAvailable = (p.getStatus() == "1" || p.getStatus() == "available");

        QString statusStr = isAvailable ? "Available" : "Sold";
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusStr);

        if(isAvailable) statusItem->setForeground(Qt::green);
        else statusItem->setForeground(Qt::red);

        ui->petsTable->setItem(row, 5, statusItem);
        // ---------------------------

        ui->petsTable->setItem(row, 6, new QTableWidgetItem("Energy: " + QString::number(p.getEnergyLevel()) + "/10"));
        ui->petsTable->setItem(row, 7, new QTableWidgetItem("dog")); // Đánh dấu là chó
        row++;
        dNode = dNode->getNext();
    }

    // Load Cats
    LinkedList<Cat> cats = m_petRepo->searchCat("all", keyword);
    Node<Cat>* cNode = cats.getHead();
    while(cNode != nullptr) {
        Cat p = cNode->getData();
        ui->petsTable->insertRow(row);
        ui->petsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p.getId())));
        ui->petsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.getName())));
        ui->petsTable->setItem(row, 2, new QTableWidgetItem("Cat"));
        ui->petsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p.getBreed())));
        ui->petsTable->setItem(row, 4, new QTableWidgetItem(QString::number(p.getPrice())));

        bool isAvailable = (p.getStatus() == "1" || p.getStatus() == "available");

        QString statusStr = isAvailable ? "Available" : "Sold";
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusStr);

        if(isAvailable) statusItem->setForeground(Qt::green);
        else statusItem->setForeground(Qt::red);

        ui->petsTable->setItem(row, 5, statusItem);
        // ---------------------------

        ui->petsTable->setItem(row, 6, new QTableWidgetItem("Fur: " + QString::fromStdString(p.getFurLength())));
        ui->petsTable->setItem(row, 7, new QTableWidgetItem("cat")); // Đánh dấu là mèo
        row++;
        cNode = cNode->getNext();
    }
    ui->petsTable->resizeColumnsToContents();
}


void AdminDashboard::on_petSearchButton_clicked() {
    loadPetsTable(ui->petSearchInput->text());
}


void AdminDashboard::on_dogRadio_toggled(bool checked) {
    // Nếu chọn Dog -> Hiện Energy, Ẩn Fur. Ngược lại cho Cat.
    ui->dogEnergyInput->setVisible(checked);
    ui->catFurInput->setVisible(!checked);


    if (!m_isEditMode) {
        std::string type = checked ? "dog" : "cat";
        // Gọi hàm sinh ID từ Repo
        std::string newId = m_petRepo->generatePetId(type);
        ui->petIdInput->setText(QString::fromStdString(newId));
    }
}


void AdminDashboard::on_addPetButton_clicked() {
    m_isEditMode = false; // Chế độ thêm mới
    ui->petStackedWidget->setCurrentWidget(ui->petFormPage);

    // Reset Form
    ui->petNameInput->clear();
    ui->petBreedInput->clear();
    ui->petAgeInput->setValue(1);
    ui->petPriceInput->clear();
    ui->petDescInput->clear();
    ui->dogEnergyInput->setValue(5);
    ui->catFurInput->clear();

    // Cho phép chọn loại thú cưng
    ui->dogRadio->setEnabled(true);
    ui->catRadio->setEnabled(true);

    // Trigger logic sinh ID
    if (ui->dogRadio->isChecked()) {
        on_dogRadio_toggled(true);
    } else {
        ui->dogRadio->setChecked(true); // Mặc định chọn Dog
    }
}


void AdminDashboard::on_editPetButton_clicked() {
    int row = ui->petsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn thú cưng cần sửa.");
        return;
    }

    m_isEditMode = true; // Chế độ sửa
    QString id = ui->petsTable->item(row, 0)->text();
    QString type = ui->petsTable->item(row, 7)->text(); // Lấy loại từ cột ẩn
    m_editingId = id.toStdString();

    ui->petStackedWidget->setCurrentWidget(ui->petFormPage);

    // Không cho đổi loại thú cưng khi đang sửa
    ui->dogRadio->setEnabled(false);
    ui->catRadio->setEnabled(false);

    // Đổ dữ liệu cũ vào Form
    if (type == "dog") {
        ui->dogRadio->setChecked(true);
        Dog d = m_petRepo->getDogInfo(m_editingId);
        ui->petNameInput->setText(QString::fromStdString(d.getName()));
        ui->petBreedInput->setText(QString::fromStdString(d.getBreed()));
        ui->petAgeInput->setValue(d.getAge());
        ui->petPriceInput->setText(QString::number(d.getPrice()));
        ui->petDescInput->setText(QString::fromStdString(d.getDescription()));
        ui->dogEnergyInput->setValue(d.getEnergyLevel());
    } else {
        ui->catRadio->setChecked(true);
        Cat c = m_petRepo->getCatInfo(m_editingId);
        ui->petNameInput->setText(QString::fromStdString(c.getName()));
        ui->petBreedInput->setText(QString::fromStdString(c.getBreed()));
        ui->petAgeInput->setValue(c.getAge());
        ui->petPriceInput->setText(QString::number(c.getPrice()));
        ui->petDescInput->setText(QString::fromStdString(c.getDescription()));
        ui->catFurInput->setText(QString::fromStdString(c.getFurLength()));
    }
    ui->petIdInput->setText(id); // Hiển thị ID nhưng ReadOnly
}


void AdminDashboard::on_savePetButton_clicked() {
    // Validate dữ liệu
    if (ui->petNameInput->text().isEmpty() || ui->petPriceInput->text().isEmpty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập Tên và Giá.");
        return;
    }

    std::string id = ui->petIdInput->text().toStdString();
    std::string name = ui->petNameInput->text().toStdString();
    std::string breed = ui->petBreedInput->text().toStdString();
    int age = ui->petAgeInput->value();
    long price = ui->petPriceInput->text().toLong();
    std::string desc = ui->petDescInput->text().toStdString();
    if (desc.empty()) desc = "No description";

    bool status = true;

    if (ui->dogRadio->isChecked()) {
        int energy = ui->dogEnergyInput->value();
        Dog d(id, name, breed, age, price, status, energy, desc);
        m_petRepo->setDogInfo(d); // Repo sẽ tự xử lý ghi đè nếu ID trùng
    } else {
        std::string fur = ui->catFurInput->text().toStdString();
        Cat c(id, name, breed, age, price, status, fur, desc);
        m_petRepo->setCatInfo(c);
    }

    QMessageBox::information(this, "Thành công", "Đã lưu thông tin thú cưng.");
    ui->petStackedWidget->setCurrentWidget(ui->petListPage); // Quay về bảng
    loadPetsTable(); // Tải lại bảng
}

// 7. Nút Xóa (DELETE - Có Backup)
void AdminDashboard::on_deletePetButton_clicked() {
    int row = ui->petsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn thú cưng để xóa.");
        return;
    }

    QString id = ui->petsTable->item(row, 0)->text();
    QString name = ui->petsTable->item(row, 1)->text();
    QString type = ui->petsTable->item(row, 7)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa",
                                  "Bạn có chắc muốn xóa: " + name + " (" + id + ")?\nDữ liệu sẽ được sao lưu.",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 1. Lấy nội dung để backup
        QString contentBackup;
        if (type == "dog") {
            Dog d = m_petRepo->getDogInfo(id.toStdString());
            // Format chuỗi tùy ý để dễ đọc
            contentBackup = QString("Type: Dog\nName: %1\nBreed: %2\nPrice: %3\nDesc: %4")
                                .arg(QString::fromStdString(d.getName()))
                                .arg(QString::fromStdString(d.getBreed()))
                                .arg(d.getPrice())
                                .arg(QString::fromStdString(d.getDescription()));
        } else {
            Cat c = m_petRepo->getCatInfo(id.toStdString());
            contentBackup = QString("Type: Cat\nName: %1\nBreed: %2\nPrice: %3\nDesc: %4")
                                .arg(QString::fromStdString(c.getName()))
                                .arg(QString::fromStdString(c.getBreed()))
                                .arg(c.getPrice())
                                .arg(QString::fromStdString(c.getDescription()));
        }

        // 2. Ghi file Backup
        backupData("Pets", id, contentBackup);

        // 3. Xóa trong Repo
        if (type == "dog") m_petRepo->deletePet(id.toStdString()); // Hàm deletePet dùng chung ID
        else m_petRepo->deletePet(id.toStdString());

        loadPetsTable();
        QMessageBox::information(this, "Đã xóa", "Đã xóa và sao lưu dữ liệu vào thư mục BackupData/Pets.");
    }
}

void AdminDashboard::on_cancelPetButton_clicked() {
    ui->petStackedWidget->setCurrentWidget(ui->petListPage);
}



void AdminDashboard::loadSpaTable(const QString& search) {
    ui->spaTable->setRowCount(0);
    ui->spaTable->setColumnCount(5);
    ui->spaTable->setHorizontalHeaderLabels({"ID", "Name", "Description", "Price (VND)", "Time"});


    LinkedList<Service> services = m_serviceRepo->getAllServices();
    Node<Service>* node = services.getHead();

    int row = 0;
    std::string keyword = search.toLower().toStdString();

    while(node != nullptr) {
        Service s = node->getData();
        std::string name = s.getName();
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        // Logic tìm kiếm theo tên
        if (keyword.empty() || name.find(keyword) != std::string::npos) {
            ui->spaTable->insertRow(row);
            ui->spaTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(s.getId())));
            ui->spaTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s.getName())));
            ui->spaTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getDescription())));
            ui->spaTable->setItem(row, 3, new QTableWidgetItem(QString::number(s.getPrice())));
            ui->spaTable->setItem(row, 4, new QTableWidgetItem(QString::number(s.getDuration()) + " mins"));
            row++;
        }
        node = node->getNext();
    }
    ui->spaTable->resizeColumnsToContents();
}

void AdminDashboard::on_spaSearchButton_clicked() {
    loadSpaTable(ui->spaSearchInput->text());
}

// 2. Nút Thêm
void AdminDashboard::on_addServiceButton_clicked() {
    ui->spaStackedWidget->setCurrentWidget(ui->spaFormPage);
    // Reset
    ui->spaIdInput->clear();
    ui->spaIdInput->setReadOnly(false); // Cho nhập ID mới
    ui->spaNameInput->clear();
    ui->spaDescInput->clear();
    ui->spaPriceInput->clear();
    ui->spaDurationInput->setValue(60);
}

// 3. Nút Sửa
void AdminDashboard::on_editServiceButton_clicked() {
    int row = ui->spaTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn dịch vụ để sửa.");
        return;
    }
    QString id = ui->spaTable->item(row, 0)->text();

    Service s = m_serviceRepo->getServiceInfo(id.toStdString());

    ui->spaStackedWidget->setCurrentWidget(ui->spaFormPage);
    ui->spaIdInput->setText(QString::fromStdString(s.getId()));
    ui->spaIdInput->setReadOnly(true); // Không cho sửa ID
    ui->spaNameInput->setText(QString::fromStdString(s.getName()));
    ui->spaDescInput->setText(QString::fromStdString(s.getDescription()));
    ui->spaPriceInput->setText(QString::number(s.getPrice()));
    ui->spaDurationInput->setValue(s.getDuration());
}



void AdminDashboard::on_saveServiceButton_clicked() {
    std::string id = ui->spaIdInput->text().toStdString();
    std::string name = ui->spaNameInput->text().toStdString();
    std::string desc = ui->spaDescInput->text().toStdString();
    long price = ui->spaPriceInput->text().toLong();
    int duration = ui->spaDurationInput->value();

    if (id.empty() || name.empty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập Mã và Tên dịch vụ.");
        return;
    }


    if (!m_isEditMode && m_serviceRepo->isValidServiceId(id)) {
        QMessageBox::warning(this, "Trùng mã",
                             "Mã dịch vụ '" + QString::fromStdString(id) + "' đã tồn tại!\nVui lòng chọn mã khác.");
        return; // Dừng lại, không lưu đè
    }
    // ----------------------------------

    Service s(id, name, desc, price, duration);
    m_serviceRepo->setServiceInfo(s); // Hàm này giờ đã cập nhật RAM -> Bảng sẽ hiện ngay

    QMessageBox::information(this, "Thành công", "Đã lưu dịch vụ spa.");
    ui->spaStackedWidget->setCurrentWidget(ui->spaListPage);
    loadSpaTable(); // Tải lại bảng từ RAM (đã có dữ liệu mới)
}

// 5. Nút Xóa (Backup)
void AdminDashboard::on_deleteServiceButton_clicked() {
    int row = ui->spaTable->currentRow();
    if (row < 0) return;
    QString id = ui->spaTable->item(row, 0)->text();

    if (QMessageBox::question(this, "Xóa", "Xóa dịch vụ " + id + "?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        // Backup
        Service s = m_serviceRepo->getServiceInfo(id.toStdString());
        QString content = QString("Service: %1\nPrice: %2\nDesc: %3")
                              .arg(QString::fromStdString(s.getName()))
                              .arg(s.getPrice())
                              .arg(QString::fromStdString(s.getDescription()));

        backupData("Spa", id, content);

        // Xóa
        m_serviceRepo->deleteService(id.toStdString());
        loadSpaTable();
    }
}

void AdminDashboard::on_cancelServiceButton_clicked() {
    ui->spaStackedWidget->setCurrentWidget(ui->spaListPage);
}
void AdminDashboard::setupHistoryUI() {
    if (m_isHistoryUiSetup) return; // Chỉ chạy 1 lần

    if (ui->historyPage->layout() == nullptr) {
        QVBoxLayout* mainLayout = new QVBoxLayout(ui->historyPage);

        // A. Tạo thanh Toolbar (Ô tìm kiếm + Nút)
        QHBoxLayout* toolbarLayout = new QHBoxLayout();

        QLineEdit* searchInput = new QLineEdit(ui->historyPage);
        searchInput->setObjectName("historySearchInput"); // Đặt tên để tìm lại sau này
        searchInput->setPlaceholderText("Search by ID, name or date (dd/mm/yyyy)...");

        QPushButton* searchBtn = new QPushButton("Search", ui->historyPage);
        searchBtn->setObjectName("historySearchButton");
        searchBtn->setCursor(Qt::PointingHandCursor);
        // Style cho nút đẹp hơn chút
        searchBtn->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold; padding: 5px 15px; border-radius: 4px;");

        toolbarLayout->addWidget(searchInput);
        toolbarLayout->addWidget(searchBtn);

        // B. Tạo Bảng
        QTableWidget* table = new QTableWidget(ui->historyPage);
        table->setObjectName("historyTable");
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho sửa
        table->setAlternatingRowColors(true); // Màu dòng xen kẽ
        table->verticalHeader()->setVisible(false);

        mainLayout->addLayout(toolbarLayout);
        mainLayout->addWidget(table);

        // Kết nối sự kiện nút Tìm kiếm
        connect(searchBtn, &QPushButton::clicked, this, &AdminDashboard::on_historySearchButton_clicked);
        // Tìm kiếm khi nhấn Enter
        connect(searchInput, &QLineEdit::returnPressed, this, &AdminDashboard::on_historySearchButton_clicked);
    }

    m_isHistoryUiSetup = true;
}





void AdminDashboard::loadHistoryTable(const QString& search) {
    // Tìm widget table theo tên (vì ta tạo bằng code hoặc ui đều đặt tên này)
    QTableWidget* table = ui->historyPage->findChild<QTableWidget*>("historyTable");
    if (!table) return;

    table->setRowCount(0);
    table->setColumnCount(6); // ID, Khách, Ngày, Giờ, Tổng tiền, Xem
    QStringList headers = {"ID", "Customer", "Date", "Time", "Total (VND)", "Description"};
    table->setHorizontalHeaderLabels(headers);

    // Căn chỉnh cột cho đẹp
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Tên khách giãn hết cỡ

    // Lấy dữ liệu từ Repo
    LinkedList<Bill> bills = m_billRepo->getAllBills();
    Node<Bill>* node = bills.getHead();
    int row = 0;
    std::string keyword = search.toLower().toStdString();

    while(node != nullptr) {
        Bill b = node->getData();

        // Chuẩn bị dữ liệu để so sánh tìm kiếm
        std::string id = b.getBillId(); std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        std::string name = b.getClientName(); std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        std::string date = b.getDate();

        // Logic lọc: Nếu từ khóa rỗng HOẶC trùng ID/Tên/Ngày
        if (keyword.empty() ||
            id.find(keyword) != std::string::npos ||
            name.find(keyword) != std::string::npos ||
            date.find(keyword) != std::string::npos) {

            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBillId())));
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.getClientName())));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.getDate())));
            table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getTime())));
            table->setItem(row, 4, new QTableWidgetItem(QString::number(b.getTotalAmount())));

            // TẠO NÚT "XEM" TRONG BẢNG
            QPushButton* viewBtn = new QPushButton("View");
            viewBtn->setCursor(Qt::PointingHandCursor);
            // Style nút nhỏ gọn
            viewBtn->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 3px; padding: 2px 10px;");

            // Gắn hành động cho nút này
            // Dùng Lambda function để bắt giá trị ID của dòng hiện tại
            std::string billIdStr = b.getBillId();
            connect(viewBtn, &QPushButton::clicked, [this, billIdStr]() {
                showBillDetail(billIdStr);
            });

            table->setCellWidget(row, 5, viewBtn);
            row++;
        }
        node = node->getNext();
    }
}

// --- 4. SLOT NÚT TÌM KIẾM ---
void AdminDashboard::on_historySearchButton_clicked() {
    QLineEdit* input = ui->historyPage->findChild<QLineEdit*>("historySearchInput");
    if (input) {
        loadHistoryTable(input->text());
    }
}

// --- 5. HÀM HIỂN THỊ CHI TIẾT (ĐỌC FILE BILLxxx.txt) ---
void AdminDashboard::showBillDetail(const std::string& billId) {
    // Đường dẫn file
    QString filePath = "data/bills/" + QString::fromStdString(billId) + ".txt";

    // Nếu thư mục bills nằm cùng cấp với exe thì dùng:
    if (!QFile::exists(filePath)) {
        filePath = QString::fromStdString(billId) + ".txt"; // Thử tìm ở thư mục gốc
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy file chi tiết cho hóa đơn: " + QString::fromStdString(billId));
        return;
    }

    QTextStream in(&file);
// Set encoding UTF-8 để đọc tiếng Việt không lỗi
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    QString content = in.readAll();
    file.close();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Invoice detail: " + QString::fromStdString(billId));
    msgBox.setText(content);
    // Chỉnh font chữ dạng Monospace (như Courier) để các cột thẳng hàng
    msgBox.setStyleSheet("QLabel{min-width: 400px; font-family: 'Courier New'; font-size: 12px;}");
    msgBox.exec();
}

void AdminDashboard::setupStatsUI() {
    if (m_isStatsUiSetup) return;

    if (ui->statsPage->layout() == nullptr) {
        QVBoxLayout* mainLayout = new QVBoxLayout(ui->statsPage);
        mainLayout->setSpacing(20);

        // --- A. HEADER & DATE FILTER ---
        QHBoxLayout* filterLayout = new QHBoxLayout();

        QLabel* lblStart = new QLabel("From:");
        QDateEdit* dateStart = new QDateEdit(QDate::currentDate().addMonths(-1)); // Mặc định lùi 1 tháng
        dateStart->setCalendarPopup(true);
        dateStart->setObjectName("statDateStart");

        QLabel* lblEnd = new QLabel("To:");
        QDateEdit* dateEnd = new QDateEdit(QDate::currentDate());
        dateEnd->setCalendarPopup(true);
        dateEnd->setObjectName("statDateEnd");

        QPushButton* btnFilter = new QPushButton("Statistic");
        btnFilter->setObjectName("statFilterButton");
        btnFilter->setCursor(Qt::PointingHandCursor);
        btnFilter->setStyleSheet("background-color: #673AB7; color: white; font-weight: bold; padding: 6px 20px; border-radius: 4px;");

        filterLayout->addWidget(lblStart);
        filterLayout->addWidget(dateStart);
        filterLayout->addSpacing(15);
        filterLayout->addWidget(lblEnd);
        filterLayout->addWidget(dateEnd);
        filterLayout->addStretch();
        filterLayout->addWidget(btnFilter);

        // --- B. OVERVIEW CARDS (Doanh thu tổng) ---
        QHBoxLayout* cardsLayout = new QHBoxLayout();

        // Card Dog
        QLabel* lblDogRev = new QLabel("DOG revenue:\n0 VND");
        lblDogRev->setObjectName("lblDogRev");
        lblDogRev->setStyleSheet("background-color: #E3F2FD; color: #1565C0; font-weight: bold; font-size: 16px; padding: 20px; border-radius: 10px; border: 1px solid #BBDEFB;");
        lblDogRev->setAlignment(Qt::AlignCenter);

        // Card Cat
        QLabel* lblCatRev = new QLabel("CAT revenue:\n0 VND");
        lblCatRev->setObjectName("lblCatRev");
        lblCatRev->setStyleSheet("background-color: #FCE4EC; color: #C2185B; font-weight: bold; font-size: 16px; padding: 20px; border-radius: 10px; border: 1px solid #F8BBD0;");
        lblCatRev->setAlignment(Qt::AlignCenter);

        cardsLayout->addWidget(lblDogRev);
        cardsLayout->addWidget(lblCatRev);

        // --- C. TABLES (Top Breeds & Top Customers) ---
        QHBoxLayout* tablesLayout = new QHBoxLayout();

        // Bảng 1: Top Hot Breeds
        QGroupBox* grpBreed = new QGroupBox("TOP BEST-SELLING PET BREEDS");
        grpBreed->setStyleSheet("QGroupBox { font-weight: bold; color: #333; }");
        QVBoxLayout* breedLayout = new QVBoxLayout(grpBreed);
        QTableWidget* tblBreed = new QTableWidget();
        tblBreed->setObjectName("tblTopBreeds");
        tblBreed->setColumnCount(4);
        tblBreed->setHorizontalHeaderLabels({"Breed", "Type", "Quantity", "Total revenue"});
        tblBreed->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        breedLayout->addWidget(tblBreed);

        // Bảng 2: Top Customers
        QGroupBox* grpCust = new QGroupBox("TOP CUSTOMERS");
        grpCust->setStyleSheet("QGroupBox { font-weight: bold; color: #333; }");
        QVBoxLayout* custLayout = new QVBoxLayout(grpCust);
        QTableWidget* tblCust = new QTableWidget();
        tblCust->setObjectName("tblTopCust");
        tblCust->setColumnCount(3);
        tblCust->setHorizontalHeaderLabels({"Name", "Number of orders", "Total spent"});
        tblCust->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        custLayout->addWidget(tblCust);

        tablesLayout->addWidget(grpBreed);
        tablesLayout->addWidget(grpCust);

        // Add all to main
        mainLayout->addLayout(filterLayout);
        mainLayout->addLayout(cardsLayout);
        mainLayout->addLayout(tablesLayout);

        // Connect
        connect(btnFilter, &QPushButton::clicked, this, &AdminDashboard::on_filterStatsButton_clicked);
    }
    m_isStatsUiSetup = true;
}

// 2. XỬ LÝ KHI VÀO TRANG STATS


// 3. SLOT NÚT LỌC
void AdminDashboard::on_filterStatsButton_clicked() {
    calculateAndShowStats();
}

// 4. HÀM PARSE FILE BILL ĐỂ LẤY CHI TIẾT (Core Logic)
void AdminDashboard::parseBillForStats(const std::string& billId,
                                       long long& dogRev, long long& catRev,
                                       QMap<QString, BreedStat>& breeds,
                                       QMap<QString, CustomerStat>& customers)
{
    // Tìm file chi tiết
    QString path = "data/bills/" + QString::fromStdString(billId) + ".txt";
    if (!QFile::exists(path)) path = QString::fromStdString(billId) + ".txt"; // Fallback

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Tìm dòng chứa ID Pet (VD: "ID: d001")
        if (line.startsWith("ID:")) {
            QString petId = line.mid(3).trimmed();

            // Phân loại
            if (petId.startsWith("d")) {
                // Tìm thông tin Dog trong Repo để lấy Breed & Price
                Dog d = m_petRepo->getDogInfo(petId.toStdString());
                // Lưu ý: Nếu pet đã bị xóa khỏi file Dog.txt (Admin xóa), getDogInfo trả về rỗng.
                // Nếu pet chỉ bị set sold (status 0), nó vẫn trả về data.

                long long price = d.getPrice();
                // Nếu không tìm thấy trong Repo (đã xóa vĩnh viễn), thử tìm giá trong bill (phức tạp hơn)
                // Ở đây giả định tìm thấy hoặc giá = 0

                dogRev += price;

                // Cộng dồn Breed
                QString breedName = d.getId().empty() ? "Unknown" : QString::fromStdString(d.getBreed());
                breeds[breedName].breedName = breedName;
                breeds[breedName].type = "Chó";
                breeds[breedName].quantity++;
                breeds[breedName].revenue += price;
            }
            else if (petId.startsWith("c")) {
                Cat c = m_petRepo->getCatInfo(petId.toStdString());
                long long price = c.getPrice();

                catRev += price;

                QString breedName = c.getId().empty() ? "Unknown" : QString::fromStdString(c.getBreed());
                breeds[breedName].breedName = breedName;
                breeds[breedName].type = "Mèo";
                breeds[breedName].quantity++;
                breeds[breedName].revenue += price;
            }
        }
    }
    file.close();
}

// 5. HÀM TÍNH TOÁN TỔNG HỢP
void AdminDashboard::calculateAndShowStats() {
    // Lấy UI elements
    QDateEdit* dtStart = ui->statsPage->findChild<QDateEdit*>("statDateStart");
    QDateEdit* dtEnd = ui->statsPage->findChild<QDateEdit*>("statDateEnd");
    QLabel* lbDog = ui->statsPage->findChild<QLabel*>("lblDogRev");
    QLabel* lbCat = ui->statsPage->findChild<QLabel*>("lblCatRev");
    QTableWidget* tbBreed = ui->statsPage->findChild<QTableWidget*>("tblTopBreeds");
    QTableWidget* tbCust = ui->statsPage->findChild<QTableWidget*>("tblTopCust");

    if (!dtStart || !dtEnd) return;

    QDate start = dtStart->date();
    QDate end = dtEnd->date();

    // Reset biến đếm
    long long totalDogRevenue = 0;
    long long totalCatRevenue = 0;
    QMap<QString, BreedStat> breedMap;
    QMap<QString, CustomerStat> customerMap;

    // Duyệt tất cả hóa đơn tóm tắt
    LinkedList<Bill> bills = m_billRepo->getAllBills();
    Node<Bill>* node = bills.getHead();

    while(node != nullptr) {
        Bill b = node->getData();

        // Parse ngày tháng từ bill (dd/MM/yyyy)
        QDate billDate = QDate::fromString(QString::fromStdString(b.getDate()), "dd/MM/yyyy");

        // Kiểm tra khoảng thời gian
        if (billDate.isValid() && billDate >= start && billDate <= end) {

            // 1. Thống kê Khách hàng
            QString cId = QString::fromStdString(b.getClientId());
            if (cId != "0000000000") { // Bỏ qua khách vãng lai
                customerMap[cId].id = cId;
                customerMap[cId].name = QString::fromStdString(b.getClientName());
                customerMap[cId].buyCount++;
                customerMap[cId].totalSpent += b.getTotalAmount();
            }

            // 2. Thống kê Pet (Cần đọc file chi tiết)
            parseBillForStats(b.getBillId(), totalDogRevenue, totalCatRevenue, breedMap, customerMap);
        }

        node = node->getNext();
    }

    // --- HIỂN THỊ ---

    // A. Overview Labels
    // Format tiền tệ (thêm dấu chấm)
    QLocale locale(QLocale::Vietnamese, QLocale::Vietnam);
    lbDog->setText("DOG revenue:\n" + locale.toString((qlonglong)totalDogRevenue) + " VND");
    lbCat->setText("CAT revenue:\n" + locale.toString((qlonglong)totalCatRevenue) + " VND");

    // B. Top Breeds Table
    QList<BreedStat> breedList = breedMap.values();
    // Sắp xếp giảm dần theo số lượng bán
    std::sort(breedList.begin(), breedList.end(), [](const BreedStat& a, const BreedStat& b) {
        return a.quantity > b.quantity;
    });

    tbBreed->setRowCount(0);
    for(const auto& item : breedList) {
        if (item.breedName == "Unknown") continue; // Bỏ qua unknown
        int r = tbBreed->rowCount();
        tbBreed->insertRow(r);
        tbBreed->setItem(r, 0, new QTableWidgetItem(item.breedName));
        tbBreed->setItem(r, 1, new QTableWidgetItem(item.type));
        tbBreed->setItem(r, 2, new QTableWidgetItem(QString::number(item.quantity)));
        tbBreed->setItem(r, 3, new QTableWidgetItem(locale.toString((qlonglong)item.revenue)));
    }

    // C. Top Customer Table
    QList<CustomerStat> custList = customerMap.values();
    // Sắp xếp giảm dần theo tổng tiền chi
    std::sort(custList.begin(), custList.end(), [](const CustomerStat& a, const CustomerStat& b) {
        return a.totalSpent > b.totalSpent;
    });

    tbCust->setRowCount(0);
    int limitCust = qMin(10, custList.size()); // Lấy top 10
    for(int i=0; i<limitCust; i++) {
        int r = tbCust->rowCount();
        tbCust->insertRow(r);
        tbCust->setItem(r, 0, new QTableWidgetItem(custList[i].name));
        tbCust->setItem(r, 1, new QTableWidgetItem(QString::number(custList[i].buyCount)));
        tbCust->setItem(r, 2, new QTableWidgetItem(locale.toString((qlonglong)custList[i].totalSpent)));
    }
}

void AdminDashboard::setupProfileUI() {
    if (m_isProfileUiSetup) return;

    if (ui->profilePage->layout() == nullptr) {
        QVBoxLayout* mainLayout = new QVBoxLayout(ui->profilePage);
        mainLayout->setSpacing(20);
        mainLayout->setAlignment(Qt::AlignTop);

        // --- A. HEADER ---
        QVBoxLayout* headerLayout = new QVBoxLayout();
        QLabel* avatar = new QLabel("👤");
        avatar->setStyleSheet("font-size: 60px; color: #555;");
        avatar->setAlignment(Qt::AlignCenter);

        QLabel* title = new QLabel("ADMIN INFORMATIONS");
        title->setStyleSheet("font-size: 20px; font-weight: bold; color: #D32F2F;");
        title->setAlignment(Qt::AlignCenter);

        headerLayout->addWidget(avatar);
        headerLayout->addWidget(title);

        // --- B. FORM NHẬP LIỆU ---
        QGroupBox* formGroup = new QGroupBox("Edit informations");
        formGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ccc; border-radius: 5px; margin-top: 10px; padding: 20px; }");

        QFormLayout* formLayout = new QFormLayout(formGroup);
        formLayout->setSpacing(15);

        m_profileNameInput = new QLineEdit();
        m_profileNameInput->setPlaceholderText("Name");
        m_profileNameInput->setStyleSheet("padding: 8px; border: 1px solid #aaa; border-radius: 4px;");

        m_profilePassInput = new QLineEdit();
        m_profilePassInput->setEchoMode(QLineEdit::Password);
        m_profilePassInput->setPlaceholderText("New password");
        m_profilePassInput->setStyleSheet("padding: 8px; border: 1px solid #aaa; border-radius: 4px;");

        m_profileRoleLabel = new QLabel("ADMINISTRATOR");
        m_profileRoleLabel->setStyleSheet("color: #D32F2F; font-weight: bold; padding: 8px;");

        formLayout->addRow("Name:", m_profileNameInput);
        formLayout->addRow("Password:", m_profilePassInput);
        formLayout->addRow("Role:", m_profileRoleLabel);

        // --- C. BUTTONS (Đã xóa nút Đăng xuất) ---
        QHBoxLayout* btnLayout = new QHBoxLayout();

        QPushButton* btnSave = new QPushButton("SAVE");
        btnSave->setCursor(Qt::PointingHandCursor);
        btnSave->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 10px 20px; border-radius: 5px;");

        // Chỉ còn nút Save, căn giữa hoặc phải tùy ý (ở đây tôi để căn phải)
        btnLayout->addStretch();
        btnLayout->addWidget(btnSave);

        mainLayout->addLayout(headerLayout);
        mainLayout->addSpacing(20);
        mainLayout->addWidget(formGroup);
        mainLayout->addLayout(btnLayout);
        mainLayout->addStretch();

        // Connect
        connect(btnSave, &QPushButton::clicked, this, &AdminDashboard::on_saveProfileButton_clicked);
    }
    m_isProfileUiSetup = true;
}

// 2. LOGIC KHI VÀO TRANG PROFILE


// 3. TẢI DỮ LIỆU TỪ OBJECT ADMIN LÊN FORM
void AdminDashboard::loadProfileData() {
    if (m_currentAdmin && m_profileNameInput) {
        m_profileNameInput->setText(QString::fromStdString(m_currentAdmin->getName()));
        m_profilePassInput->setText(QString::fromStdString(m_currentAdmin->getPassword()));
    }
}

// 4. XỬ LÝ LƯU THÔNG TIN
void AdminDashboard::on_saveProfileButton_clicked() {
    QString newName = m_profileNameInput->text();
    QString newPass = m_profilePassInput->text();

    if (newName.isEmpty() || newPass.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Tên và Mật khẩu không được để trống.");
        return;
    }

    // 1. Cập nhật Object Admin trong bộ nhớ
    m_currentAdmin->setName(newName.toStdString());
    m_currentAdmin->setPassword(newPass.toStdString());

    // 2. Cập nhật giao diện Sidebar ngay lập tức
    ui->userNameLabel->setText(newName);

    QMessageBox::information(this, "Thành công", "Thông tin quản trị viên đã được cập nhật!\n(Lưu ý: Cần implement hàm ghi file Admin nếu muốn lưu vĩnh viễn)");
}
