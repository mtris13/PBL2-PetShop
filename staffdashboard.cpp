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
    m_currentPosFilter("Dog"), // Mặc định xem Chó
    m_currentPosKeyword("")
{
    ui->setupUi(this);

    // 1. Hiển thị tên Staff
    ui->userNameLabel->setText(QString::fromStdString(m_currentStaff->getName()));
    ui->welcomeTitleLabel->setText("Xin chào, " + QString::fromStdString(m_currentStaff->getName()));
    ui->posDogButton->setCheckable(true);
    ui->posCatButton->setCheckable(true);
    ui->posDogButton->setChecked(true);

    ui->spaDateEdit->setDisplayFormat("d/M/yyyy");
    ui->spaDateEdit->setDate(QDate::currentDate()); // Ngày hiện tại
    ui->spaDateEdit->setCalendarPopup(true);

    ui->spaTimeEdit->setDisplayFormat("HH:mm");
    ui->spaTimeEdit->setTime(QTime::currentTime());
    m_posTypeGroup = new QButtonGroup(this);
    m_posTypeGroup->addButton(ui->posDogButton);
    m_posTypeGroup->addButton(ui->posCatButton);
    m_posTypeGroup->setExclusive(true);
    // 2. Nhóm nút NavBar
    m_navGroup = new QButtonGroup(this);
    m_navGroup->addButton(ui->posPetsButton);
    m_navGroup->addButton(ui->posSpaButton);
    m_navGroup->addButton(ui->manageBookingsButton);
    m_navGroup->addButton(ui->historyButton);
    m_navGroup->addButton(ui->createClientButton);
    m_navGroup->addButton(ui->profileButton);
    m_navGroup->setExclusive(true);

    // 3. Kết nối thủ công
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

    // 3. Kết nối bảng (tính tiền khi tick)
    connect(ui->posSpaTable, &QTableWidget::itemChanged, this, &StaffDashboard::on_posSpaTable_itemChanged);

    connect(ui->historySearchButton, &QPushButton::clicked, this, &StaffDashboard::on_historySearchButton_clicked);

    // Cấu hình bảng Lịch sử (để đẹp hơn)
    ui->bookingHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bookingHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->salesHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->salesHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 4. Cấu hình bảng
    ui->posSpaTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->posSpaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Mặc định vào trang 1
    ui->pendingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pendingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pendingTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Bảng Confirmed
    ui->confirmedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->confirmedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->confirmedTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // 3. Cấu hình bảng (giống Client)
    ui->posPetsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->posPetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->posPetsTable->setSelectionMode(QAbstractItemView::MultiSelection); // Cho phép chọn nhiều
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

// --- CÁC HÀM CHUYỂN TRANG ---
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
    loadHistoryTables(); // <-- Gọi hàm tải dữ liệu
}

void StaffDashboard::handleCreateClientClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->createClientPage);
}

void StaffDashboard::handleProfileClick() {
    ui->pageStackedWidget->setCurrentWidget(ui->profilePage);
    ui->profileStackedWidget->setCurrentWidget(ui->profileViewPage); // Mặc định vào trang Xem
    loadProfileData(); // Tải dữ liệu mới nhất
}

void StaffDashboard::handleLogoutClick() {
    emit logoutSignal();
    this->close();
}
void StaffDashboard::on_saveClientButton_clicked()
{
    // Lấy dữ liệu
    std::string id = ui->clientIdInput->text().toStdString();
    std::string name = ui->clientNameInput->text().toStdString();
    std::string pass = ui->clientPassInput->text().toStdString();
    std::string address = ui->clientAddressInput->text().toStdString();
    std::string city = ui->clientCityInput->text().toStdString();

    std::string gender = "Unknown";
    if (ui->maleRadio->isChecked()) gender = "Male";
    else if (ui->femaleRadio->isChecked()) gender = "Female";

    // Validate đơn giản
    if (id.empty() || name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập ít nhất ID, Tên và Mật khẩu.");
        return;
    }
    if (id.length() != 10) {
        QMessageBox::warning(this, "Lỗi ID", "Số điện thoại phải có 10 chữ số.");
        return;
    }

    // Kiểm tra trùng ID
    if (m_accountRepo->isValidId(id)) {
        QMessageBox::warning(this, "Trùng lặp", "Khách hàng này đã có tài khoản trong hệ thống.");
        return;
    }

    // Tạo và Lưu
    try {
        Client newClient(id, name, pass, gender, address, city);
        m_accountRepo->registerClient(newClient);

        QMessageBox::information(this, "Thành công", "Đã tạo tài khoản khách hàng thành công!");

        // Reset form để nhập người tiếp theo
        ui->clientIdInput->clear();
        ui->clientNameInput->clear();
        ui->clientPassInput->setText("123"); // Reset về mặc định cho nhanh
        ui->clientAddressInput->clear();
        ui->clientCityInput->clear();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Lỗi", e.what());
    }
}
void StaffDashboard::loadPosPetsTable()
{
    ui->posPetsTable->blockSignals(true); // Chặn tín hiệu để tránh tính tiền sai
    ui->posPetsTable->setRowCount(0);
    ui->posPetsTable->setColumnCount(6);
    QStringList headers = {"Chọn", "ID", "Tên", "Giống", "Giá", "Trạng thái"};
    ui->posPetsTable->setHorizontalHeaderLabels(headers);

    int row = 0;
    std::string keyword = m_currentPosKeyword.toStdString();

    // --- LOGIC LỌC DỮ LIỆU ---
    if (m_currentPosFilter == "Dog")
    {
        // Lấy danh sách Chó (Có tìm kiếm hoặc không)
        LinkedList<Dog> dogs;
        if (m_currentPosKeyword.isEmpty()) {
            dogs = m_petRepo->getAllDogInfoAvailable();
        } else {
            dogs = m_petRepo->searchDog("all", keyword); // Tìm trong ID, Tên, Giống
        }

        Node<Dog>* dNode = dogs.getHead();
        while(dNode != nullptr) {
            Dog p = dNode->getData();
            // Chỉ hiện Available (nếu đang tìm kiếm thì searchDog đã trả về cả sold, nên cần check lại status)
            if (p.getStatus() == "available") {
                ui->posPetsTable->insertRow(row);

                QTableWidgetItem* check = new QTableWidgetItem();
                QString idStr = QString::fromStdString(p.getId());

                // Nếu ID này đã có trong bộ nhớ (đã chọn trước đó), thì tick vào
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
    else // Filter == "Cat"
    {
        // Lấy danh sách Mèo
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
    ui->posPetsTable->blockSignals(false); // Mở lại tín hiệu
}
void StaffDashboard::on_posCheckButton_clicked()
{
    std::string phone = ui->posPhoneInput->text().toStdString();

    if (phone.empty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập Số điện thoại.");
        return;
    }

    // Tìm trong Repo (Hàm getClientInfo trả về Client rỗng nếu không thấy)
    Client c = m_accountRepo->getClientInfo(phone);

    if (c.getId().empty()) {
        // Không tìm thấy
        ui->posClientNameLabel->setText("Khách: KHÔNG TÌM THẤY");
        ui->posClientNameLabel->setStyleSheet("color: red; font-weight: bold;");

        // Gợi ý tạo mới
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Khách mới", "Khách hàng chưa có tài khoản. Tạo mới ngay?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ui->pageStackedWidget->setCurrentWidget(ui->createClientPage); // Chuyển sang trang tạo TK
            // (Optional) Điền sẵn SĐT vào form tạo mới
            ui->clientIdInput->setText(QString::fromStdString(phone));
        }

        if (m_currentPosClient) delete m_currentPosClient; // Xóa client cũ nếu có
        m_currentPosClient = nullptr;

    } else {
        // Tìm thấy
        ui->posClientNameLabel->setText("Khách: " + QString::fromStdString(c.getName()));
        ui->posClientNameLabel->setStyleSheet("color: green; font-weight: bold;");

        if (m_currentPosClient) delete m_currentPosClient;
        m_currentPosClient = new Client(c); // Lưu lại khách hàng này
    }
}
void StaffDashboard::on_posPetsTable_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        // Lấy ID từ cột 1
        QString id = ui->posPetsTable->item(item->row(), 1)->text();

        if (item->checkState() == Qt::Checked) {
            m_selectedPetIds.insert(id); // Thêm vào bộ nhớ
        } else {
            m_selectedPetIds.remove(id); // Xóa khỏi bộ nhớ
        }

        calculatePosTotal(); // Tính lại tiền
    }
}

void StaffDashboard::calculatePosTotal()
{
    long total = 0;
    ui->posCartList->clear();

    // Duyệt qua tất cả ID đang được chọn trong bộ nhớ
    for (const QString& idStr : m_selectedPetIds) {
        std::string id = idStr.toStdString();
        std::string name;
        long price = 0;

        // Hỏi Repo thông tin chi tiết (dựa vào ID)
        if (id[0] == 'd') {
            Dog d = m_petRepo->getDogInfo(id);
            name = d.getName();
            price = d.getPrice();
        } else {
            Cat c = m_petRepo->getCatInfo(id);
            name = c.getName();
            price = c.getPrice();
        }

        // Hiển thị lên list bên phải
        ui->posCartList->addItem(QString::fromStdString(name) + " - " + QString::number(price) + " VND");
        total += price;
    }

    ui->posTotalLabel->setText("TỔNG TIỀN: " + QString::number(total) + " VND");
}
void StaffDashboard::on_posPayButton_clicked()
{
    // 1. Kiểm tra Khách hàng
    if (m_currentPosClient == nullptr) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập SĐT và bấm 'Kiểm tra' để chọn khách hàng.");
        return;
    }

    // 2. Kiểm tra Giỏ hàng
    if (ui->posCartList->count() == 0) {
        QMessageBox::warning(this, "Giỏ hàng trống", "Vui lòng chọn ít nhất 1 thú cưng.");
        return;
    }

    // 3. Tạo Bill Items
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
        m_petRepo->setStatusUnavailable(id); // Cập nhật đã bán
    }

    // 4. Tạo và Lưu Hóa đơn
    try {
        Bill bill = m_billRepo->createBillFromCart(
            m_currentPosClient->getId(),
            m_currentPosClient->getName(),
            billItems
            );
        m_billRepo->saveBill(bill);

        // 5. Hiện Hóa đơn & Reset
        // (Copy logic hiện BillDialog từ ClientDashboard sang đây nếu muốn, hoặc thông báo đơn giản)
        QMessageBox::information(this, "Thành công", "Thanh toán thành công! Bill ID: " + QString::fromStdString(bill.getBillId()));

        // Reset giao diện
        m_selectedPetIds.clear();
        loadPosPetsTable(); // Load lại để ẩn pet đã bán
        ui->posCartList->clear();
        ui->posTotalLabel->setText("TỔNG TIỀN: 0 VND");
        // Giữ nguyên khách hàng để họ mua tiếp nếu muốn, hoặc reset tùy bạn

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Lỗi", e.what());
    }
}
void StaffDashboard::on_posDogButton_clicked()
{
    m_currentPosFilter = "Dog";
    // Reset tìm kiếm khi chuyển loại (tùy chọn)
    // ui->posSearchInput->clear();
    // m_currentPosKeyword = "";
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

    // Cột: Chọn | ID | Tên Dịch Vụ | Giá | Thời gian
    QStringList headers = {"Chọn", "ID", "Tên Dịch vụ", "Giá (VND)", "Thời gian"};
    ui->posSpaTable->setHorizontalHeaderLabels(headers);

    // Lấy tất cả dịch vụ từ Repo
    LinkedList<Service> services = m_serviceRepo->getAllServices();

    int row = 0;
    Node<Service>* current = services.getHead();

    while(current != nullptr) {
        Service s = current->getData();
        ui->posSpaTable->insertRow(row);

        // Cột Checkbox
        QTableWidgetItem* check = new QTableWidgetItem();
        check->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (m_selectedServiceIds.contains(QString::fromStdString(s.getId())))
            check->setCheckState(Qt::Checked);
        else
            check->setCheckState(Qt::Unchecked);
        ui->posSpaTable->setItem(row, 0, check);

        // Các cột dữ liệu
        ui->posSpaTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s.getId())));
        ui->posSpaTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));
        ui->posSpaTable->setItem(row, 3, new QTableWidgetItem(QString::number(s.getPrice())));
        ui->posSpaTable->setItem(row, 4, new QTableWidgetItem(QString::number(s.getDuration()) + " phút"));

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
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập Số điện thoại.");
        return;
    }

    Client c = m_accountRepo->getClientInfo(phone);

    if (c.getId().empty()) {
        ui->spaClientNameLabel->setText("Khách: KHÔNG TÌM THẤY");
        ui->spaClientNameLabel->setStyleSheet("color: red; font-weight: bold;");

        if (m_currentSpaClient) { delete m_currentSpaClient; m_currentSpaClient = nullptr; }

        QMessageBox::StandardButton reply = QMessageBox::question(this, "Khách mới",
                                                                  "Khách hàng chưa có tài khoản. Tạo mới ngay?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ui->pageStackedWidget->setCurrentWidget(ui->createClientPage);
            ui->clientIdInput->setText(QString::fromStdString(phone));
        }
    } else {
        ui->spaClientNameLabel->setText("Khách: " + QString::fromStdString(c.getName()));
        ui->spaClientNameLabel->setStyleSheet("color: green; font-weight: bold;");

        if (m_currentSpaClient) delete m_currentSpaClient;
        m_currentSpaClient = new Client(c);
    }
}
// staffdashboard.cpp

void StaffDashboard::on_posSpaTable_itemChanged(QTableWidgetItem *item)
{
    // Chỉ xử lý khi thay đổi ở cột 0 (Checkbox)
    if (item->column() != 0) return;

    QString id = ui->posSpaTable->item(item->row(), 1)->text();

    // Nếu người dùng vừa TICK vào ô này
    if (item->checkState() == Qt::Checked) {

        // 1. CHẶN TÍN HIỆU (Cực kỳ quan trọng để tránh vòng lặp vô tận)
        ui->posSpaTable->blockSignals(true);

        // 2. Bỏ tick tất cả các ô khác
        for (int i = 0; i < ui->posSpaTable->rowCount(); i++) {
            QTableWidgetItem* otherItem = ui->posSpaTable->item(i, 0);
            if (otherItem != item) { // Không phải ô đang chọn
                otherItem->setCheckState(Qt::Unchecked);
            }
        }

        // 3. Cập nhật dữ liệu: Xóa hết cũ, chỉ giữ lại cái mới
        m_selectedServiceIds.clear();
        m_selectedServiceIds.insert(id);

        // 4. MỞ LẠI TÍN HIỆU
        ui->posSpaTable->blockSignals(false);
    }
    else {
        // Nếu người dùng BỎ TICK (Untick)
        m_selectedServiceIds.remove(id);
    }

    // Tính lại tiền và hiển thị
    calculateSpaTotal();
}

// staffdashboard.cpp

void StaffDashboard::calculateSpaTotal()
{
    long total = 0;

    // Quan trọng: Phải là spaCartList (của trang Spa)
    ui->spaCartList->clear();

    for (const QString& idStr : m_selectedServiceIds) {
        std::string id = idStr.toStdString();
        // Lấy thông tin dịch vụ
        Service s = m_serviceRepo->getServiceInfo(id);

        // Hiển thị tên và giá
        ui->spaCartList->addItem(QString::fromStdString(s.getName()) + "\n" + QString::number(s.getPrice()) + " VND");

        total += s.getPrice();
    }

    ui->spaTotalLabel->setText("TỔNG: " + QString::number(total) + " VND");
}
void StaffDashboard::on_spaBookButton_clicked()
{
    // 1. Kiểm tra
    if (m_currentSpaClient == nullptr) {
        QMessageBox::warning(this, "Chưa chọn khách", "Vui lòng chọn khách hàng trước.");
        return;
    }
    if (m_selectedServiceIds.isEmpty()) {
        QMessageBox::warning(this, "Chưa chọn dịch vụ", "Vui lòng chọn ít nhất 1 dịch vụ.");
        return;
    }

    std::string date = ui->spaDateEdit->date().toString("dd/MM/yyyy").toStdString();
    std::string time = ui->spaTimeEdit->time().toString("hh:mm").toStdString();

    // Kiểm tra trùng lịch (đơn giản)
    if (!m_bookingRepo->isTimeSlotAvailable(date, time)) {
        QMessageBox::warning(this, "Trùng lịch", "Giờ này đã có người đặt. Vui lòng chọn giờ khác.");
        return;
    }

    try {
        // 2. Tạo Booking cho từng dịch vụ được chọn
        int count = 0;
        for (const QString& idStr : m_selectedServiceIds) {
            std::string serviceId = idStr.toStdString();
            std::string bookingId = m_bookingRepo->generateBookingId();

            // Tạo booking trạng thái "Confirmed" luôn vì Staff đặt trực tiếp
            Booking newBooking(bookingId, m_currentSpaClient->getId(), "N/A", serviceId, date, time, "Confirmed");
            m_bookingRepo->createBooking(newBooking);
            count++;
        }

        // 3. Thông báo
        QMessageBox::information(this, "Thành công",
                                 QString("Đã đặt thành công %1 dịch vụ cho khách %2 vào lúc %3 ngày %4.")
                                     .arg(count)
                                     .arg(QString::fromStdString(m_currentSpaClient->getName()))
                                     .arg(QString::fromStdString(time))
                                     .arg(QString::fromStdString(date)));

        // 4. Reset
        m_selectedServiceIds.clear();
        loadSpaTable();
        ui->spaTotalLabel->setText("TỔNG: 0 VND");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Lỗi", e.what());
    }
}
// --- HÀM TẢI DỮ LIỆU CHO CẢ 2 BẢNG ---
void StaffDashboard::loadBookingTables()
{
    // 1. Tải bảng PENDING
    ui->pendingTable->setRowCount(0);
    ui->pendingTable->setColumnCount(5);
    QStringList headers = {"Mã Đặt", "Khách hàng", "Dịch vụ", "Ngày", "Giờ"};
    ui->pendingTable->setHorizontalHeaderLabels(headers);

    LinkedList<Booking> pendingList = m_bookingRepo->getBookingsByStatus("Pending");
    Node<Booking>* pNode = pendingList.getHead();
    int row = 0;

    while(pNode != nullptr) {
        Booking b = pNode->getData();
        ui->pendingTable->insertRow(row);

        ui->pendingTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBookingId())));

        // Lấy tên khách từ ID (để hiển thị cho đẹp)
        Client c = m_accountRepo->getClientInfo(b.getClientId());
        ui->pendingTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName())));

        // Lấy tên dịch vụ
        Service s = m_serviceRepo->getServiceInfo(b.getServiceId());
        ui->pendingTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.getName())));

        ui->pendingTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getDate())));
        ui->pendingTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(b.getTime())));

        pNode = pNode->getNext();
        row++;
    }
    ui->pendingTable->resizeColumnsToContents();


    // 2. Tải bảng CONFIRMED
    ui->confirmedTable->setRowCount(0);
    ui->confirmedTable->setColumnCount(5);
    ui->confirmedTable->setHorizontalHeaderLabels(headers); // Dùng chung header

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

// --- HÀM XỬ LÝ NÚT: HỦY (PENDING -> CANCELLED) ---
void StaffDashboard::on_cancelBookingButton_clicked()
{
    int currentRow = ui->pendingTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn một lịch đặt để hủy.");
        return;
    }

    std::string bookingId = ui->pendingTable->item(currentRow, 0)->text().toStdString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận", "Bạn có chắc muốn hủy lịch đặt này không?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_bookingRepo->updateStatus(bookingId, "Cancelled");
        loadBookingTables(); // Tải lại để cập nhật
        QMessageBox::information(this, "Thành công", "Đã hủy lịch đặt.");
    }
}

// --- HÀM XỬ LÝ NÚT: XÁC NHẬN (PENDING -> CONFIRMED) ---
void StaffDashboard::on_confirmBookingButton_clicked()
{
    int currentRow = ui->pendingTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn một lịch đặt để xác nhận.");
        return;
    }

    std::string bookingId = ui->pendingTable->item(currentRow, 0)->text().toStdString();

    // Cập nhật trạng thái
    m_bookingRepo->updateStatus(bookingId, "Confirmed");

    loadBookingTables(); // Tải lại
    QMessageBox::information(this, "Thành công", "Lịch đặt đã được xác nhận và chuyển sang tab 'Đã xác nhận'.");
}

// --- HÀM XỬ LÝ NÚT: HOÀN THÀNH (CONFIRMED -> COMPLETED) ---
 // Để hiện hóa đơn sau khi hoàn thành (nếu muốn)

void StaffDashboard::on_completeBookingButton_clicked()
{
    int currentRow = ui->confirmedTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Chưa chọn", "Vui lòng chọn một lịch đặt để hoàn thành.");
        return;
    }

    std::string bookingId = ui->confirmedTable->item(currentRow, 0)->text().toStdString();

    // 1. Cập nhật trạng thái
    m_bookingRepo->updateStatus(bookingId, "Completed");

    // 2. (Tùy chọn) Tạo hóa đơn tự động cho dịch vụ này
    // Bạn có thể viết logic tạo Bill ở đây giống như nút "Thanh toán" bên Spa
    // Ví dụ: Lấy thông tin booking -> tạo Bill -> Save Bill

    loadBookingTables();
    QMessageBox::information(this, "Thành công", "Dịch vụ đã hoàn thành! Trạng thái đã được cập nhật.");
}
// --- HÀM TẢI DỮ LIỆU LỊCH SỬ (GỌI KHI CHUYỂN TRANG HOẶC TÌM KIẾM) ---



// --- TAB 1: LỊCH SỬ ĐẶT CHỖ (Completed/Cancelled) ---


// --- TAB 2: LỊCH SỬ HÓA ĐƠN (Bill) ---

// --- HÀM TẢI DỮ LIỆU LỊCH SỬ CHUNG ---
void StaffDashboard::loadHistoryTables() {
    std::string filter = ui->historySearchInput->text().toStdString();

    // Tải cả 2 bảng
    loadBookingHistoryTable(filter);
    loadSalesHistoryTable(filter);
}

// Nút Tìm kiếm
void StaffDashboard::on_historySearchButton_clicked() {
    loadHistoryTables();
}

// --- TAB 1: LỊCH SỬ ĐẶT CHỖ (Booking) ---
void StaffDashboard::loadBookingHistoryTable(const std::string& phoneFilter) {
    ui->bookingHistoryTable->setRowCount(0);
    ui->bookingHistoryTable->setColumnCount(6);
    QStringList headers = {"Mã Đặt", "Khách hàng", "Dịch vụ", "Ngày", "Giờ", "Trạng thái"};
    ui->bookingHistoryTable->setHorizontalHeaderLabels(headers);

    // Lấy toàn bộ booking
    LinkedList<Booking> allBookings = m_bookingRepo->getAllBookings();
    Node<Booking>* node = allBookings.getHead();
    int row = 0;

    while (node != nullptr) {
        Booking b = node->getData();

        // Chỉ hiện Completed hoặc Cancelled
        if (b.getStatus() == "Completed" || b.getStatus() == "Cancelled") {

            // Nếu có bộ lọc SĐT, kiểm tra xem có khớp không
            if (!phoneFilter.empty() && b.getClientId().find(phoneFilter) == std::string::npos) {
                node = node->getNext();
                continue; // Bỏ qua nếu không khớp SĐT
            }

            ui->bookingHistoryTable->insertRow(row);
            ui->bookingHistoryTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBookingId())));

            // Lấy tên khách
            Client c = m_accountRepo->getClientInfo(b.getClientId());
            ui->bookingHistoryTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName()) + "\n(" + QString::fromStdString(b.getClientId()) + ")"));

            // Lấy tên dịch vụ
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

// --- TAB 2: LỊCH SỬ HÓA ĐƠN (Bill) ---
void StaffDashboard::loadSalesHistoryTable(const std::string& phoneFilter) {
    ui->salesHistoryTable->setRowCount(0);
    ui->salesHistoryTable->setColumnCount(5);
    QStringList headers = {"Mã Bill", "Khách hàng", "Ngày", "Giờ", "Tổng tiền"};
    ui->salesHistoryTable->setHorizontalHeaderLabels(headers);

    // Lấy toàn bộ Bill (nhờ hàm getAllBills bạn vừa thêm)
    LinkedList<Bill> allBills = m_billRepo->getAllBills();
    Node<Bill>* node = allBills.getHead();
    int row = 0;

    while (node != nullptr) {
        Bill b = node->getData();

        // Nếu có bộ lọc SĐT
        if (!phoneFilter.empty() && b.getClientId().find(phoneFilter) == std::string::npos) {
            node = node->getNext();
            continue;
        }

        ui->salesHistoryTable->insertRow(row);
        ui->salesHistoryTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getBillId())));

        // Tên khách + SĐT
        ui->salesHistoryTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.getClientName()) + "\n(" + QString::fromStdString(b.getClientId()) + ")"));

        ui->salesHistoryTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.getDate())));
        ui->salesHistoryTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.getTime())));

        // Tổng tiền (Màu xanh đậm)
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(b.getTotalAmount()) + " VND");
        priceItem->setForeground(QBrush(QColor("darkblue")));
        priceItem->setFont(QFont("Arial", 9, QFont::Bold));
        ui->salesHistoryTable->setItem(row, 4, priceItem);

        row++;
        node = node->getNext();
    }
    ui->salesHistoryTable->resizeColumnsToContents();
}
// --- HÀM TẢI DỮ LIỆU STAFF ---
void StaffDashboard::loadProfileData()
{
    // Lấy thông tin mới nhất từ Repo (để đảm bảo cập nhật)
    // (Lưu ý: AccountRepo cần có hàm getStaffInfo(id). File bạn gửi ĐÃ CÓ).
    Staff staff = m_accountRepo->getStaffInfo(m_currentStaff->getId());

    // Hiển thị (Dùng HTML để in đậm tiêu đề)
    ui->valStaffId->setText("<b>Mã NV:</b> " + QString::fromStdString(staff.getId()));
    ui->valStaffName->setText("<b>Họ tên:</b> " + QString::fromStdString(staff.getName()));
    ui->valStaffGender->setText("<b>Giới tính:</b> " + QString::fromStdString(staff.getGender()));
    ui->valStaffRole->setText("<b>Chức vụ:</b> Staff"); // Hard-code hoặc lấy từ DB nếu có

    // (Không hiển thị Lương ở đây)

    ui->valStaffId->setTextFormat(Qt::RichText);
    ui->valStaffName->setTextFormat(Qt::RichText);
    ui->valStaffGender->setTextFormat(Qt::RichText);
    ui->valStaffRole->setTextFormat(Qt::RichText);
}

// --- CHUYỂN SANG CHẾ ĐỘ SỬA ---
void StaffDashboard::on_editProfileButton_clicked()
{
    Staff s = m_accountRepo->getStaffInfo(m_currentStaff->getId());

    ui->editStaffIdInput->setText(QString::fromStdString(s.getId())); // ReadOnly
    ui->editStaffNameInput->setText(QString::fromStdString(s.getName()));
    ui->editStaffPassInput->setText(QString::fromStdString(s.getPassword()));

    if (s.getGender() == "Male") ui->editStaffMale->setChecked(true);
    else ui->editStaffFemale->setChecked(true);

    ui->profileStackedWidget->setCurrentWidget(ui->profileEditPage);
}

// --- LƯU THÔNG TIN ---
void StaffDashboard::on_saveProfileButton_clicked()
{
    std::string id = ui->editStaffIdInput->text().toStdString();
    std::string name = ui->editStaffNameInput->text().toStdString();
    std::string pass = ui->editStaffPassInput->text().toStdString();
    std::string gender = ui->editStaffMale->isChecked() ? "Male" : "Female";

    if (name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Lỗi", "Tên và Mật khẩu không được để trống.");
        return;
    }

    // Lấy lương cũ (để không bị mất khi ghi đè)
    Staff oldStaff = m_accountRepo->getStaffInfo(id);
    long salary = oldStaff.getSalary();

    // Tạo đối tượng mới (giữ nguyên lương)
    Staff updatedStaff(id, name, pass, gender, salary);

    try {
        m_accountRepo->setStaffInfo(updatedStaff);

        // Cập nhật lại giao diện chính (Tên trên góc phải)
        ui->userNameLabel->setText(QString::fromStdString(name));
        ui->welcomeTitleLabel->setText("Xin chào, " + QString::fromStdString(name));

        QMessageBox::information(this, "Thành công", "Cập nhật hồ sơ thành công!");

        // Quay về trang xem
        ui->profileStackedWidget->setCurrentWidget(ui->profileViewPage);
        loadProfileData();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Lỗi", e.what());
    }
}

void StaffDashboard::on_cancelProfileButton_clicked()
{
    ui->profileStackedWidget->setCurrentWidget(ui->profileViewPage);
}
