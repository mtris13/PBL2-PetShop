#include "staffdashboard.h"
#include "ui_staffdashboard.h"
#include <QDebug>
#include <QMessageBox> // Nhớ include cái này
#include "billdialog.h"
StaffDashboard::StaffDashboard(Staff* staff,
                               AccountRepository* accountRepo,
                               PetRepository* petRepo,
                               CartRepository* cartRepo,
                               ServiceRepository* serviceRepo,
                               BookingRepository* bookingRepo,
                               BillRepository* billRepo,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaffDashboard),
    m_currentStaff(staff),
    m_accountRepo(accountRepo),
    m_petRepo(petRepo),
    m_cartRepo(cartRepo),
    m_serviceRepo(serviceRepo),
    m_bookingRepo(bookingRepo),
    m_billRepo(billRepo),
    m_currentPosClient(nullptr),
m_currentSpaClient(nullptr),
    m_currentPosFilter("Dog"),
    m_currentPosKeyword("")
{
    ui->setupUi(this);

    ui->userNameLabel->setText(QString::fromStdString(m_currentStaff->getName()));
    ui->welcomeTitleLabel->setText("Welcome, " + QString::fromStdString(m_currentStaff->getName()));
    ui->posDogButton->setCheckable(true);
    ui->posCatButton->setCheckable(true);
    ui->posDogButton->setChecked(true);

    ui->spaDateEdit->setDisplayFormat("d/M/yyyy");
    ui->spaDateEdit->setDate(QDate::currentDate());
    ui->spaDateEdit->setCalendarPopup(true);

    ui->spaTimeEdit->setDisplayFormat("HH:mm");
    ui->spaTimeEdit->setTime(QTime::currentTime());
    m_posTypeGroup = new QButtonGroup(this);
    m_posTypeGroup->addButton(ui->posDogButton);
    m_posTypeGroup->addButton(ui->posCatButton);
    m_posTypeGroup->setExclusive(true);
    m_navGroup = new QButtonGroup(this);
    m_navGroup->addButton(ui->posPetsButton);
    m_navGroup->addButton(ui->posSpaButton);
    m_navGroup->addButton(ui->manageBookingsButton);
    m_navGroup->addButton(ui->historyButton);
    m_navGroup->addButton(ui->createClientButton);
    m_navGroup->addButton(ui->profileButton);
    m_navGroup->setExclusive(true);

    connect(ui->posPetsButton, &QPushButton::clicked, this, &StaffDashboard::handlePosPetsClick);
    connect(ui->posSpaButton, &QPushButton::clicked, this, &StaffDashboard::handlePosSpaClick);
    connect(ui->manageBookingsButton, &QPushButton::clicked, this, &StaffDashboard::handleManageBookingsClick);
    connect(ui->historyButton, &QPushButton::clicked, this, &StaffDashboard::handleHistoryClick);
    connect(ui->createClientButton, &QPushButton::clicked, this, &StaffDashboard::handleCreateClientClick);
    connect(ui->profileButton, &QPushButton::clicked, this, &StaffDashboard::handleProfileClick);
    connect(ui->logoutButton, &QPushButton::clicked, this, &StaffDashboard::handleLogoutClick);

    connect(ui->posDogButton, &QPushButton::clicked, this, &StaffDashboard::on_posDogButton_clicked);
    connect(ui->posCatButton, &QPushButton::clicked, this, &StaffDashboard::on_posCatButton_clicked);
    connect(ui->posSearchInput, &QLineEdit::textChanged, this, &StaffDashboard::on_posSearchInput_textChanged);
    connect(ui->spaCheckButton, &QPushButton::clicked, this, &StaffDashboard::on_spaCheckButton_clicked);

    connect(ui->posSpaTable, &QTableWidget::itemChanged, this, &StaffDashboard::on_posSpaTable_itemChanged);

    connect(ui->historySearchButton, &QPushButton::clicked, this, &StaffDashboard::on_historySearchButton_clicked);

    ui->bookingHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bookingHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->salesHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->salesHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->posSpaTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->posSpaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pendingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pendingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pendingTable->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->confirmedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->confirmedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->confirmedTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->posPetsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->posPetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->posPetsTable->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->posPetsButton->setChecked(true);
    handlePosPetsClick();
    loadPosPetsTable();
    loadSpaTable();
    loadBookingTables();
}

StaffDashboard::~StaffDashboard()
{
    delete ui;
}

void StaffDashboard::handlePosPetsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->posPetsPage);
}

void StaffDashboard::handlePosSpaClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->posSpaPage);
}

void StaffDashboard::handleManageBookingsClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->manageBookingPage);
    loadBookingTables();
}

void StaffDashboard::handleHistoryClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->historyPage);
    loadHistoryTables();
}

void StaffDashboard::handleCreateClientClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->createClientPage);
}

void StaffDashboard::handleProfileClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->profilePage);
    ui->profileStackedWidget->setCurrentWidget(ui->profileViewPage);
    loadProfileData();
}

void StaffDashboard::handleLogoutClick() {
    emit logoutSignal();
    this->close();
}
void StaffDashboard::on_saveClientButton_clicked()
{
    std::string id = ui->clientIdInput->text().toStdString();
    std::string name = ui->clientNameInput->text().toStdString();
    std::string pass = ui->clientPassInput->text().toStdString();
    std::string address = ui->clientAddressInput->text().toStdString();
    std::string city = ui->clientCityInput->text().toStdString();

    std::string gender = "Unknown";
    if (ui->maleRadio->isChecked()) gender = "Male";
    else if (ui->femaleRadio->isChecked()) gender = "Female";

    if (id.empty() || name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Lack of information", "Please enter at least ID, Name and Password.");
        return;
    }
    if (id.length() != 10) {
        QMessageBox::warning(this, "ID error", "Phone number must be 10 digits.");
        return;
    }

    if (m_accountRepo->isValidId(id)) {
        QMessageBox::warning(this, "Duplicate", "This customer already has an account in the system.");
        return;
    }

    try {
        Client newClient(id, name, pass, gender, address, city);
        m_accountRepo->registerClient(newClient);

        QMessageBox::information(this, "Success", "Customer account created successfully!");

        ui->clientIdInput->clear();
        ui->clientNameInput->clear();
        ui->clientPassInput->setText("123");
        ui->clientAddressInput->clear();
        ui->clientCityInput->clear();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}
void StaffDashboard::loadPosPetsTable()
{
    ui->posPetsTable->blockSignals(true);
    ui->posPetsTable->setRowCount(0);
    ui->posPetsTable->setColumnCount(6);
    QStringList headers = {"Choose", "ID", "Name", "Breed", "Price", "Status"};
    ui->posPetsTable->setHorizontalHeaderLabels(headers);

    int row = 0;
    std::string keyword = m_currentPosKeyword.toStdString();

    if (m_currentPosFilter == "Dog")
    {
        LinkedList<Dog> dogs;
        if (m_currentPosKeyword.isEmpty()) {
            dogs = m_petRepo->getAllDogInfoAvailable();
        } else {
            dogs = m_petRepo->searchDog("all", keyword);
        }

        Node<Dog>* dNode = dogs.getHead();
        while(dNode != nullptr) {
            Dog p = dNode->getData();
            if (p.getStatus() == "available") {
                ui->posPetsTable->insertRow(row);

                QTableWidgetItem* check = new QTableWidgetItem();
                QString idStr = QString::fromStdString(p.getId());

                if (m_selectedPetIds.contains(idStr)) {
                    check->setCheckState(Qt::Checked);
                } else {
                    check->setCheckState(Qt::Unchecked);
                }
                ui->posPetsTable->setItem(row, 0, check);

                ui->posPetsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.getId())));
                ui->posPetsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(p.getName())));
                ui->posPetsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p.getBreed())));
                ui->posPetsTable->setItem(row, 4, new QTableWidgetItem(QString::number(p.getPrice())));
                ui->posPetsTable->setItem(row, 5, new QTableWidgetItem("Dog"));
                row++;
            }
            dNode = dNode->getNext();
        }
    }
    else
    {
        LinkedList<Cat> cats;
        if (m_currentPosKeyword.isEmpty()) {
            cats = m_petRepo->getAllCatInfoAvailable();
        } else {
            cats = m_petRepo->searchCat("all", keyword);
        }

        Node<Cat>* cNode = cats.getHead();
        while(cNode != nullptr) {
            Cat p = cNode->getData();
            if (p.getStatus() == "available") {
                ui->posPetsTable->insertRow(row);

                QTableWidgetItem* check = new QTableWidgetItem();
                QString idStr = QString::fromStdString(p.getId());
                if (m_selectedPetIds.contains(idStr)) {
                    check->setCheckState(Qt::Checked);
                } else {
                    check->setCheckState(Qt::Unchecked);
                }
                ui->posPetsTable->setItem(row, 0, check);

                ui->posPetsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.getId())));
                ui->posPetsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(p.getName())));
                ui->posPetsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p.getBreed())));
                ui->posPetsTable->setItem(row, 4, new QTableWidgetItem(QString::number(p.getPrice())));
                ui->posPetsTable->setItem(row, 5, new QTableWidgetItem("Cat"));
                row++;
            }
            cNode = cNode->getNext();
        }
    }

    ui->posPetsTable->resizeColumnsToContents();
    ui->posPetsTable->blockSignals(false);
}
void StaffDashboard::on_posCheckButton_clicked()
{
    std::string phone = ui->posPhoneInput->text().toStdString();

    if (phone.empty()) {
        QMessageBox::warning(this, "Error", "Please enter Phone Number.");
        return;
    }

    Client c = m_accountRepo->getClientInfo(phone);

    if (c.getId().empty()) {
        ui->posClientNameLabel->setText("Customer: CAN'T FOUND");
        ui->posClientNameLabel->setStyleSheet("color: red; font-weight: bold;");

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "New customer", "Customer does not have an account. Create a new one now?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ui->pageStackedWidget->setCurrentWidget(ui->createClientPage);
            ui->clientIdInput->setText(QString::fromStdString(phone));
        }

        if (m_currentPosClient) delete m_currentPosClient;
        m_currentPosClient = nullptr;

    } else {
        ui->posClientNameLabel->setText("Customer: " + QString::fromStdString(c.getName()));
        ui->posClientNameLabel->setStyleSheet("color: green; font-weight: bold;");

        if (m_currentPosClient) delete m_currentPosClient;
        m_currentPosClient = new Client(c);
    }
}
void StaffDashboard::on_posPetsTable_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        QString id = ui->posPetsTable->item(item->row(), 1)->text();

        if (item->checkState() == Qt::Checked) {
            m_selectedPetIds.insert(id);
        } else {
            m_selectedPetIds.remove(id);
        }

        calculatePosTotal();
    }
}

void StaffDashboard::calculatePosTotal()
{
    long total = 0;
    ui->posCartList->clear();

    for (const QString& idStr : m_selectedPetIds) {
        std::string id = idStr.toStdString();
        std::string name;
        long price = 0;

        if (id[0] == 'd') {
            Dog d = m_petRepo->getDogInfo(id);
            name = d.getName();
            price = d.getPrice();
        } else {
            Cat c = m_petRepo->getCatInfo(id);
            name = c.getName();
            price = c.getPrice();
        }

        ui->posCartList->addItem(QString::fromStdString(name) + " - " + QString::number(price) + " VND");
        total += price;
    }

    ui->posTotalLabel->setText("TOTAL: " + QString::number(total) + " VND");
}
void StaffDashboard::on_posPayButton_clicked()
{
    if (m_currentPosClient == nullptr) {
        QMessageBox::warning(this, "Lack of information", "Please enter phone number and find to select customer.");
        return;
    }

    if (ui->posCartList->count() == 0) {
        QMessageBox::warning(this, "Cart is empty", "Please select at least 1 pet.");
        return;
    }

    LinkedList<BillItem> billItems;
    for (const QString& idStr : m_selectedPetIds) {
        std::string id = idStr.toStdString();
        std::string name;
        long price = 0;

        if (id[0] == 'd') {
            Dog d = m_petRepo->getDogInfo(id);
            name = d.getName();
            price = d.getPrice();
        } else {
            Cat c = m_petRepo->getCatInfo(id);
            name = c.getName();
            price = c.getPrice();
        }

        billItems.pushBack(BillItem(id, name, "Pet", price));
        m_petRepo->setStatusUnavailable(id);
    }

    try {
        Bill bill = m_billRepo->createBillFromCart(
            m_currentPosClient->getId(),
            m_currentPosClient->getName(),
            billItems
            );
        m_billRepo->saveBill(bill);

        QMessageBox::information(this, "Success", "Payment successful! Bill ID: " + QString::fromStdString(bill.getBillId()));

        m_selectedPetIds.clear();
        loadPosPetsTable();
        ui->posCartList->clear();
        ui->posTotalLabel->setText("TOTAL: 0 VND");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}
void StaffDashboard::on_posDogButton_clicked()
{
    m_currentPosFilter = "Dog";
    loadPosPetsTable();
}

void StaffDashboard::on_posCatButton_clicked()
{
    m_currentPosFilter = "Cat";
    loadPosPetsTable();
}

void StaffDashboard::on_posSearchInput_textChanged(const QString &text)
{
    m_currentPosKeyword = text.toLower();
    loadPosPetsTable();
}
void StaffDashboard::loadSpaTable()
{
    ui->posSpaTable->blockSignals(true);
    ui->posSpaTable->setRowCount(0);
    ui->posSpaTable->setColumnCount(5);

    QStringList headers = {"Choose", "ID", "Name", "Price (VND)", "Time"};
    ui->posSpaTable->setHorizontalHeaderLabels(headers);

    LinkedList<Service> services = m_serviceRepo->getAllServices();

    int row = 0;
    Node<Service>* current = services.getHead();

    while(current != nullptr) {
        Service s = current->getData();
        ui->posSpaTable->insertRow(row);

        QTableWidgetItem* check = new QTableWidgetItem();
        check->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (m_selectedServiceIds.contains(QString::fromStdString(s.getId())))
            check->setCheckState(Qt::Checked);
        else
            check->setCheckState(Qt::Unchecked);
        ui->posSpaTable->setItem(row, 0, check);

        ui->posSpaTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s.getId())));
        ui->posSpaTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));
        ui->posSpaTable->setItem(row, 3, new QTableWidgetItem(QString::number(s.getPrice())));
        ui->posSpaTable->setItem(row, 4, new QTableWidgetItem(QString::number(s.getDuration()) + " minutes"));

        current = current->getNext();
        row++;
    }

    ui->posSpaTable->resizeColumnsToContents();
    ui->posSpaTable->blockSignals(false);
}
void StaffDashboard::on_spaCheckButton_clicked()
{
    std::string phone = ui->spaPhoneInput->text().toStdString();

    if (phone.empty()) {
        QMessageBox::warning(this, "Error", "Please enter Phone Number.");
        return;
    }

    Client c = m_accountRepo->getClientInfo(phone);

    if (c.getId().empty()) {
        ui->spaClientNameLabel->setText("Customer: NOT FOUND");
        ui->spaClientNameLabel->setStyleSheet("color: red; font-weight: bold;");

        if (m_currentSpaClient) { delete m_currentSpaClient; m_currentSpaClient = nullptr; }

        QMessageBox::StandardButton reply = QMessageBox::question(this, "New customer",
                                                                  "Customer does not have an account. Create a new one now?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ui->pageStackedWidget->setCurrentWidget(ui->createClientPage);
            ui->clientIdInput->setText(QString::fromStdString(phone));
        }
    } else {
        ui->spaClientNameLabel->setText("Customer: " + QString::fromStdString(c.getName()));
        ui->spaClientNameLabel->setStyleSheet("color: green; font-weight: bold;");

        if (m_currentSpaClient) delete m_currentSpaClient;
        m_currentSpaClient = new Client(c);
    }
}

void StaffDashboard::on_posSpaTable_itemChanged(QTableWidgetItem *item)
{
    if (item->column() != 0) return;

    QString id = ui->posSpaTable->item(item->row(), 1)->text();

    if (item->checkState() == Qt::Checked) {

        ui->posSpaTable->blockSignals(true);

        for (int i = 0; i < ui->posSpaTable->rowCount(); i++) {
            QTableWidgetItem* otherItem = ui->posSpaTable->item(i, 0);
            if (otherItem != item) {
                otherItem->setCheckState(Qt::Unchecked);
            }
        }

        m_selectedServiceIds.clear();
        m_selectedServiceIds.insert(id);

        ui->posSpaTable->blockSignals(false);
    }
    else {
        m_selectedServiceIds.remove(id);
    }

    calculateSpaTotal();
}


void StaffDashboard::calculateSpaTotal()
{
    long total = 0;

    ui->spaCartList->clear();

    for (const QString& idStr : m_selectedServiceIds) {
        std::string id = idStr.toStdString();
        Service s = m_serviceRepo->getServiceInfo(id);

        ui->spaCartList->addItem(QString::fromStdString(s.getName()) + "\n" + QString::number(s.getPrice()) + " VND");

        total += s.getPrice();
    }

    ui->spaTotalLabel->setText("TOTAL: " + QString::number(total) + " VND");
}
void StaffDashboard::on_spaBookButton_clicked()
{
    if (m_currentSpaClient == nullptr) {
        QMessageBox::warning(this, "No guests selected yet", "Please select a customer first.");
        return;
    }
    if (m_selectedServiceIds.isEmpty()) {
        QMessageBox::warning(this, "No service selected yet", "Please select at least 1 service.");
        return;
    }

    std::string date = ui->spaDateEdit->date().toString("dd/MM/yyyy").toStdString();
    std::string time = ui->spaTimeEdit->time().toString("hh:mm").toStdString();

    if (!m_bookingRepo->isTimeSlotAvailable(date, time)) {
        QMessageBox::warning(this, "Duplicate", "This time is taken. Please choose another time.");
        return;
    }

    try {
        int count = 0;
        for (const QString& idStr : m_selectedServiceIds) {
            std::string serviceId = idStr.toStdString();
            std::string bookingId = m_bookingRepo->generateBookingId();

            Booking newBooking(bookingId, m_currentSpaClient->getId(), "N/A", serviceId, date, time, "Confirmed");
            m_bookingRepo->createBooking(newBooking);
            count++;
        }

        QMessageBox::information(this, "Success",
                                 QString("Successfully booked %1 service for customer %2 at %3 on %4.")
                                     .arg(count)
                                     .arg(QString::fromStdString(m_currentSpaClient->getName()))
                                     .arg(QString::fromStdString(time))
                                     .arg(QString::fromStdString(date)));

        m_selectedServiceIds.clear();
        loadSpaTable();
        ui->spaTotalLabel->setText("TOTAL: 0 VND");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}
void StaffDashboard::loadBookingTables()
{
    ui->pendingTable->setRowCount(0);
    ui->pendingTable->setColumnCount(5);
    QStringList headers = {"ID", "Customer", "Service", "Date", "Time"};
    ui->pendingTable->setHorizontalHeaderLabels(headers);

    LinkedList<Booking> pendingList = m_bookingRepo->getBookingsByStatus("Pending");
    Node<Booking>* pNode = pendingList.getHead();
    int row = 0;

    while(pNode != nullptr) {
        Booking b = pNode->getData();
        ui->pendingTable->insertRow(row);

        ui->pendingTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBookingId())));

        Client c = m_accountRepo->getClientInfo(b.getClientId());
        ui->pendingTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName())));

        Service s = m_serviceRepo->getServiceInfo(b.getServiceId());
        ui->pendingTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));

        ui->pendingTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getDate())));
        ui->pendingTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(b.getTime())));

        pNode = pNode->getNext();
        row++;
    }
    ui->pendingTable->resizeColumnsToContents();


    ui->confirmedTable->setRowCount(0);
    ui->confirmedTable->setColumnCount(5);
    ui->confirmedTable->setHorizontalHeaderLabels(headers);

    LinkedList<Booking> confirmedList = m_bookingRepo->getBookingsByStatus("Confirmed");
    Node<Booking>* cNode = confirmedList.getHead();
    row = 0;

    while(cNode != nullptr) {
        Booking b = cNode->getData();
        ui->confirmedTable->insertRow(row);

        ui->confirmedTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBookingId())));

        Client c = m_accountRepo->getClientInfo(b.getClientId());
        ui->confirmedTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName())));

        Service s = m_serviceRepo->getServiceInfo(b.getServiceId());
        ui->confirmedTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));

        ui->confirmedTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getDate())));
        ui->confirmedTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(b.getTime())));

        cNode = cNode->getNext();
        row++;
    }
    ui->confirmedTable->resizeColumnsToContents();
}

void StaffDashboard::on_cancelBookingButton_clicked()
{
    int currentRow = ui->pendingTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select a booking to cancel.");
        return;
    }

    std::string bookingId = ui->pendingTable->item(currentRow, 0)->text().toStdString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", "Are you sure you want to cancel this booking?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_bookingRepo->updateStatus(bookingId, "Cancelled");
        loadBookingTables();
        QMessageBox::information(this, "Success", "Booking cancelled.");
    }
}

void StaffDashboard::on_confirmBookingButton_clicked()
{
    int currentRow = ui->pendingTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select a booking schedule to confirm.");
        return;
    }

    std::string bookingId = ui->pendingTable->item(currentRow, 0)->text().toStdString();

    m_bookingRepo->updateStatus(bookingId, "Confirmed");

    loadBookingTables();
    QMessageBox::information(this, "Success", "The booking has been confirmed and moved to the 'Confirmed' tab.");
}


void StaffDashboard::on_completeBookingButton_clicked()
{
    int currentRow = ui->confirmedTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select a booking schedule to complete.");
        return;
    }

    std::string bookingId = ui->confirmedTable->item(currentRow, 0)->text().toStdString();

    m_bookingRepo->updateStatus(bookingId, "Completed");


    loadBookingTables();
    QMessageBox::information(this, "Success", "Service completed! Status updated.");
}

void StaffDashboard::loadHistoryTables() {
    std::string filter = ui->historySearchInput->text().toStdString();

    loadBookingHistoryTable(filter);
    loadSalesHistoryTable(filter);
}

void StaffDashboard::on_historySearchButton_clicked() {
    loadHistoryTables();
}

void StaffDashboard::loadBookingHistoryTable(const std::string& phoneFilter) {
    ui->bookingHistoryTable->setRowCount(0);
    ui->bookingHistoryTable->setColumnCount(6);
    QStringList headers = {"ID", "Customer", "Service", "Date", "Time", "Status"};
    ui->bookingHistoryTable->setHorizontalHeaderLabels(headers);

    LinkedList<Booking> allBookings = m_bookingRepo->getAllBookings();
    Node<Booking>* node = allBookings.getHead();
    int row = 0;

    while (node != nullptr) {
        Booking b = node->getData();

        if (b.getStatus() == "Completed" || b.getStatus() == "Cancelled") {

            if (!phoneFilter.empty() && b.getClientId().find(phoneFilter) == std::string::npos) {
                node = node->getNext();
                continue;
            }

            ui->bookingHistoryTable->insertRow(row);
            ui->bookingHistoryTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBookingId())));

            Client c = m_accountRepo->getClientInfo(b.getClientId());
            ui->bookingHistoryTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName()) + "\n(" + QString::fromStdString(b.getClientId()) + ")"));

            Service s = m_serviceRepo->getServiceInfo(b.getServiceId());
            ui->bookingHistoryTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));

            ui->bookingHistoryTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getDate())));
            ui->bookingHistoryTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(b.getTime())));

            QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(b.getStatus()));
            if (b.getStatus() == "Completed") statusItem->setForeground(QBrush(QColor("green")));
            else statusItem->setForeground(QBrush(QColor("red")));
            ui->bookingHistoryTable->setItem(row, 5, statusItem);

            row++;
        }
        node = node->getNext();
    }
    ui->bookingHistoryTable->resizeColumnsToContents();
}

void StaffDashboard::loadSalesHistoryTable(const std::string& phoneFilter) {
    ui->salesHistoryTable->setRowCount(0);
    ui->salesHistoryTable->setColumnCount(5);
    QStringList headers = {"Bill ID", "Customer", "Date", "Time", "Total"};
    ui->salesHistoryTable->setHorizontalHeaderLabels(headers);

    LinkedList<Bill> allBills = m_billRepo->getAllBills();
    Node<Bill>* node = allBills.getHead();
    int row = 0;

    while (node != nullptr) {
        Bill b = node->getData();

        if (!phoneFilter.empty() && b.getClientId().find(phoneFilter) == std::string::npos) {
            node = node->getNext();
            continue;
        }

        ui->salesHistoryTable->insertRow(row);
        ui->salesHistoryTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBillId())));

        ui->salesHistoryTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.getClientName()) + "\n(" + QString::fromStdString(b.getClientId()) + ")"));

        ui->salesHistoryTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.getDate())));
        ui->salesHistoryTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getTime())));

        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(b.getTotalAmount()) + " VND");
        priceItem->setForeground(QBrush(QColor("yellow")));
        priceItem->setFont(QFont("Arial", 9, QFont::Bold));
        ui->salesHistoryTable->setItem(row, 4, priceItem);

        row++;
        node = node->getNext();
    }
    ui->salesHistoryTable->resizeColumnsToContents();
}
void StaffDashboard::loadProfileData()
{
    Staff staff = m_accountRepo->getStaffInfo(m_currentStaff->getId());

    ui->valStaffId->setText("<b>ID:</b> " + QString::fromStdString(staff.getId()));
    ui->valStaffName->setText("<b>Name:</b> " + QString::fromStdString(staff.getName()));
    ui->valStaffGender->setText("<b>Gender:</b> " + QString::fromStdString(staff.getGender()));
    ui->valStaffRole->setText("<b>Role:</b> Staff");


    ui->valStaffId->setTextFormat(Qt::RichText);
    ui->valStaffName->setTextFormat(Qt::RichText);
    ui->valStaffGender->setTextFormat(Qt::RichText);
    ui->valStaffRole->setTextFormat(Qt::RichText);
}

void StaffDashboard::on_editProfileButton_clicked()
{
    Staff s = m_accountRepo->getStaffInfo(m_currentStaff->getId());

    ui->editStaffIdInput->setText(QString::fromStdString(s.getId()));
    ui->editStaffNameInput->setText(QString::fromStdString(s.getName()));
    ui->editStaffPassInput->setText(QString::fromStdString(s.getPassword()));

    if (s.getGender() == "Male") ui->editStaffMale->setChecked(true);
    else ui->editStaffFemale->setChecked(true);

    ui->profileStackedWidget->setCurrentWidget(ui->profileEditPage);
}

void StaffDashboard::on_saveProfileButton_clicked()
{
    std::string id = ui->editStaffIdInput->text().toStdString();
    std::string name = ui->editStaffNameInput->text().toStdString();
    std::string pass = ui->editStaffPassInput->text().toStdString();
    std::string gender = ui->editStaffMale->isChecked() ? "Male" : "Female";

    if (name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Error", "Name and Password cannot be blank.");
        return;
    }

    Staff oldStaff = m_accountRepo->getStaffInfo(id);
    long salary = oldStaff.getSalary();

    Staff updatedStaff(id, name, pass, gender, salary);

    try {
        m_accountRepo->setStaffInfo(updatedStaff);

        ui->userNameLabel->setText(QString::fromStdString(name));
        ui->welcomeTitleLabel->setText("Welcome, " + QString::fromStdString(name));

        QMessageBox::information(this, "Success", "Profile updated successfully!");

        ui->profileStackedWidget->setCurrentWidget(ui->profileViewPage);
        loadProfileData();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void StaffDashboard::on_cancelProfileButton_clicked()
{
    ui->profileStackedWidget->setCurrentWidget(ui->profileViewPage);
}
