#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void userLoggedIn(int userId, bool isAdmin);

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();

private:
    Ui::LoginWindow *ui;
    DatabaseManager dbManager;
};

#endif // LOGINWINDOW_H
