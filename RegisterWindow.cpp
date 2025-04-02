#include "RegisterWindow.h"
#include "ui_RegisterWindow.h"
#include <QMessageBox>
#include "DatabaseManager.h"
#include "LoginWindow.h"

RegisterWindow::RegisterWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterWindow) {
    ui->setupUi(this);
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWindow::onRegisterButtonClicked);
    connect(ui->cancleButton, &QPushButton::clicked, this, &RegisterWindow::onCalcleButtonClicked);
}

RegisterWindow::~RegisterWindow() {
    delete ui;
}

void RegisterWindow::onRegisterButtonClicked() {
    QString name = ui->nameInput->text();
    QString email = ui->emailInput->text();
    QString password = ui->passwordInput->text();
    QString confirmPassword = ui->passwordConInput->text();  // Get confirmation password

    if (name.isEmpty() || email.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Error", "Passwords do not match. Please try again.");
        return;
    }

    // Check if email already exists
    DatabaseManager dbManager;
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email = :email");
    query.bindValue(":email", email);
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Error", "Email already exists. Please use a different email.");
        return;
    }

    // Insert new user into the database
    query.prepare("INSERT INTO users (name, email, password, is_admin) VALUES (:name, :email, :password, 0)");
    query.bindValue(":name", name);
    query.bindValue(":email", email);
    query.bindValue(":password", password);

    if (query.exec()) {
        QMessageBox::information(this, "Registration Successful", "Your account has been created successfully.");
        this->hide();

        LoginWindow *L = new LoginWindow();
        L->show();

        connect(L, &LoginWindow::destroyed, this, &RegisterWindow::close);
    } else {
        QMessageBox::warning(this, "Error", "Registration failed. Please try again.");
    }
}

void RegisterWindow::onCalcleButtonClicked(){
    this->close();
    LoginWindow *l = new LoginWindow();
    l->exec();
}
