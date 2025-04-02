// MainWindow.cpp
#include "mainwindow.h"
#include "AddBookDialog.h"
#include "EditBookDialog.h"
#include "ui_mainwindow.h"
#include "DeleteBookDialog.h"
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(int userId, const QString &username, const QString &email, bool isAdmin, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    currentUserId(userId), currentUsername(username), currentEmail(email), currentIsAdmin(isAdmin) {

    ui->setupUi(this);

    dbManager.openDatabase();
    loadBooks();

    // Hide menuBooks if the user is not an admin
    if (!currentIsAdmin) {
        ui->menuBooks->menuAction()->setVisible(false);
    }

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExitClicked);
    connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::onActionRefreshClicked);
    connect(ui->actionAddBook, &QAction::triggered, this, &MainWindow::onActionAddBookClicked);
    connect(ui->actionEditBook, &QAction::triggered, this, &MainWindow::onActionEditBookClicked);
    connect(ui->actionDeleteBook, &QAction::triggered, this, &MainWindow::onActionDeleteBookClicked);
    connect(ui->borrowButton, &QPushButton::clicked, this, &MainWindow::onBorrowButtonClicked);
    connect(ui->booksTableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onBookSelectionChanged);
    connect(ui->comboBoxSearchType, &QComboBox::currentTextChanged, this, &MainWindow::searchBooks);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::searchBooks);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::onReturnButtonClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onActionRefreshClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadBooks() {
    QSqlQuery query("SELECT book_id, title, author, genre, publication_year, stock, status FROM books");

    ui->booksTableWidget->setRowCount(0);
    ui->booksTableWidget->setColumnCount(7);
    ui->booksTableWidget->setHorizontalHeaderLabels({"ID", "Title", "Author", "Genre","Publication Year","Stock", "Status"});
    ui->booksTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // Stretch columns

    int row = 0;
    while (query.next()) {
        ui->booksTableWidget->insertRow(row);
        for (int col = 0; col < 7; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);  // Make the item non-editable
            ui->booksTableWidget->setItem(row, col, item);
        }
        row++;
    }
}

void MainWindow::searchBooks() {
    QString searchText = ui->lineEditSearch->text();
    QString searchColumn = ui->comboBoxSearchType->currentText();

    if (searchText.isEmpty()) {
        loadBooks();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT book_id, title, author, genre, publication_year, stock, status FROM books WHERE " + searchColumn + " LIKE ?");
    query.addBindValue("%" + searchText + "%");
    query.exec();

    ui->booksTableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        ui->booksTableWidget->insertRow(row);
        for (int col = 0; col < 7; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->booksTableWidget->setItem(row, col, item);
        }
        row++;
    }
    if (row == 0) {
        ui->ResultLabel->setText("Status: Not found!");
    } else {
        ui->ResultLabel->clear();
    }
}

void MainWindow::onBorrowButtonClicked() {
    int row = ui->booksTableWidget->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "Warning", "No book selected!");
        return;
    }

    int bookId = ui->booksTableWidget->item(row, 0)->text().toInt();
    QString bookTitle = ui->booksTableWidget->item(row, 1)->text();
    int currentStock = ui->booksTableWidget->item(row, 5)->text().toInt();

    if (currentUserId == 0) {
        QMessageBox::warning(this, "Warning", "User not logged in!");
        return;
    }

    if (currentStock <= 0) {
        QMessageBox::warning(this, "Warning", "This book is out of stock!");
        return;
    }

    // Confirmation dialog
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm Borrow",
                                    "Do you want to borrow \"" + bookTitle + "\"?",
                                    QMessageBox::Yes|QMessageBox::No);

    if (confirm == QMessageBox::Yes) {
        QString errorMessage;
        if (dbManager.borrowBook(currentUserId, bookId, errorMessage)) {
            QMessageBox::information(this, "Success", "Book borrowed successfully!");
            loadBooks();
        } else {
            QMessageBox::critical(this, "Error", errorMessage);  // Show specific error
        }
    }
    dbManager.printBookDataADMIN();
    dbManager.printRecordTableDataADMIN();
}

void MainWindow::onBookSelectionChanged() {
    int row = ui->booksTableWidget->currentRow();
    if (row != -1) {
        QString selectedBook = ui->booksTableWidget->item(row, 1)->text();
        ui->SelectedLabel->setText(selectedBook);
    } else {
        ui->SelectedLabel->setText("None");
    }
}
void MainWindow::onReturnButtonClicked() {
    // Get list of books borrowed by this user
    QSqlQuery query;
    query.prepare("SELECT b.book_id, b.title FROM books b "
                  "JOIN transactionrecord t ON b.book_id = t.book_id "
                  "JOIN users u ON t.email = u.email "
                  "WHERE u.user_id = :user_id AND t.amount = 1");
    query.bindValue(":user_id", currentUserId);
    query.exec();

    QMap<int, QString> borrowedBooks;
    while (query.next()) {
        int bookId = query.value(0).toInt();
        QString title = query.value(1).toString();
        borrowedBooks[bookId] = title;
    }

    if (borrowedBooks.isEmpty()) {
        QMessageBox::information(this, "Return Book", "You have no books to return.");
        return;
    }

    // Create a list of book titles for the dialog
    QStringList items;
    QList<int> bookIds;

    for (auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
        items << QString("ID: %1 - %2").arg(it.key()).arg(it.value());
        bookIds << it.key();
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Return Book",
                                             "Select a book to return:",
                                             items, 0, false, &ok);

    if (!ok || selected.isEmpty()) {
        return; // User canceled
    }

    // Extract book ID from selection
    int selectedIndex = items.indexOf(selected);
    if (selectedIndex >= 0) {
        int bookId = bookIds.at(selectedIndex);

        if (dbManager.returnBook(currentUserId, bookId)) {
            QMessageBox::information(this, "Success", "Book returned successfully!");
            loadBooks();  // Refresh book list
        } else {
            QMessageBox::critical(this, "Error", "Failed to return book.");
        }
    }
    dbManager.printBookDataADMIN();
    dbManager.printRecordTableDataADMIN();
}

void MainWindow::onActionExitClicked(){
    QApplication::quit();
}

void MainWindow::onActionRefreshClicked(){
    loadBooks();
    qDebug()<<"Book load!";
}

void MainWindow::onActionAddBookClicked() {
    AddBookDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        loadBooks();  // Refresh book table after adding
    }
}

void MainWindow::onActionEditBookClicked() {
    EditBookDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        loadBooks();  // Refresh book table after editing
    }
}

void MainWindow::onActionDeleteBookClicked(){
    DeleteBookDialog dialog(this);
    if (dialog.exec()==QDialog::Accepted){
        loadBooks();
    }
}
