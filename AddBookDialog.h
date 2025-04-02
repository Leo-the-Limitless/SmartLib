#ifndef ADDBOOKDIALOG_H
#define ADDBOOKDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class AddBookDialog;
}

class AddBookDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddBookDialog(QWidget *parent = nullptr);
    ~AddBookDialog();

private slots:
    void onAddButtonClicked();

private:
    Ui::AddBookDialog *ui;
    DatabaseManager dbManager;
    bool isIdUnique(int bookId);
};

#endif // ADDBOOKDIALOG_H
