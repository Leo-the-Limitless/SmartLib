#include "EditBookDialog.h"
#include "ui_EditBookDialog.h"

#include <QMessageBox>

EditBookDialog::EditBookDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::EditBookDialog) {
    ui->setupUi(this);
    connect(ui->idInput, &QLineEdit::textChanged, this, &EditBookDialog::onIdInputChanged);
    connect(ui->SaveButton, &QPushButton::clicked, this, &EditBookDialog::onSaveButtonClicked);
}

EditBookDialog::~EditBookDialog() {
    delete ui;
}

void EditBookDialog::onIdInputChanged(const QString &text) {
    // If the ID is empty, don't do anything
    if (text.isEmpty()) {
        return;
    }

    currentBookId = text.toInt();

    QSqlQuery query;
    query.prepare("SELECT title, author, genre, publication_year, stock FROM books WHERE book_id = :book_id");
    query.bindValue(":book_id", currentBookId);

    if (query.exec() && query.next()) {
        ui->titleInput->setText(query.value(0).toString());
        ui->authorInput->setText(query.value(1).toString());
        ui->genreInput->setText(query.value(2).toString());
        ui->yearInput->setText(query.value(3).toString());
        ui->stockInput->setText(query.value(4).toString());
    } else {
        QMessageBox::warning(this, "Error", "Book ID not found!");
    }
}

void EditBookDialog::onSaveButtonClicked() {
    QString title = ui->titleInput->text();
    QString author = ui->authorInput->text();
    QString genre = ui->genreInput->text();
    QString year = ui->yearInput->text();
    int stock = ui->stockInput->text().toInt();  // Get the stock value

    if (title.isEmpty() || author.isEmpty() || genre.isEmpty() || year.isEmpty() || ui->stockInput->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE books SET title = :title, author = :author, genre = :genre, "
                  "publication_year = :year, stock = :stock WHERE book_id = :book_id");
    query.bindValue(":title", title);
    query.bindValue(":author", author);
    query.bindValue(":genre", genre);
    query.bindValue(":year", year);
    query.bindValue(":stock", stock);
    query.bindValue(":book_id", currentBookId);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Book updated successfully!");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update book.");
    }
}
