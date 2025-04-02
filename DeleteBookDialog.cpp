#include "DeleteBookDialog.h"
#include "ui_DeleteBookDialog.h"
#include <QMessageBox>

DeleteBookDialog::DeleteBookDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeleteBookDialog)
{
    ui->setupUi(this);
    connect(ui->idInput, &QLineEdit::textChanged, this, &DeleteBookDialog::onIdInputChanged);
    connect(ui->ConfirmButton, &QPushButton::clicked, this, &DeleteBookDialog::onDeleteBookClicked);  // Connect delete button

}

DeleteBookDialog::~DeleteBookDialog()
{
    delete ui;
}

void DeleteBookDialog::onIdInputChanged(const QString &text) {
    // Check if the input text is valid and non-empty
    if (text.isEmpty() || !text.toInt()) {
        ui->ResultLabel->setText("ID not found or Exist!");  // Handle invalid input or empty input
        return;
    }

    currentBookId = text.toInt();

    // Prepare query to find the book by ID
    QSqlQuery query;
    query.prepare("SELECT title FROM books WHERE book_id = :book_id");
    query.bindValue(":book_id", currentBookId);

    if (query.exec() && query.next()) {
        // If book found, display title
        QString title = query.value(0).toString();
        ui->ResultLabel->setText("ID Book Found!\nName: " + title);
    } else {
        // If no book found, display error message
        ui->ResultLabel->setText("ID not found or Exist!");
    }
}


void DeleteBookDialog::onDeleteBookClicked() {
    // Ensure that the book ID input is not empty
    if (ui->idInput->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a Book ID to delete.");
        return;
    }

    // Prepare query to check if the book exists
    QSqlQuery query;
    query.prepare("SELECT title FROM books WHERE book_id = :book_id");
    query.bindValue(":book_id", currentBookId);

    if (query.exec() && query.next()) {
        QString title = query.value(0).toString();

        // Ask for user confirmation before deleting
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Book",
                                      "Are you sure you want to delete the book: " + title + "?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Proceed with the deletion
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM books WHERE book_id = :book_id");
            deleteQuery.bindValue(":book_id", currentBookId);

            if (deleteQuery.exec()) {
                QMessageBox::information(this, "Success", "Book deleted successfully!");
                accept();  // Close dialog after successful deletion
            } else {
                QMessageBox::warning(this, "Error", "Failed to delete book: " + deleteQuery.lastError().text());
            }
        }
    } else {
        QMessageBox::warning(this, "Error", "Book ID not found!");
    }
}
