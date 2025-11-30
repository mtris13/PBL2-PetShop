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

    ui->userNameLabel->setText(QString::fromStdString(m_currentAdmin->getName()));

    m_navGroup = new QButtonGroup(this);
    m_navGroup->addButton(ui->manageAccountsButton);
    m_navGroup->addButton(ui->managePetsButton);
    m_navGroup->addButton(ui->manageSpaButton);
    m_navGroup->addButton(ui->historyButton);
    m_navGroup->addButton(ui->statsButton);
    m_navGroup->addButton(ui->profileButton);
    m_navGroup->setExclusive(true);

    connect(ui->manageAccountsButton, &QPushButton::clicked, this, &AdminDashboard::handleManageAccountsClick);
    connect(ui->managePetsButton, &QPushButton::clicked, this, &AdminDashboard::handleManagePetsClick);
    connect(ui->manageSpaButton, &QPushButton::clicked, this, &AdminDashboard::handleManageSpaClick);
    connect(ui->historyButton, &QPushButton::clicked, this, &AdminDashboard::handleHistoryClick);
    connect(ui->statsButton, &QPushButton::clicked, this, &AdminDashboard::handleStatsClick);
    connect(ui->profileButton, &QPushButton::clicked, this, &AdminDashboard::handleProfileClick);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminDashboard::handleLogoutClick);

    ui->manageAccountsButton->setChecked(true);
    ui->pageStackedWidget->setCurrentWidget(ui->manageAccountsPage);
    ui->accStackedWidget->setCurrentWidget(ui->accListPage);
    ui->accountsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->accountsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->petStatusCheckbox->setStyleSheet("QCheckBox { color: black; font-weight: bold; }");
    loadAccountsTable();
    connect(ui->petTypeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index){
                loadPetsTable(ui->petSearchInput->text());
            });
}

AdminDashboard::~AdminDashboard()
{
    delete ui;
}

void AdminDashboard::handleManageAccountsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->manageAccountsPage);
    ui->accStackedWidget->setCurrentWidget(ui->accListPage);
    // ------------------------

    loadAccountsTable();
}
void AdminDashboard::handleManagePetsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->managePetsPage);
    ui->petStackedWidget->setCurrentWidget(ui->petListPage);

    loadPetsTable();
}
void AdminDashboard::handleManageSpaClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->manageSpaPage);

    ui->spaStackedWidget->setCurrentWidget(ui->spaListPage);

    loadSpaTable();
}
void AdminDashboard::handleHistoryClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->historyPage);

    setupHistoryUI();

    loadHistoryTable("");
}
void AdminDashboard::handleStatsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->statsPage);
    setupStatsUI();
    calculateAndShowStats();
}
void AdminDashboard::handleProfileClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->profilePage);
    setupProfileUI();
    loadProfileData();
}
void AdminDashboard::handleLogoutClick() {
    emit logoutSignal();
    this->close();
}

#include <QMessageBox>

void AdminDashboard::loadAccountsTable(const QString& search)
{
    ui->accountsTable->setRowCount(0);
    ui->accountsTable->setColumnCount(5);
    QStringList headers = {"ID", "Name", "Password", "Gender", "Role"};
    ui->accountsTable->setHorizontalHeaderLabels(headers);

    int row = 0;
    std::string keyword = search.toLower().toStdString();

    LinkedList<Staff> staffs = m_accountRepo->getAllStaffInfo();
    Node<Staff>* sNode = staffs.getHead();
    while(sNode != nullptr) {
        Staff s = sNode->getData();

        string id = s.getId();
        string name = s.getName();
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

    LinkedList<Client> clients = m_accountRepo->getAllClientInfo();
    Node<Client>* cNode = clients.getHead();
    while(cNode != nullptr) {
        Client c = cNode->getData();
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


void AdminDashboard::on_accSearchButton_clicked() {
    loadAccountsTable(ui->accSearchInput->text());
}

void AdminDashboard::on_addStaffButton_clicked() {
    ui->accIdInput->clear(); ui->accIdInput->setReadOnly(false);
    ui->accNameInput->clear();
    ui->accPassInput->clear();
    ui->maleRadio->setChecked(true);
    ui->accSalaryInput->clear(); ui->accSalaryInput->setEnabled(true);

    ui->accStackedWidget->setCurrentWidget(ui->accFormPage);
}

void AdminDashboard::on_viewAccButton_clicked() {
    int row = ui->accountsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select an account to view.");
        return;
    }

    std::string id = ui->accountsTable->item(row, 0)->text().toStdString();
    std::string role = ui->accountsTable->item(row, 4)->text().toStdString();

    if (role == "Staff") {
        Staff s = m_accountRepo->getStaffInfo(id);
        ui->accIdInput->setText(QString::fromStdString(s.getId()));
        ui->accNameInput->setText(QString::fromStdString(s.getName()));
        ui->accPassInput->setText(QString::fromStdString(s.getPassword()));
        if(s.getGender() == "Male") ui->maleRadio->setChecked(true); else ui->femaleRadio->setChecked(true);
        ui->accSalaryInput->setText(QString::number(s.getSalary()));
        ui->accSalaryInput->setEnabled(true);
    } else {
        Client c = m_accountRepo->getClientInfo(id);
        ui->accIdInput->setText(QString::fromStdString(c.getId()));
        ui->accNameInput->setText(QString::fromStdString(c.getName()));
        ui->accPassInput->setText(QString::fromStdString(c.getPassword()));
        if(c.getGender() == "Male") ui->maleRadio->setChecked(true); else ui->femaleRadio->setChecked(true);
        ui->accSalaryInput->setText("");
        ui->accSalaryInput->setEnabled(false);
    }

    ui->accIdInput->setReadOnly(true);
    ui->accStackedWidget->setCurrentWidget(ui->accFormPage);
}

void AdminDashboard::on_deleteAccButton_clicked() {
    int row = ui->accountsTable->currentRow();
    if (row < 0) return;

    std::string id = ui->accountsTable->item(row, 0)->text().toStdString();
    std::string name = ui->accountsTable->item(row, 1)->text().toStdString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm deletion",
                                  QString("Are you sure you want to delete account: %1 (%2)?\nData will be backed up to the backup folder.").arg(QString::fromStdString(name), QString::fromStdString(id)),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_accountRepo->deleteAccount(id);
        loadAccountsTable();
        QMessageBox::information(this, "Success", "Account deleted successfully.");
    }
}

void AdminDashboard::on_saveAccButton_clicked() {
    std::string id = ui->accIdInput->text().toStdString();
    std::string name = ui->accNameInput->text().toStdString();
    std::string pass = ui->accPassInput->text().toStdString();
    std::string gender = ui->maleRadio->isChecked() ? "Male" : "Female";

    if (id.empty() || name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Lack of information", "Please enter full ID, Name and Password.");
        return;
    }


    if (id.length() == 5) {
        long salary = ui->accSalaryInput->text().toLong();
        Staff s(id, name, pass, gender, salary);
        m_accountRepo->setStaffInfo(s);
    }
    else if (id.length() == 10) {
        // Client
        Client oldC = m_accountRepo->getClientInfo(id);
        Client newC(id, name, pass, gender, oldC.getStreet(), oldC.getCity());
        m_accountRepo->setClientInfo(newC);
    }
    else {
        QMessageBox::warning(this, "Wrong ID", "Staff ID must be 5 characters, Client must be 10 characters.");
        return;
    }

    QMessageBox::information(this, "Success", "Account information saved.");
    ui->accStackedWidget->setCurrentWidget(ui->accListPage);
    loadAccountsTable();
}

void AdminDashboard::on_cancelAccButton_clicked() {
    ui->accStackedWidget->setCurrentWidget(ui->accListPage);
}
void AdminDashboard::backupData(const QString& folderName, const QString& id, const QString& content) {
    QString path = "BackupData/" + folderName;
    QDir dir;
    if (!dir.exists(path)) {
        dir.mkpath(path);
    }

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
    ui->petsTable->setColumnCount(8);
    QStringList headers = {"ID", "Name", "Type", "Breed", "Price (VND)", "Status", "Characteristic", "HiddenType"};
    ui->petsTable->setHorizontalHeaderLabels(headers);
    ui->petsTable->setColumnHidden(7, true);

    int row = 0;
    std::string keyword = search.toLower().toStdString();

    // Lấy chỉ mục đang chọn: 0=All, 1=Dog, 2=Cat
    int filterIndex = ui->petTypeFilter->currentIndex();

    // --- LOAD DOGS ---
    // Logic: Nếu chọn All (0) HOẶC chọn Dog (1) thì mới load chó
    if (filterIndex == 0 || filterIndex == 1) {
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

            // Set màu chữ cho trạng thái
            if(isAvailable) statusItem->setForeground(QBrush(QColor("green")));
            else statusItem->setForeground(QBrush(QColor("red")));

            ui->petsTable->setItem(row, 5, statusItem);
            ui->petsTable->setItem(row, 6, new QTableWidgetItem("Energy: " + QString::number(p.getEnergyLevel()) + "/10"));
            ui->petsTable->setItem(row, 7, new QTableWidgetItem("dog"));
            row++;
            dNode = dNode->getNext();
        }
    }

    // --- LOAD CATS ---
    // Logic: Nếu chọn All (0) HOẶC chọn Cat (2) thì mới load mèo
    if (filterIndex == 0 || filterIndex == 2) {
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

            if(isAvailable) statusItem->setForeground(QBrush(QColor("green")));
            else statusItem->setForeground(QBrush(QColor("red")));

            ui->petsTable->setItem(row, 5, statusItem);
            ui->petsTable->setItem(row, 6, new QTableWidgetItem("Fur: " + QString::fromStdString(p.getFurLength())));
            ui->petsTable->setItem(row, 7, new QTableWidgetItem("cat"));
            row++;
            cNode = cNode->getNext();
        }
    }
    ui->petsTable->resizeColumnsToContents();
}

void AdminDashboard::on_petTypeFilter_currentIndexChanged(int index) {
    // Khi đổi filter (All/Dog/Cat), load lại bảng ngay lập tức
    loadPetsTable(ui->petSearchInput->text());
}
void AdminDashboard::on_petSearchButton_clicked() {
    loadPetsTable(ui->petSearchInput->text());
}


void AdminDashboard::on_dogRadio_toggled(bool checked) {
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
    m_isEditMode = false;
    ui->petStackedWidget->setCurrentWidget(ui->petFormPage);


    ui->petNameInput->clear();
    ui->petBreedInput->clear();
    ui->petAgeInput->setValue(1);
    ui->petPriceInput->clear();
    ui->petDescInput->clear();
    ui->dogEnergyInput->setValue(5);
    ui->catFurInput->clear();
ui->petStatusCheckbox->setChecked(true);
    ui->dogRadio->setEnabled(true);
    ui->catRadio->setEnabled(true);

    if (ui->dogRadio->isChecked()) {
        on_dogRadio_toggled(true);
    } else {
        ui->dogRadio->setChecked(true);
    }
}


void AdminDashboard::on_editPetButton_clicked() {
    int row = ui->petsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select the pet you want to edit.");
        return;
    }

    m_isEditMode = true;
    QString id = ui->petsTable->item(row, 0)->text();
    QString type = ui->petsTable->item(row, 7)->text();
    m_editingId = id.toStdString();

    ui->petStackedWidget->setCurrentWidget(ui->petFormPage);

    ui->dogRadio->setEnabled(false);
    ui->catRadio->setEnabled(false);

    if (type == "dog") {
        ui->dogRadio->setChecked(true);
        Dog d = m_petRepo->getDogInfo(m_editingId);
        ui->petNameInput->setText(QString::fromStdString(d.getName()));
        ui->petBreedInput->setText(QString::fromStdString(d.getBreed()));
        ui->petAgeInput->setValue(d.getAge());
        ui->petPriceInput->setText(QString::number(d.getPrice()));
        ui->petDescInput->setText(QString::fromStdString(d.getDescription()));
        ui->dogEnergyInput->setValue(d.getEnergyLevel());
        bool isAvail = (d.getStatus() == "1" || d.getStatus() == "available");
        ui->petStatusCheckbox->setChecked(isAvail);
    } else {
        ui->catRadio->setChecked(true);
        Cat c = m_petRepo->getCatInfo(m_editingId);
        ui->petNameInput->setText(QString::fromStdString(c.getName()));
        ui->petBreedInput->setText(QString::fromStdString(c.getBreed()));
        ui->petAgeInput->setValue(c.getAge());
        ui->petPriceInput->setText(QString::number(c.getPrice()));
        ui->petDescInput->setText(QString::fromStdString(c.getDescription()));
        ui->catFurInput->setText(QString::fromStdString(c.getFurLength()));
        bool isAvail = (c.getStatus() == "1" || c.getStatus() == "available");
        ui->petStatusCheckbox->setChecked(isAvail);
    }
    ui->petIdInput->setText(id);
}


void AdminDashboard::on_savePetButton_clicked() {
    // Validate dữ liệu
    if (ui->petNameInput->text().isEmpty() || ui->petPriceInput->text().isEmpty()) {
        QMessageBox::warning(this, "Lack of information", "Please enter Name and Price.");
        return;
    }

    std::string id = ui->petIdInput->text().toStdString();
    std::string name = ui->petNameInput->text().toStdString();
    std::string breed = ui->petBreedInput->text().toStdString();
    int age = ui->petAgeInput->value();
    long price = ui->petPriceInput->text().toLong();
    std::string desc = ui->petDescInput->text().toStdString();
    if (desc.empty()) desc = "No description";

    bool status = ui->petStatusCheckbox->isChecked();

    if (ui->dogRadio->isChecked()) {
        int energy = ui->dogEnergyInput->value();
        Dog d(id, name, breed, age, price, status, energy, desc);
        m_petRepo->setDogInfo(d);
    } else {
        std::string fur = ui->catFurInput->text().toStdString();
        Cat c(id, name, breed, age, price, status, fur, desc);
        m_petRepo->setCatInfo(c);
    }

    QMessageBox::information(this, "Success", "Pet information saved.");
    ui->petStackedWidget->setCurrentWidget(ui->petListPage);
    loadPetsTable();
}

void AdminDashboard::on_deletePetButton_clicked() {
    int row = ui->petsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select a pet to delete.");
        return;
    }

    QString id = ui->petsTable->item(row, 0)->text();
    QString name = ui->petsTable->item(row, 1)->text();
    QString type = ui->petsTable->item(row, 7)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm deletion",
                                  "Are you sure you want to delete: " + name + " (" + id + ")?\nData will be backed up.",
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
        if (type == "dog") m_petRepo->deletePet(id.toStdString());
        else m_petRepo->deletePet(id.toStdString());

        loadPetsTable();
        QMessageBox::information(this, "Đã xóa", "Deleted and backed up data to folder BackupData/Pets.");
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
    ui->spaIdInput->setReadOnly(false);
    ui->spaNameInput->clear();
    ui->spaDescInput->clear();
    ui->spaPriceInput->clear();
    ui->spaDurationInput->setValue(60);
}

// 3. Nút Sửa
void AdminDashboard::on_editServiceButton_clicked() {
    int row = ui->spaTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select a service to edit.");
        return;
    }
    QString id = ui->spaTable->item(row, 0)->text();

    Service s = m_serviceRepo->getServiceInfo(id.toStdString());

    ui->spaStackedWidget->setCurrentWidget(ui->spaFormPage);
    ui->spaIdInput->setText(QString::fromStdString(s.getId()));
    ui->spaIdInput->setReadOnly(true);
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
        QMessageBox::warning(this, "Lack of information", "Please enter ID and Name.");
        return;
    }


    if (!m_isEditMode && m_serviceRepo->isValidServiceId(id)) {
        QMessageBox::warning(this, "Duplicate ID",
                             "ID '" + QString::fromStdString(id) + "' already exists!\nPlease choose another ID.");
        return;
    }

    Service s(id, name, desc, price, duration);
    m_serviceRepo->setServiceInfo(s);

    QMessageBox::information(this, "Success", "Spa service saved.");
    ui->spaStackedWidget->setCurrentWidget(ui->spaListPage);
    loadSpaTable(); // Tải lại bảng từ RAM (đã có dữ liệu mới)
}

// 5. Nút Xóa (Backup)
void AdminDashboard::on_deleteServiceButton_clicked() {
    int row = ui->spaTable->currentRow();
    if (row < 0) return;
    QString id = ui->spaTable->item(row, 0)->text();

    if (QMessageBox::question(this, "Delete", "Delete service " + id + "?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
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
    if (m_isHistoryUiSetup) return;

    // Kiểm tra layout, nếu chưa có thì tạo mới
    if (ui->historyPage->layout() == nullptr) {
        QVBoxLayout* mainLayout = new QVBoxLayout(ui->historyPage);

        // 1. THANH TÌM KIẾM (Giữ nguyên)
        QHBoxLayout* toolbarLayout = new QHBoxLayout();
        QLineEdit* searchInput = new QLineEdit(ui->historyPage);
        searchInput->setObjectName("historySearchInput");
        searchInput->setPlaceholderText("Search by ID, Name, Date...");

        QPushButton* searchBtn = new QPushButton("Search", ui->historyPage);
        searchBtn->setObjectName("historySearchButton");
        searchBtn->setCursor(Qt::PointingHandCursor);
        searchBtn->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold; padding: 5px 15px; border-radius: 4px;");

        toolbarLayout->addWidget(searchInput);
        toolbarLayout->addWidget(searchBtn);

        // 2. TẠO TAB WIDGET (Booking | Sale)

        QTabWidget* tabWidget = new QTabWidget(ui->historyPage);
        tabWidget->setObjectName("historyTabWidget");

        // --- STYLE MỚI: NỔI BẬT HƠN (Vàng - Đỏ) ---
        tabWidget->setStyleSheet(
            // Khung viền của nội dung tab
            "QTabWidget::pane { border: 2px solid #D32F2F; background: #FFFFFF; }"

            // Tab trạng thái bình thường (Chưa chọn): Nền xám đậm, Chữ trắng
            "QTabBar::tab { "
            "    background: #555555; "
            "    color: #FFFFFF; "
            "    min-width: 150px; "
            "    padding: 10px; "
            "    font-weight: bold; "
            "    margin-right: 2px; "
            "    border-top-left-radius: 4px; "
            "    border-top-right-radius: 4px; "
            "}"

            // Tab khi rê chuột vào: Sáng hơn chút
            "QTabBar::tab:hover { background: #777777; }"


            "QTabBar::tab:selected { "
            "    background: #FDD85D; " /* Màu vàng thương hiệu Admin */
            "    color: #D32F2F; "      /* Chữ đỏ đậm */
            "    border-bottom: 3px solid #D32F2F; " /* Gạch chân đỏ */
            "}"
            );

        // --- TAB 1: BOOKING HISTORY ---
        QWidget* bookingTab = new QWidget();
        QVBoxLayout* bookingLayout = new QVBoxLayout(bookingTab);
        QTableWidget* tblBooking = new QTableWidget();
        tblBooking->setObjectName("tblBookingHistory");
        tblBooking->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblBooking->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblBooking->setAlternatingRowColors(true);
        tblBooking->verticalHeader()->setVisible(false);
        // Setup cột cho Booking
        tblBooking->setColumnCount(6);
        tblBooking->setHorizontalHeaderLabels({"ID", "Customer", "Service", "Date", "Time", "Detail"});
        tblBooking->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Giãn cột tên khách
        bookingLayout->addWidget(tblBooking);

        // --- TAB 2: SALE HISTORY (Hóa đơn) ---
        QWidget* saleTab = new QWidget();
        QVBoxLayout* saleLayout = new QVBoxLayout(saleTab);
        QTableWidget* tblSale = new QTableWidget();
        tblSale->setObjectName("tblSaleHistory"); // Đổi tên để dễ tìm
        tblSale->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblSale->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblSale->setAlternatingRowColors(true);
        tblSale->verticalHeader()->setVisible(false);
        // Setup cột cho Bill
        tblSale->setColumnCount(6);
        tblSale->setHorizontalHeaderLabels({"ID", "Customer", "Date", "Time", "Total (VND)", "Detail"});
        tblSale->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        saleLayout->addWidget(tblSale);

        // Thêm các tab vào Widget
        tabWidget->addTab(bookingTab, "Booking History");
        tabWidget->addTab(saleTab, "Sales History");

        // Thêm vào layout chính
        mainLayout->addLayout(toolbarLayout);
        mainLayout->addWidget(tabWidget);

        // Kết nối sự kiện tìm kiếm
        connect(searchBtn, &QPushButton::clicked, this, [this, searchInput](){
            loadHistoryTable(searchInput->text());
        });
        connect(searchInput, &QLineEdit::returnPressed, this, [this, searchInput](){
            loadHistoryTable(searchInput->text());
        });
    }
    m_isHistoryUiSetup = true;
}





void AdminDashboard::loadHistoryTable(const QString& search) {
    // Gọi hàm load cho cả 2 bảng
    loadBookingHistoryTable(search);
    loadBillHistoryTable(search);
}

void AdminDashboard::on_historySearchButton_clicked() {
    QLineEdit* input = ui->historyPage->findChild<QLineEdit*>("historySearchInput");
    if (input) {
        loadHistoryTable(input->text());
    }
}

void AdminDashboard::showBillDetail(const std::string& billId) {
    QString filePath = "data/bills/" + QString::fromStdString(billId) + ".txt";

    if (!QFile::exists(filePath)) {
        filePath = QString::fromStdString(billId) + ".txt";
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No detail file found for invoice: " + QString::fromStdString(billId));
        return;
    }

    QTextStream in(&file);
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
    msgBox.setStyleSheet("QLabel{min-width: 400px; font-family: 'Courier New'; font-size: 12px;}");
    msgBox.exec();
}

void AdminDashboard::setupStatsUI() {
    if (m_isStatsUiSetup) return;

    if (ui->statsPage->layout() == nullptr) {
        QVBoxLayout* mainLayout = new QVBoxLayout(ui->statsPage);
        mainLayout->setSpacing(20);

        QHBoxLayout* filterLayout = new QHBoxLayout();

        QLabel* lblStart = new QLabel("From:");
        QDateEdit* dateStart = new QDateEdit(QDate::currentDate().addMonths(-1));
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

        QHBoxLayout* cardsLayout = new QHBoxLayout();

        QLabel* lblDogRev = new QLabel("DOG revenue:\n0 VND");
        lblDogRev->setObjectName("lblDogRev");
        lblDogRev->setStyleSheet("background-color: #E3F2FD; color: #1565C0; font-weight: bold; font-size: 16px; padding: 20px; border-radius: 10px; border: 1px solid #BBDEFB;");
        lblDogRev->setAlignment(Qt::AlignCenter);

        QLabel* lblCatRev = new QLabel("CAT revenue:\n0 VND");
        lblCatRev->setObjectName("lblCatRev");
        lblCatRev->setStyleSheet("background-color: #FCE4EC; color: #C2185B; font-weight: bold; font-size: 16px; padding: 20px; border-radius: 10px; border: 1px solid #F8BBD0;");
        lblCatRev->setAlignment(Qt::AlignCenter);

        cardsLayout->addWidget(lblDogRev);
        cardsLayout->addWidget(lblCatRev);

        QHBoxLayout* tablesLayout = new QHBoxLayout();

        QGroupBox* grpBreed = new QGroupBox("TOP BEST-SELLING PET BREEDS");
        grpBreed->setStyleSheet("QGroupBox { font-weight: bold; color: #333; }");
        QVBoxLayout* breedLayout = new QVBoxLayout(grpBreed);
        QTableWidget* tblBreed = new QTableWidget();
        tblBreed->setObjectName("tblTopBreeds");
        tblBreed->setColumnCount(4);
        tblBreed->setHorizontalHeaderLabels({"Breed", "Type", "Quantity", "Total revenue"});
        tblBreed->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        breedLayout->addWidget(tblBreed);

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

        mainLayout->addLayout(filterLayout);
        mainLayout->addLayout(cardsLayout);
        mainLayout->addLayout(tablesLayout);

        connect(btnFilter, &QPushButton::clicked, this, &AdminDashboard::on_filterStatsButton_clicked);
    }
    m_isStatsUiSetup = true;
}



void AdminDashboard::on_filterStatsButton_clicked() {
    calculateAndShowStats();
}

void AdminDashboard::parseBillForStats(const std::string& billId,
                                       long long& dogRev, long long& catRev,
                                       QMap<QString, BreedStat>& breeds,
                                       QMap<QString, CustomerStat>& customers)
{
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

        if (line.startsWith("ID:")) {
            QString petId = line.mid(3).trimmed();

            if (petId.startsWith("d")) {
                Dog d = m_petRepo->getDogInfo(petId.toStdString());

                long long price = d.getPrice();

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

void AdminDashboard::calculateAndShowStats() {
    QDateEdit* dtStart = ui->statsPage->findChild<QDateEdit*>("statDateStart");
    QDateEdit* dtEnd = ui->statsPage->findChild<QDateEdit*>("statDateEnd");
    QLabel* lbDog = ui->statsPage->findChild<QLabel*>("lblDogRev");
    QLabel* lbCat = ui->statsPage->findChild<QLabel*>("lblCatRev");
    QTableWidget* tbBreed = ui->statsPage->findChild<QTableWidget*>("tblTopBreeds");
    QTableWidget* tbCust = ui->statsPage->findChild<QTableWidget*>("tblTopCust");

    if (!dtStart || !dtEnd) return;

    QDate start = dtStart->date();
    QDate end = dtEnd->date();

    long long totalDogRevenue = 0;
    long long totalCatRevenue = 0;
    QMap<QString, BreedStat> breedMap;
    QMap<QString, CustomerStat> customerMap;

    LinkedList<Bill> bills = m_billRepo->getAllBills();
    Node<Bill>* node = bills.getHead();

    while(node != nullptr) {
        Bill b = node->getData();

        QDate billDate = QDate::fromString(QString::fromStdString(b.getDate()), "dd/MM/yyyy");

        if (billDate.isValid() && billDate >= start && billDate <= end) {

            QString cId = QString::fromStdString(b.getClientId());
            if (cId != "0000000000") {
                customerMap[cId].id = cId;
                customerMap[cId].name = QString::fromStdString(b.getClientName());
                customerMap[cId].buyCount++;
                customerMap[cId].totalSpent += b.getTotalAmount();
            }


            parseBillForStats(b.getBillId(), totalDogRevenue, totalCatRevenue, breedMap, customerMap);
        }

        node = node->getNext();
    }


    QLocale locale(QLocale::Vietnamese, QLocale::Vietnam);
    lbDog->setText("DOG revenue:\n" + locale.toString((qlonglong)totalDogRevenue) + " VND");
    lbCat->setText("CAT revenue:\n" + locale.toString((qlonglong)totalCatRevenue) + " VND");

    QList<BreedStat> breedList = breedMap.values();
    std::sort(breedList.begin(), breedList.end(), [](const BreedStat& a, const BreedStat& b) {
        return a.quantity > b.quantity;
    });

    tbBreed->setRowCount(0);
    for(const auto& item : breedList) {
        if (item.breedName == "Unknown") continue;
        int r = tbBreed->rowCount();
        tbBreed->insertRow(r);
        tbBreed->setItem(r, 0, new QTableWidgetItem(item.breedName));
        tbBreed->setItem(r, 1, new QTableWidgetItem(item.type));
        tbBreed->setItem(r, 2, new QTableWidgetItem(QString::number(item.quantity)));
        tbBreed->setItem(r, 3, new QTableWidgetItem(locale.toString((qlonglong)item.revenue)));
    }

    QList<CustomerStat> custList = customerMap.values();

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

        QVBoxLayout* headerLayout = new QVBoxLayout();
        QLabel* avatar = new QLabel("👤");
        avatar->setStyleSheet("font-size: 60px; color: #555;");
        avatar->setAlignment(Qt::AlignCenter);

        QLabel* title = new QLabel("ADMIN INFORMATIONS");
        title->setStyleSheet("font-size: 20px; font-weight: bold; color: #D32F2F;");
        title->setAlignment(Qt::AlignCenter);

        headerLayout->addWidget(avatar);
        headerLayout->addWidget(title);

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

        QHBoxLayout* btnLayout = new QHBoxLayout();

        QPushButton* btnSave = new QPushButton("SAVE");
        btnSave->setCursor(Qt::PointingHandCursor);
        btnSave->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 10px 20px; border-radius: 5px;");

        btnLayout->addStretch();
        btnLayout->addWidget(btnSave);

        mainLayout->addLayout(headerLayout);
        mainLayout->addSpacing(20);
        mainLayout->addWidget(formGroup);
        mainLayout->addLayout(btnLayout);
        mainLayout->addStretch();

        connect(btnSave, &QPushButton::clicked, this, &AdminDashboard::on_saveProfileButton_clicked);
    }
    m_isProfileUiSetup = true;
}



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
        QMessageBox::warning(this, "Error", "Name and Password cannot be blank.");
        return;
    }

    m_currentAdmin->setName(newName.toStdString());
    m_currentAdmin->setPassword(newPass.toStdString());

    ui->userNameLabel->setText(newName);

    QMessageBox::information(this, "Success", "Admin information has been updated!");
}
void AdminDashboard::loadBillHistoryTable(const QString& search) {
    // Tìm bảng Sale theo tên đã đặt ở bước 2.1
    QTableWidget* table = ui->historyPage->findChild<QTableWidget*>("tblSaleHistory");
    if (!table) return;

    table->setRowCount(0);
    // ... (Các bước setup header đã làm lúc tạo bảng rồi nên bỏ qua)

    LinkedList<Bill> bills = m_billRepo->getAllBills();
    Node<Bill>* node = bills.getHead();
    int row = 0;
    std::string keyword = search.toLower().toStdString();

    while(node != nullptr) {
        Bill b = node->getData();
        std::string id = b.getBillId(); std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        std::string name = b.getClientName(); std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        std::string date = b.getDate();

        if (keyword.empty() || id.find(keyword) != std::string::npos ||
            name.find(keyword) != std::string::npos || date.find(keyword) != std::string::npos) {

            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBillId())));
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.getClientName())));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.getDate())));
            table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getTime())));

            QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(b.getTotalAmount()));
            priceItem->setForeground(Qt::blue);
            priceItem->setFont(QFont("Arial", 9, QFont::Bold));
            table->setItem(row, 4, priceItem);

            // Nút View
            QPushButton* viewBtn = new QPushButton("View");
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 3px; padding: 2px 10px;");
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
void AdminDashboard::loadBookingHistoryTable(const QString& search) {
    QTableWidget* table = ui->historyPage->findChild<QTableWidget*>("tblBookingHistory");
    if (!table) return;

    table->setRowCount(0);

    LinkedList<Booking> bookings = m_bookingRepo->getAllBookings();
    Node<Booking>* node = bookings.getHead();
    int row = 0;
    std::string keyword = search.toLower().toStdString();

    while(node != nullptr) {
        Booking b = node->getData();

        // Lấy thông tin phụ để tìm kiếm
        Client c = m_accountRepo->getClientInfo(b.getClientId());
        Service s = m_serviceRepo->getServiceInfo(b.getServiceId());

        std::string id = b.getBookingId(); std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        std::string custName = c.getName(); std::transform(custName.begin(), custName.end(), custName.begin(), ::tolower);
        std::string date = b.getDate();

        // Chỉ hiện Completed hoặc Cancelled trong lịch sử (tùy bạn, ở đây tôi hiện tất cả để Admin dễ quản lý)
        if (keyword.empty() || id.find(keyword) != std::string::npos ||
            custName.find(keyword) != std::string::npos || date.find(keyword) != std::string::npos) {

            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBookingId())));
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName())));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));
            table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getDate())));
            table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(b.getTime())));

            // Nút View Booking
            QPushButton* viewBtn = new QPushButton("View");
            viewBtn->setCursor(Qt::PointingHandCursor);
            // Đổi màu nút dựa trên trạng thái
            if(b.getStatus() == "Completed")
                viewBtn->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 3px;"); // Xanh
            else if(b.getStatus() == "Cancelled")
                viewBtn->setStyleSheet("background-color: #F44336; color: white; border-radius: 3px;"); // Đỏ
            else
                viewBtn->setStyleSheet("background-color: #FF9800; color: white; border-radius: 3px;"); // Cam (Pending/Confirmed)

            viewBtn->setText(QString::fromStdString(b.getStatus())); // Hiện trạng thái lên nút luôn cho gọn, hoặc để chữ View

            std::string bookingIdStr = b.getBookingId();
            connect(viewBtn, &QPushButton::clicked, [this, bookingIdStr]() {
                showBookingDetail(bookingIdStr);
            });
            table->setCellWidget(row, 5, viewBtn);
            row++;
        }
        node = node->getNext();
    }
}
void AdminDashboard::showBookingDetail(const std::string& bookingId) {
    // Tìm thông tin booking
    Booking b;
    bool found = false;
    LinkedList<Booking> list = m_bookingRepo->getAllBookings();
    Node<Booking>* node = list.getHead();
    while(node){
        if(node->getData().getBookingId() == bookingId) {
            b = node->getData();
            found = true;
            break;
        }
        node = node->getNext();
    }

    if (!found) return;

    Client c = m_accountRepo->getClientInfo(b.getClientId());
    Service s = m_serviceRepo->getServiceInfo(b.getServiceId());

    QString detail = QString("--- BOOKING DETAIL ---\n\n"
                             "ID: %1\n"
                             "Status: %2\n\n"
                             "Customer: %3\n"
                             "Phone: %4\n\n"
                             "Service: %5\n"
                             "Price: %6 VND\n"
                             "Duration: %7 mins\n\n"
                             "Date: %8\n"
                             "Time: %9")
                         .arg(QString::fromStdString(b.getBookingId()))
                         .arg(QString::fromStdString(b.getStatus()))
                         .arg(QString::fromStdString(c.getName()))
                         .arg(QString::fromStdString(c.getId()))
                         .arg(QString::fromStdString(s.getName()))
                         .arg(s.getPrice())
                         .arg(s.getDuration())
                         .arg(QString::fromStdString(b.getDate()))
                         .arg(QString::fromStdString(b.getTime()));

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Booking Detail");
    msgBox.setText(detail);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}
