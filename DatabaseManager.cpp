#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("library.db");
}

bool DatabaseManager::openDatabase() {
    if (!db.open()) {
        qDebug() << "Database connection failed: " << db.lastError().text();
        return false;
    }
    initializeDatabase(); // Ensure tables exist
    return true;
}

void DatabaseManager::closeDatabase() {
    db.close();
}

void DatabaseManager::initializeDatabase() {
    QSqlQuery query;

    // Create Users Table
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "email TEXT UNIQUE NOT NULL, "
               "password TEXT NOT NULL, "
               "is_admin BOOLEAN DEFAULT 0)");

    // Create Books Table
    query.exec("CREATE TABLE IF NOT EXISTS books ("
               "book_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "title TEXT NOT NULL, "
               "author TEXT NOT NULL, "
               "genre TEXT, "
               "stock INTEGER NOT NULL)");

    // Create Transactions Table
    query.exec("CREATE TABLE IF NOT EXISTS transactions ("
               "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "user_id INTEGER, "
               "book_id INTEGER, "
               "borrow_date TEXT, "
               "return_date TEXT, "
               "status TEXT CHECK(status IN ('borrowed', 'returned')), "
               "FOREIGN KEY (user_id) REFERENCES users(user_id), "
               "FOREIGN KEY (book_id) REFERENCES books(book_id))");

    // Check for initial admin account, create one if missing
    query.exec("SELECT COUNT(*) FROM users WHERE is_admin = 1");
    if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO users (name, email, password, is_admin) VALUES "
                   "('Admin', 'admin@library.com', 'admin123', 1)");
        qDebug() << "Initial admin account created (email: admin@library.com, password: admin123)";
    }
}

bool DatabaseManager::borrowBook(int userId, int bookId) {
    QSqlQuery query;

    // Check if the book is available
    query.prepare("SELECT stock FROM books WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (!query.exec() || !query.next()) {
        qDebug() << "Error fetching book stock: " << query.lastError().text();
        return false;
    }

    int stock = query.value(0).toInt();
    if (stock <= 0) {
        qDebug() << "Book is out of stock!";
        return false;
    }

    // Insert a borrowing transaction
    query.prepare("INSERT INTO transactions (user_id, book_id, borrow_date, status) "
                  "VALUES (:user_id, :book_id, DATE('now'), 'borrowed')");
    query.bindValue(":user_id", userId);
    query.bindValue(":book_id", bookId);
    if (!query.exec()) {
        qDebug() << "Error inserting transaction: " << query.lastError().text();
        return false;
    }

    // Decrease book stock
    query.prepare("UPDATE books SET stock = stock - 1 WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (!query.exec()) {
        qDebug() << "Error updating book stock: " << query.lastError().text();
        return false;
    }

    qDebug() << "Book borrowed successfully!";
    return true;
}

bool DatabaseManager::returnBook(int userId, int bookId) {
    QSqlQuery query;

    // Check if the user has borrowed the book and not yet returned it
    query.prepare("SELECT transaction_id FROM transactions "
                  "WHERE user_id = :user_id AND book_id = :book_id AND status = 'borrowed' LIMIT 1");
    query.bindValue(":user_id", userId);
    query.bindValue(":book_id", bookId);

    if (!query.exec() || !query.next()) {
        qDebug() << "No active borrow record found!";
        return false;
    }

    int transactionId = query.value(0).toInt();

    // Update the transaction status to 'returned'
    query.prepare("UPDATE transactions SET status = 'returned', return_date = DATE('now') "
                  "WHERE transaction_id = :transaction_id");
    query.bindValue(":transaction_id", transactionId);
    if (!query.exec()) {
        qDebug() << "Error updating transaction: " << query.lastError().text();
        return false;
    }

    // Increase book stock
    query.prepare("UPDATE books SET stock = stock + 1 WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (!query.exec()) {
        qDebug() << "Error updating book stock: " << query.lastError().text();
        return false;
    }

    qDebug() << "Book returned successfully!";
    return true;
}

