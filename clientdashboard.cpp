#include "clientdashboard.h"
#include "ui_clientdashboard.h"
#include "ds/LinkedList.hpp"
#include "domain/entities/Dog.hpp"
#include "domain/entities/Cat.hpp"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDateTime>
#include "billdialog.h"
ClientDashboard::ClientDashboard(Client* client,
                                 AccountRepository* accountRepo,
                                 PetRepository* petRepo,
                                 CartRepository* cartRepo,
                                 ServiceRepository* serviceRepo,
                                 BookingRepository* bookingRepo,
                                 BillRepository* billRepo,
                                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientDashboard),
    m_currentClient(client),
    m_accountRepo(accountRepo),
    m_petRepo(petRepo),
    m_cartRepo(cartRepo),
    m_serviceRepo(serviceRepo),
    m_bookingRepo(bookingRepo),
    m_billRepo(billRepo),
    m_currentPetTypeFilter("Dog"),
    m_currentSearchKeyword("")
{
    ui->setupUi(this);
    // Thêm vào clientdashboard.cpp sau ui->setupUi(this);

    // Cập nhật StyleSheet cho Navigation Bar
    QString currentStyle = this->styleSheet();
    QString navStyle = R"(
        /* Style chung cho các nút trong NavBar */
        QWidget#navBar QPushButton {
            text-align: left;
            padding-left: 20px;
            border: none;
            color: #333333;
            background-color: transparent;
            font-size: 16px;
        }

        /* Hiệu ứng khi di chuột vào */
        QWidget#navBar QPushButton:hover {
            background-color: #FBCB2E;
        }

        /* QUAN TRỌNG: Style khi nút đang được CHỌN (Active) */
        QWidget#navBar QPushButton:checked {
            background-color: #FFFFFF;    /* Nền trắng nối liền với nội dung bên phải */
            color: #FDD85D;               /* Chữ màu vàng thương hiệu (hoặc màu đen đậm nếu bạn thích) */
            font-weight: bold;            /* Chữ đậm */
            border-left: 5px solid #FDD85D; /* (Tùy chọn) Thêm viền trái để nhấn mạnh */
        }
    )";

    // Gộp style cũ và style mới (nếu bạn muốn giữ style cũ, hoặc chỉ set navStyle nếu muốn thay thế phần nav)
    ui->navBar->setStyleSheet(navStyle);

    // Xóa style riêng lẻ của petsButton để nó tuân theo style chung
    ui->petsButton->setStyleSheet("");
    setupHelpPage();


    QString welcomeText = QString("Welcome back, %1")
                              .arg(QString::fromStdString(m_currentClient->getName()));
    ui->welcomeTitleLabel->setText(welcomeText);
    ui->welcomeSubtitleLabel->setText("Let's explore new pets and special offers waiting for you!");
    ui->userNameLabel->setText(QString::fromStdString(m_currentClient->getName()));
    ui->userRoleLabel->setText("Client");

    connect(ui->petsButton, &QPushButton::clicked, this, &ClientDashboard::handlePetsButtonClick);
    connect(ui->spaButton, &QPushButton::clicked, this, &ClientDashboard::handleSpaButtonClick);
    connect(ui->bookingsButton, &QPushButton::clicked, this, &ClientDashboard::handleBookingsButtonClick);
    connect(ui->cartButton, &QPushButton::clicked, this, &ClientDashboard::handleCartButtonClick);
    connect(ui->historyButton, &QPushButton::clicked, this, &ClientDashboard::handleHistoryButtonClick);
    connect(ui->profileButton, &QPushButton::clicked, this, &ClientDashboard::handleProfileButtonClick);
    connect(ui->callButton, &QPushButton::clicked, this, &ClientDashboard::handleCallButtonClick);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ClientDashboard::handleLogoutButtonClick);
    connect(ui->removeButton, &QPushButton::clicked, this, &ClientDashboard::handleRemoveFromCart);
    connect(ui->buyButton, &QPushButton::clicked, this, &ClientDashboard::handleBuy);
    connect(ui->bookSpaButton, &QPushButton::clicked, this, &ClientDashboard::handleBookSpaClick);
    connect(ui->cancelBookingButton, &QPushButton::clicked, this, &ClientDashboard::handleCancelBookingClick);
    connect(ui->editProfileButton, &QPushButton::clicked, this, &ClientDashboard::on_editProfileButton_clicked);
    connect(ui->cancelProfileButton, &QPushButton::clicked, this, &ClientDashboard::on_cancelProfileButton_clicked);



    ui->dogToggleButton->setCheckable(true);
    ui->catToggleButton->setCheckable(true);
    ui->purchaseHistoryButton->setCheckable(true);
    ui->bookingHistoryButton->setCheckable(true);

    m_petTypeGroup = new QButtonGroup(this);

    m_petTypeGroup->addButton(ui->dogToggleButton);
    m_petTypeGroup->addButton(ui->catToggleButton);
    m_petTypeGroup->setExclusive(true);

    m_historyGroup = new QButtonGroup(this);
    m_historyGroup->addButton(ui->purchaseHistoryButton);
    m_historyGroup->addButton(ui->bookingHistoryButton);
    m_historyGroup->setExclusive(true);

    m_navGroup = new QButtonGroup(this);
    m_navGroup->addButton(ui->petsButton);
    m_navGroup->addButton(ui->spaButton);
    m_navGroup->addButton(ui->cartButton);
    m_navGroup->addButton(ui->bookingsButton);
    m_navGroup->addButton(ui->historyButton);
    m_navGroup->addButton(ui->profileButton);
    m_navGroup->addButton(ui->callButton);

    // Đảm bảo chỉ 1 nút được chọn tại 1 thời điểm
    m_navGroup->setExclusive(true);

    // 2. Bật chế độ checkable cho tất cả các nút này
    QList<QAbstractButton*> buttons = m_navGroup->buttons();
    for(QAbstractButton* btn : buttons) {
        btn->setCheckable(true);
    }

    // 3. Mặc định chọn trang Pets đầu tiên
    ui->petsButton->setChecked(true);


    ui->dogToggleButton->setChecked(true);
    ui->purchaseHistoryButton->setChecked(true);

    ui->petsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->petsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->petsTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);


    ui->petsTableWidget->setColumnWidth(0, 30);
    ui->petsTableWidget->setColumnWidth(1, 60);
    ui->petsTableWidget->setColumnWidth(2, 120);
    ui->petsTableWidget->setColumnWidth(3, 150);
    ui->petsTableWidget->setColumnWidth(4, 50);
    ui->petsTableWidget->setColumnWidth(5, 100);
    ui->petsTableWidget->setColumnWidth(6, 80);
    ui->petsTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->spaTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->spaTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->spaTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);


    ui->spaTableWidget->setColumnWidth(0, 50);
    ui->spaTableWidget->setColumnWidth(1, 80);
    ui->spaTableWidget->setColumnWidth(2, 200);
    ui->spaTableWidget->setColumnWidth(3, 250);
    ui->spaTableWidget->setColumnWidth(4, 100);
    ui->spaTableWidget->setColumnWidth(5, 100);

    handlePetsButtonClick();
    ui->bookingDateEdit->setDisplayFormat("d/M/yyyy");
    ui->bookingDateEdit->setDate(QDate::currentDate());
    ui->bookingDateEdit->setCalendarPopup(true);

    ui->bookingTimeEdit->setDisplayFormat("HH:mm");
    ui->bookingTimeEdit->setTime(QTime::currentTime());
}

ClientDashboard::~ClientDashboard()
{
    delete ui;
}

void ClientDashboard::updatePetsTable()
{
    ui->petsTableWidget->setRowCount(0);

    ui->petsTableWidget->setColumnCount(8);

    std::string keyword_std = m_currentSearchKeyword.toStdString();
    int row = 0;

    if (m_currentPetTypeFilter == "Dog")
    {

        QStringList headers = {"Choose", "ID", "Name", "Breed", "Age", "Price (VND)", "Energy level", "Description"};
        ui->petsTableWidget->setHorizontalHeaderLabels(headers);


        LinkedList<Dog> dogs = (m_currentSearchKeyword.isEmpty())
                                   ? m_petRepo->getAllDogInfoAvailable()
                                   : m_petRepo->searchDog("all", keyword_std);


        Node<Dog>* dogNode = dogs.getHead();
        while(dogNode != nullptr)
        {
            Dog dog = dogNode->getData();
            if (m_currentSearchKeyword.isEmpty() || dog.getStatus() == "available")
            {
                ui->petsTableWidget->insertRow(row);


                QTableWidgetItem *selectBox = new QTableWidgetItem();
                selectBox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                selectBox->setCheckState(Qt::Unchecked);
                ui->petsTableWidget->setItem(row, 0, selectBox);


                ui->petsTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(dog.getId())));
                ui->petsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(dog.getName())));
                ui->petsTableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(dog.getBreed())));
                ui->petsTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(dog.getAge())));
                ui->petsTableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(dog.getPrice())));
                ui->petsTableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(dog.getEnergyLevel())));
                ui->petsTableWidget->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(dog.getDescription())));
                row++;
            }
            dogNode = dogNode->getNext();
        }
    }
    else
    {

        QStringList headers = {"Choose", "ID", "Name", "Breed", "Age", "Price (VND)", "Fur lenght", "Description"};
        ui->petsTableWidget->setHorizontalHeaderLabels(headers);


        LinkedList<Cat> cats = (m_currentSearchKeyword.isEmpty())
                                   ? m_petRepo->getAllCatInfoAvailable()
                                   : m_petRepo->searchCat("all", keyword_std);


        Node<Cat>* catNode = cats.getHead();
        while(catNode != nullptr)
        {
            Cat cat = catNode->getData();
            if (m_currentSearchKeyword.isEmpty() || cat.getStatus() == "available")
            {
                ui->petsTableWidget->insertRow(row);


                QTableWidgetItem *selectBox = new QTableWidgetItem();
                selectBox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                selectBox->setCheckState(Qt::Unchecked);
                ui->petsTableWidget->setItem(row, 0, selectBox);


                ui->petsTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(cat.getId())));
                ui->petsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(cat.getName())));
                ui->petsTableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(cat.getBreed())));
                ui->petsTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(cat.getAge())));
                ui->petsTableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(cat.getPrice())));
                ui->petsTableWidget->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(cat.getFurLength())));
                ui->petsTableWidget->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(cat.getDescription())));
                row++;
            }
            catNode = catNode->getNext();
        }
    }
}




void ClientDashboard::handlePetsButtonClick()
{
    ui->pageStackedWidget->setCurrentIndex(0);
    updatePetsTable();
    qDebug() << "Đã chuyển sang trang Pets và tải dữ liệu.";
}

void ClientDashboard::handleSpaButtonClick()
{
    ui->pageStackedWidget->setCurrentIndex(1);
    loadSpaDataToTable();
    qDebug() << "Chuyển sang trang Spa Services";
}

void ClientDashboard::handleBookingsButtonClick()
{
    //Lật sang trang Bookings
    ui->pageStackedWidget->setCurrentWidget(ui->bookingsPage);

    //Tải dữ liệu
    loadBookingsDataToTable();

    qDebug() << "Chuyển sang trang My Bookings.";
}
void ClientDashboard::handleCartButtonClick()
{
    ui->pageStackedWidget->setCurrentWidget(ui->cartPage);


    loadCartDataToTable();

    qDebug() << "Chuyển sang trang My Cart và tải dữ liệu.";
}
void ClientDashboard::handleHistoryButtonClick()
{
    ui->pageStackedWidget->setCurrentWidget(ui->historyPage);

    if (ui->purchaseHistoryButton->isChecked()) loadPurchaseHistory();
    else loadBookingHistory();
}
void ClientDashboard::handleProfileButtonClick()
{
    ui->pageStackedWidget->setCurrentWidget(ui->profileViewPage);
    loadProfileData();
}
void ClientDashboard::handleCallButtonClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->helpPage);
    qDebug() << "Nút 'Call Center' đã được bấm!"; }
void ClientDashboard::handleLogoutButtonClick() {
    qDebug() << "Nút 'Log Out' đã được bấm! Đang đóng cửa sổ...";
    emit logoutSignal();
    this->close();
}




void ClientDashboard::on_dogToggleButton_clicked()
{
    m_currentPetTypeFilter = "Dog";
    qDebug() << "Lọc: Chuyển sang xem Chó";
    updatePetsTable();
}

void ClientDashboard::on_catToggleButton_clicked()
{
    m_currentPetTypeFilter = "Cat";
    qDebug() << "Lọc: Chuyển sang xem Mèo";
    updatePetsTable();
}

void ClientDashboard::on_searchLineEdit_textChanged(const QString &text)
{
    m_currentSearchKeyword = text.toLower();
    qDebug() << "Lọc: Tìm kiếm với từ khóa:" << m_currentSearchKeyword;
    updatePetsTable();
}

void ClientDashboard::on_addToCartButton_clicked()
{
    int petsAddedCount = 0;
    std::string clientId = m_currentClient->getId();


    for (int i = 0; i < ui->petsTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem* selectBox = ui->petsTableWidget->item(i, 0);


        if (selectBox != nullptr && selectBox->checkState() == Qt::Checked)
        {

            std::string petId = ui->petsTableWidget->item(i, 1)->text().toStdString();
            std::string petName = ui->petsTableWidget->item(i, 2)->text().toStdString();
            long petPrice = ui->petsTableWidget->item(i, 5)->text().toLong();

            try {
                m_cartRepo->addToCart(clientId, petId, petName, petPrice);
                petsAddedCount++;


                selectBox->setCheckState(Qt::Unchecked);

            } catch (const std::exception& e) {

                QMessageBox::warning(this, "Cart Error", QString("Cannot add %1: %2").arg(QString::fromStdString(petName), e.what()));
            }
        }
    }


    if (petsAddedCount > 0) {
        QMessageBox::information(this, "Success", QString("Added %1 pet to cart!").arg(petsAddedCount));
    } else {
        QMessageBox::warning(this, "Not selected yet", "You have not selected any pets to add to your cart.");
    }
}

void ClientDashboard::loadSpaDataToTable()
{
    ui->spaTableWidget->clearContents();
    ui->spaTableWidget->setRowCount(0);
    ui->spaTableWidget->setColumnCount(6);

    QStringList headers = {"Choose", "ID", "Name", "Description", "Price (VND)", "Time (minutes)"};
    ui->spaTableWidget->setHorizontalHeaderLabels(headers);

    const LinkedList<Service>& services = m_serviceRepo->getAllServices();
    Node<Service>* current = services.getHead();
    int row = 0;

    while (current != nullptr) {
        const Service& service = current->getData();
        ui->spaTableWidget->insertRow(row);

        QTableWidgetItem* selectBox = new QTableWidgetItem();
        selectBox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        selectBox->setCheckState(Qt::Unchecked);
        ui->spaTableWidget->setItem(row, 0, selectBox);

        ui->spaTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(service.getId())));
        ui->spaTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(service.getName())));
        ui->spaTableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(service.getDescription())));
        ui->spaTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(service.getPrice())));
        ui->spaTableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(service.getDuration())));

        current = current->getNext();
        row++;
    }

    ui->spaTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->spaTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}




void ClientDashboard::handleBookSpaClick()
{
    int checkedCount = 0;
    int checkedRow = -1;


    for (int i = 0; i < ui->spaTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem* item = ui->spaTableWidget->item(i, 0);
        if (item->checkState() == Qt::Checked) {
            checkedCount++;
            checkedRow = i;
        }
    }


    if (checkedCount == 0) {
        QMessageBox::warning(this, "Not selected yet", "Please select a service to schedule.");
        return;
    }
    if (checkedCount > 1) {
        QMessageBox::warning(this, "Select too many", "Please select only 1 service per booking.");
        return;
    }


    std::string serviceId = ui->spaTableWidget->item(checkedRow, 1)->text().toStdString();
    std::string serviceName = ui->spaTableWidget->item(checkedRow, 2)->text().toStdString();


    std::string date = ui->bookingDateEdit->date().toString("dd/MM/yyyy").toStdString();
    std::string time = ui->bookingTimeEdit->time().toString("hh:mm").toStdString();


    try {
        if (!m_bookingRepo->isTimeSlotAvailable(date, time)) {
            QMessageBox::warning(this, "Duplicate booking", QString("Hour %1 day %2 is full.").arg(QString::fromStdString(time), QString::fromStdString(date)));
            return;
        }

        std::string bookingId = m_bookingRepo->generateBookingId();
        Booking newBooking(bookingId, m_currentClient->getId(), "CUSTOMER_PET", serviceId, date, time, "Pending");

        m_bookingRepo->createBooking(newBooking);


        ui->spaTableWidget->item(checkedRow, 0)->setCheckState(Qt::Unchecked);

        QMessageBox::information(this, "Success", QString("Scheduled: %1").arg(QString::fromStdString(serviceName)));

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}


void ClientDashboard::handleBuy()
{
    qDebug() << "[handleBuy] Da duoc goi...";


    if (m_currentClient == nullptr) {
        qDebug() << "LOI NGHIEM TRONG: m_currentClient la NULL!"; return;
    }
    if (m_petRepo == nullptr) {
        qDebug() << "LOI NGHIEM TRONG: m_petRepo la NULL!"; return;
    }
    if (m_cartRepo == nullptr) {
        qDebug() << "LOI NGHIEM TRONG: m_cartRepo la NULL!"; return;
    }
    if (m_billRepo == nullptr) {
        qDebug() << "LOI NGHIEM TRONG: m_billRepo la NULL! (KIEM TRA mainwindow.cpp)"; return;
    }
    qDebug() << "[handleBuy] Tat ca Repo deu OK.";



    std::string clientId = m_currentClient->getId();
    LinkedList<CartItem> itemsToBuy;
    LinkedList<std::string> unavailablePets;

    qDebug() << "[handleBuy] Bat dau kiem tra ton kho...";
    for (int i = 0; i < ui->cartTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem* selectBox = ui->cartTableWidget->item(i, 0);
        if (selectBox != nullptr && selectBox->checkState() == Qt::Checked)
        {
            std::string petId = ui->cartTableWidget->item(i, 1)->text().toStdString();
            if (m_petRepo->isAvailablePet(petId)) {
                std::string petName = ui->cartTableWidget->item(i, 2)->text().toStdString();
                long petPrice = ui->cartTableWidget->item(i, 4)->text().toLong();
                itemsToBuy.pushBack(CartItem(petId, petName, petPrice));
            } else {
                unavailablePets.pushBack(petId);
            }
        }
    }


    if (!unavailablePets.isEmpty()) {
        std::string petId = unavailablePets.getHead()->getData();
        QMessageBox::warning(this, "Payment Error",
                             QString("Item '%1' is no longer available. Please remove it from your cart.")
                                 .arg(QString::fromStdString(petId)));
        loadCartDataToTable();
        return;
    }
    if (itemsToBuy.isEmpty()) {
        QMessageBox::warning(this, "Not selected yet", "You have not selected any items to purchase.");
        return;
    }


    qDebug() << "[handleBuy] Bat dau thanh toan...";
    try {
        LinkedList<BillItem> billItems;
        Node<CartItem>* itemNode = itemsToBuy.getHead();
        while(itemNode != nullptr) {
            CartItem cartItem = itemNode->getData();
            billItems.pushBack(BillItem(cartItem.getPetId(), cartItem.getPetName(), "Pet", cartItem.getPrice()));
            itemNode = itemNode->getNext();
        }

        qDebug() << "[handleBuy] Dang tao Bill...";
        Bill bill = m_billRepo->createBillFromCart(
            m_currentClient->getId(),
            m_currentClient->getName(),
            billItems);

        qDebug() << "[handleBuy] Dang luu Bill...";
        m_billRepo->saveBill(bill);

        qDebug() << "[handleBuy] Dang cap nhat trang thai Pet...";
        itemNode = itemsToBuy.getHead();
        while(itemNode != nullptr) {
            std::string petId = itemNode->getData().getPetId();
            m_petRepo->setStatusUnavailable(petId);
            m_cartRepo->removeFromCart(clientId, petId);
            itemNode = itemNode->getNext();
        }

        qDebug() << "[handleBuy] Dang tao Bill Dialog...";

        QString billTitle = QString("INVOICE: %1").arg(QString::fromStdString(bill.getBillId()));
        QString billContent;

        billContent += "========================================\n";
        billContent += "              SALES INVOICE\n";
        billContent += "               DUT PETSHOP\n";
        billContent += "========================================\n";
        billContent += QString("Invoice ID: %1\n").arg(QString::fromStdString(bill.getBillId()));
        billContent += QString("Customer: %1\n").arg(QString::fromStdString(bill.getClientName()));
        billContent += QString("ID: %1\n").arg(QString::fromStdString(bill.getClientId()));
        billContent += QString("Date: %1   Time: %2\n").arg(QString::fromStdString(bill.getDate()), QString::fromStdString(bill.getTime()));
        billContent += "----------------------------------------\n";
        billContent += "Order Details:\n";
        billContent += "----------------------------------------\n";
        Node<BillItem>* billItemNode = bill.getItems().getHead();
        int stt = 1;
        while (billItemNode != nullptr) {
            billContent += QString("%1. %2 (%3)\n")
            .arg(stt++)
                .arg(QString::fromStdString(billItemNode->getData().getItemName()))
                .arg(QString::fromStdString(billItemNode->getData().getItemType()));
            billContent += QString("   ID: %1\n").arg(QString::fromStdString(billItemNode->getData().getItemId()));
            billContent += QString("   Gia: %1 VND\n\n").arg(billItemNode->getData().getPrice());
            billItemNode = billItemNode->getNext();
        }
        billContent += "----------------------------------------\n";
        billContent += QString("TOTAL: %1 VND\n").arg(bill.getTotalAmount());
        billContent += "========================================\n";
        billContent += "             THANK YOU!\n";
        billContent += "========================================\n";


        BillDialog* billDialog = new BillDialog(this);
        billDialog->setAttribute(Qt::WA_DeleteOnClose);
        billDialog->setBillContent(billTitle, billContent);

        qDebug() << "[handleBuy] Dang hien thi Bill Dialog...";
        billDialog->exec();

        qDebug() << "[handleBuy] Da dong Bill. Tai lai Cart...";
        loadCartDataToTable();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Backend error", e.what());
    }
}



void ClientDashboard::loadCartDataToTable()
{
    ui->cartTableWidget->clearContents();
    ui->cartTableWidget->setRowCount(0);
    ui->cartTableWidget->setColumnCount(5);

    QStringList headers = {"Choose", "ID", "Name", "Type", "Price (VND)"};
    ui->cartTableWidget->setHorizontalHeaderLabels(headers);

    LinkedList<CartItem> items = m_cartRepo->getCartItems(m_currentClient->getId());
    Node<CartItem>* current = items.getHead();
    int row = 0;

    while(current != nullptr) {
        CartItem item = current->getData();
        ui->cartTableWidget->insertRow(row);

        QTableWidgetItem* selectBox = new QTableWidgetItem();
        selectBox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        selectBox->setCheckState(Qt::Unchecked);
        ui->cartTableWidget->setItem(row, 0, selectBox);

        ui->cartTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(item.getPetId())));
        ui->cartTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(item.getPetName())));
        ui->cartTableWidget->setItem(row, 3, new QTableWidgetItem("Pet"));
        ui->cartTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(item.getPrice())));

        current = current->getNext();
        row++;
    }

    ui->cartTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->cartTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void ClientDashboard::handleRemoveFromCart()
{
    int removedCount = 0;

    for (int i = ui->cartTableWidget->rowCount() - 1; i >= 0; --i)
    {
        QTableWidgetItem* selectBox = ui->cartTableWidget->item(i, 0);


        if (selectBox != nullptr && selectBox->checkState() == Qt::Checked)
        {
            std::string petId = ui->cartTableWidget->item(i, 1)->text().toStdString();

            m_cartRepo->removeFromCart(m_currentClient->getId(), petId);

            ui->cartTableWidget->removeRow(i);
            removedCount++;
        }
    }

    if (removedCount > 0) {
        QMessageBox::information(this, "Success", QString("%1 items removed from cart.").arg(removedCount));
    } else {
        QMessageBox::warning(this, "Not selected yet", "You have not selected any items to delete.");
    }
}

void ClientDashboard::loadBookingsDataToTable()
{

    if (ui->bookingsTableWidget == nullptr || m_bookingRepo == nullptr) return;

    ui->bookingsTableWidget->setRowCount(0);
    ui->bookingsTableWidget->setColumnCount(6);
    QStringList headers = {"Choose", "ID", "Service", "Date", "Time", "Status"};
    ui->bookingsTableWidget->setHorizontalHeaderLabels(headers);


    std::string myId = m_currentClient->getId();
    LinkedList<Booking> myBookings = m_bookingRepo->getBookingsByClient(myId);

    Node<Booking>* current = myBookings.getHead();
    int row = 0;

    while(current != nullptr)
    {
        Booking booking = current->getData();
        std::string status = booking.getStatus();
        if (status == "Pending" || status == "Confirmed")
        {
            ui->bookingsTableWidget->insertRow(row);

            QTableWidgetItem *selectBox = new QTableWidgetItem();
            selectBox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            selectBox->setCheckState(Qt::Unchecked);
            ui->bookingsTableWidget->setItem(row, 0, selectBox);

            ui->bookingsTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(booking.getBookingId())));

            Service service = m_serviceRepo->getServiceInfo(booking.getServiceId());
            ui->bookingsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(service.getName())));

            ui->bookingsTableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(booking.getDate())));
            ui->bookingsTableWidget->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(booking.getTime())));
            ui->bookingsTableWidget->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(status)));


            if (status == "Pending")
                ui->bookingsTableWidget->item(row, 5)->setForeground(QBrush(QColor("orange")));
            else if (status == "Confirmed")
                ui->bookingsTableWidget->item(row, 5)->setForeground(QBrush(QColor("green")));


            row++;
        }


        current = current->getNext();
    }
    ui->bookingsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ClientDashboard::handleCancelBookingClick()
{
    int canceledCount = 0;
    bool hasError = false;


    for (int i = 0; i < ui->bookingsTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem* selectBox = ui->bookingsTableWidget->item(i, 0);
        if (selectBox != nullptr && selectBox->checkState() == Qt::Checked)
        {
            std::string bookingId = ui->bookingsTableWidget->item(i, 1)->text().toStdString();
            std::string status = ui->bookingsTableWidget->item(i, 5)->text().toStdString();


            if (status != "Pending") {
                QMessageBox::warning(this, "Cannot be canceled",
                                     QString("The booking %1 is in status '%2'. You can only cancel pending bookings.")
                                         .arg(QString::fromStdString(bookingId), QString::fromStdString(status)));

                hasError = true;


                selectBox->setCheckState(Qt::Unchecked);
                continue;
            }


            m_bookingRepo->updateStatus(bookingId, "Cancelled");
            canceledCount++;

            ui->bookingsTableWidget->item(i, 5)->setText("Cancelled");
            ui->bookingsTableWidget->item(i, 5)->setForeground(QBrush(QColor("red")));


            selectBox->setCheckState(Qt::Unchecked);
        }
    }

    if (canceledCount > 0) {
        QMessageBox::information(this, "Success", QString("%1 bookings canceled successfully.").arg(canceledCount));
    }
    else {

        if (!hasError) {
            QMessageBox::warning(this, "Notification", "Please select at least one 'Pending' schedule to cancel.");
        }
    }
}




void ClientDashboard::on_purchaseHistoryButton_clicked() { loadPurchaseHistory(); }
void ClientDashboard::on_bookingHistoryButton_clicked() { loadBookingHistory(); }

void ClientDashboard::loadPurchaseHistory()
{

    ui->historyTableWidget->setRowCount(0);


    ui->historyTableWidget->setColumnCount(4);
    QStringList headers = {"Bill ID", "Date", "Time", "Total"};
    ui->historyTableWidget->setHorizontalHeaderLabels(headers);


    LinkedList<Bill> bills = m_billRepo->getBillsByClientId(m_currentClient->getId());
    Node<Bill>* current = bills.getHead();
    int row = 0;


    while (current != nullptr)
    {
        const Bill& bill = current->getData();

        ui->historyTableWidget->insertRow(row);

        ui->historyTableWidget->setItem(row, 0,
                                        new QTableWidgetItem(QString::fromStdString(bill.getBillId())));

        ui->historyTableWidget->setItem(row, 1,
                                        new QTableWidgetItem(QString::fromStdString(bill.getDate())));

        ui->historyTableWidget->setItem(row, 2,
                                        new QTableWidgetItem(QString::fromStdString(bill.getTime())));

        ui->historyTableWidget->setItem(row, 3,
                                        new QTableWidgetItem(QString::number(bill.getTotalAmount())));

        current = current->getNext();
        row++;
    }

    ui->historyTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}



void ClientDashboard::loadBookingHistory()
{
    ui->historyTableWidget->setRowCount(0);
    ui->historyTableWidget->setColumnCount(5);
    QStringList headers = {"ID", "Service", "Date", "Time", "Status"};
    ui->historyTableWidget->setHorizontalHeaderLabels(headers);

    LinkedList<Booking> bookings = m_bookingRepo->getBookingsByClient(m_currentClient->getId());
    Node<Booking>* current = bookings.getHead();
    int row = 0;

    while(current != nullptr) {
        Booking booking = current->getData();
        std::string status = booking.getStatus();

        if (status == "Completed" || status == "Cancelled")
        {
            ui->historyTableWidget->insertRow(row);

            ui->historyTableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(booking.getBookingId())));

            Service service = m_serviceRepo->getServiceInfo(booking.getServiceId());
            ui->historyTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(service.getName())));

            ui->historyTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(booking.getDate())));

            ui->historyTableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(booking.getTime())));

            QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(status));
            ui->historyTableWidget->setItem(row, 4, statusItem);

            if (status == "Completed")
                statusItem->setForeground(QBrush(QColor("green")));
            else if (status == "Cancelled")
                statusItem->setForeground(QBrush(QColor("red")));

            row++;
        }

        current = current->getNext();
    }
    ui->historyTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void ClientDashboard::loadProfileData()
{
    Client updatedClient = m_accountRepo->getClientInfo(m_currentClient->getId());

    ui->valId->setText("Phone number (ID): " + QString::fromStdString(updatedClient.getId()));
    ui->valName->setText("Name: " + QString::fromStdString(updatedClient.getName()));
    ui->valGender->setText("Gender: " + QString::fromStdString(updatedClient.getGender()));
    ui->valAddress->setText("Address: " + QString::fromStdString(updatedClient.getStreet()));
    ui->valCity->setText("City: " + QString::fromStdString(updatedClient.getCity()));
}

void ClientDashboard::on_editProfileButton_clicked()
{
    Client c = m_accountRepo->getClientInfo(m_currentClient->getId());

    ui->editIdInput->setText(QString::fromStdString(c.getId()));
    ui->editPassInput->setText(QString::fromStdString(c.getPassword()));
    ui->editNameInput->setText(QString::fromStdString(c.getName()));
    ui->editStreetInput->setText(QString::fromStdString(c.getStreet()));
    ui->editCityInput->setText(QString::fromStdString(c.getCity()));

    if (c.getGender() == "Male") ui->editMaleRadio->setChecked(true);
    else ui->editFemaleRadio->setChecked(true);

    ui->pageStackedWidget->setCurrentWidget(ui->profileEditPage);
}

void ClientDashboard::on_saveProfileButton_clicked()
{
    // Lấy dữ liệu từ form
    std::string id = ui->editIdInput->text().toStdString();
    std::string pass = ui->editPassInput->text().toStdString();
    std::string name = ui->editNameInput->text().toStdString();
    std::string street = ui->editStreetInput->text().toStdString();
    std::string city = ui->editCityInput->text().toStdString();
    std::string gender = ui->editMaleRadio->isChecked() ? "Male" : "Female";

    if (pass.empty() || name.empty() || street.empty() || city.empty()) {
        QMessageBox::warning(this, "Error", "Please do not leave information blank.");
        return;
    }

    Client updatedClient(id, name, pass, gender, street, city);

    try {
        m_accountRepo->setClientInfo(updatedClient);

        ui->welcomeTitleLabel->setText("Welcome back, " + QString::fromStdString(name));
        ui->userNameLabel->setText(QString::fromStdString(name));

        QMessageBox::information(this, "Success", "Profile updated successfully!");

        ui->pageStackedWidget->setCurrentWidget(ui->profileViewPage);
        loadProfileData();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void ClientDashboard::on_cancelProfileButton_clicked()
{
    ui->pageStackedWidget->setCurrentWidget(ui->profileViewPage);
}
void ClientDashboard::setupHelpPage()
{
    QWidget* page = ui->helpPage;

    QVBoxLayout* mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(40, 20, 40, 30);
    mainLayout->setSpacing(20);

    QLabel* title = new QLabel("call center");
    title->setStyleSheet("color: #243db8; font-size: 30px; font-weight: bold;");
    mainLayout->addWidget(title);

    mainLayout->addSpacing(10);

    QWidget* centerWidget = new QWidget();
    QVBoxLayout* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setAlignment(Qt::AlignCenter);

    QLabel* bigTitle = new QLabel("we are here to help!");
    bigTitle->setStyleSheet("color: #243db8; font-size: 36px; font-weight: bold;");
    bigTitle->setAlignment(Qt::AlignCenter);

    QLabel* description = new QLabel(
        "If you have any questions about our services, your bookings, or need advice "
        "for your pets, the DUT Petshop support team is ready to assist you. Feel free "
        "to leave a message or contact us directly through the Call Center. We're committed "
        "to making your experience smooth and enjoyable—for both you and your furry friends."
        );
    description->setWordWrap(true);
    description->setAlignment(Qt::AlignCenter);
    description->setStyleSheet("font-size: 14px; line-height: 20px;");
    description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    description->setMaximumWidth(1100);

    centerLayout->addWidget(bigTitle);
    centerLayout->addWidget(description);

    mainLayout->addWidget(centerWidget);

    mainLayout->addSpacing(20);

    QHBoxLayout* contactRow = new QHBoxLayout();

    QWidget* contactBlock = new QWidget();
    QVBoxLayout* contactLayout = new QVBoxLayout(contactBlock);

    QLabel* contactTitle = new QLabel("Contact:");
    contactTitle->setStyleSheet("color: #243db8; font-size: 18px; font-weight: bold;");

    QLabel* hotline = new QLabel("Our Hotline: 0000.000.000");
    hotline->setStyleSheet("font-style: italic; font-size: 14px;");

    QLabel* email1 = new QLabel("Our Email: 102240154@sv1.dut.udn.vn");
    email1->setStyleSheet("font-style: italic; font-size: 14px;");

    QLabel* email2 = new QLabel("102240170@sv1.dut.udn.vn");
    email2->setStyleSheet("font-style: italic; font-size: 14px;");

    QLabel* social = new QLabel("Social: facebook.com");
    social->setStyleSheet("font-style: italic; font-size: 14px;");

    contactLayout->addWidget(contactTitle);
    contactLayout->addWidget(hotline);
    contactLayout->addWidget(email1);
    contactLayout->addWidget(email2);
    contactLayout->addWidget(social);

    contactRow->addWidget(contactBlock);
    contactRow->addStretch();

    mainLayout->addLayout(contactRow);

    page->setStyleSheet("QLabel { color: black; }");
}

