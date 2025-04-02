#include "mainwindow.h"
#include "LoginWindow.h"
#include "RegisterWindow.h"
#include "ui_LoginWindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginWindow), dbManager() {  // Initialize dbManager
    ui->setupUi(this);

    if (!dbManager.openDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database.");
        return;
    }

    connect(ui->LoginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->RegisterButton, &QPushButton::clicked, this, &LoginWindow::onRegisterButtonClicked);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::onLoginButtonClicked() {
    QString email = ui->emailInput->text();
    QString password = ui->passwordInput->text();
    int userId;
    QString username;
    bool isAdmin;

    // Use the dbManager to check credentials and get user details
    if (dbManager.checkCredentials(email, password, userId, username, isAdmin)) {
        // If login is successful, show a success message
        QMessageBox::information(this, "Login Successful", "Welcome, " + username);

        // Emit the signal to notify that the user has logged in with their details
        emit userLoggedIn(userId, isAdmin);

        // Now, pass user details to the MainWindow and show it
        MainWindow *mainWindow = new MainWindow(userId, username, email, isAdmin);  // Pass username and email to MainWindow
        mainWindow->show();

        // Optionally, hide the login window instead of closing it
        this->hide();

        // Optionally, close the login window completely when the main window is closed
        connect(mainWindow, &MainWindow::destroyed, this, &LoginWindow::close);
    } else {
        // If login fails, show a warning message
        QMessageBox::warning(this, "Login Failed", "Invalid email or password.");
    }
}

void LoginWindow::onRegisterButtonClicked() {
    this->hide();
    RegisterWindow *registerWindow = new RegisterWindow(this);
    registerWindow->exec();
}
