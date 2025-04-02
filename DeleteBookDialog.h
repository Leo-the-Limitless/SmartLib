#ifndef DELETEBOOKDIALOG_H
#define DELETEBOOKDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class DeleteBookDialog;
}

class DeleteBookDialog : public QDialog{
    Q_OBJECT

public:
    explicit DeleteBookDialog(QWidget *parent = nullptr);
    ~DeleteBookDialog();

private slots:
    void onDeleteBookClicked();
    void onIdInputChanged(const QString &);

private:
    Ui::DeleteBookDialog *ui;
    DatabaseManager dbManager;
    int currentBookId;
};

#endif // DELETEBOOKDIALOG_H
