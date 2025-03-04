#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    dbManager.openDatabase();  // Ensure database is open
    loadBooks();  // Load book data into the table
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadBooks() {
    QSqlQuery query("SELECT book_id, title, author, genre, stock FROM books");

    ui->booksTableWidget->setRowCount(0);
    ui->booksTableWidget->setColumnCount(5);
    ui->booksTableWidget->setHorizontalHeaderLabels({"ID", "Title", "Author", "Genre", "Stock"});

    int row = 0;
    while (query.next()) {
        ui->booksTableWidget->insertRow(row);
        for (int col = 0; col < 5; col++) {
            ui->booksTableWidget->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        row++;
    }
}

void MainWindow::on_borrowButton_clicked() {
    int row = ui->booksTableWidget->currentRow();
    if (row == -1) {
        qDebug() << "No book selected!";
        return;
    }

    int bookId = ui->booksTableWidget->item(row, 0)->text().toInt();
    int userId = ui->userIdInput->text().toInt();

    if (dbManager.borrowBook(userId, bookId)) {
        qDebug() << "Book borrowed successfully!";
        loadBooks();  // Refresh book list
    } else {
        qDebug() << "Failed to borrow book.";
    }
}

void MainWindow::on_returnButton_clicked() {
    int bookId = ui->booksTableWidget->currentRow();
    if (bookId == -1) {
        qDebug() << "No book selected!";
        return;
    }

    int userId = ui->userIdInput->text().toInt();

    if (dbManager.returnBook(userId, bookId)) {
        qDebug() << "Book returned successfully!";
        loadBooks();  // Refresh book list
    } else {
        qDebug() << "Failed to return book.";
    }
}
