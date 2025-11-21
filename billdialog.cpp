// billdialog.cpp
#include "billdialog.h"
#include "ui_billdialog.h"

BillDialog::BillDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BillDialog)
{
    ui->setupUi(this);
    connect(ui->closeButton, &QPushButton::clicked, this, &BillDialog::accept);
}

BillDialog::~BillDialog()
{
    delete ui;
}


void BillDialog::setBillContent(const QString& title, const QString& content)
{
    ui->billTitleLabel->setText(title);
    ui->billTextEdit->setPlainText(content);
}

