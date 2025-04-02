#include "AddBookDialog.h"
#include "ui_AddBookDialog.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

AddBookDialog::AddBookDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddBookDialog) {
    ui->setupUi(this);
    connect(ui->ConfirmButton, &QPushButton::clicked, this, &::AddBookDialog::onAddButtonClicked);
}

AddBookDialog::~AddBookDialog() {
    delete ui;
}

bool AddBookDialog::isIdUnique(int bookId) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM books WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 0;
    }
    return false;
}

void AddBookDialog::onAddButtonClicked() {
    int id = ui->idInput->text().toInt();
    QString title = ui->titleInput->text().trimmed();
    QString author = ui->authorInput->text().trimmed();
    QString genre = ui->genreInput->text().trimmed();
    int year = ui->yearInput->text().toInt();
    int stock = ui->stockInput->text().toInt();

    if (title.isEmpty() || author.isEmpty() || genre.isEmpty() ||
        ui->idInput->text().isEmpty() || ui->yearInput->text().isEmpty() || ui->stockInput->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }

    if (stock <= 0) {
        QMessageBox::warning(this, "Error", "Stock must be greater than 0.");
        return;
    }

    if (!isIdUnique(id)) {
        QMessageBox::warning(this, "Error", "Book ID already exists!");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO books (book_id, title, author, genre, publication_year, status, stock) "
                  "VALUES (:id, :title, :author, :genre, :year, :status, :stock)");

    query.bindValue(":id", id);
    query.bindValue(":title", title);
    query.bindValue(":author", author);
    query.bindValue(":genre", genre);
    query.bindValue(":year", year);
    query.bindValue(":status", "Available");  // Explicitly binding status
    query.bindValue(":stock", stock);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Book added successfully!");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add book: " + query.lastError().text());
    }
}

