
#ifndef BILLDIALOG_H
#define BILLDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class BillDialog;
}

class BillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BillDialog(QWidget *parent = nullptr);
    ~BillDialog();

    void setBillContent(const QString& title, const QString& content);

private:
    Ui::BillDialog *ui;
};
#endif
