#ifndef EDITBOOKDIALOG_H
#define EDITBOOKDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class EditBookDialog;
}

class EditBookDialog : public QDialog {
    Q_OBJECT

public:
    explicit EditBookDialog(QWidget *parent = nullptr);
    ~EditBookDialog();

private slots:
    void onSaveButtonClicked();
    void onIdInputChanged(const QString &);

private:
    Ui::EditBookDialog *ui;
    DatabaseManager dbManager;
    int currentBookId;
};

#endif // EDITBOOKDIALOG_H
