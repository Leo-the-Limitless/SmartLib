#include "DatabaseManager.h"
#include <QFile>

DatabaseManager::DatabaseManager() {
    if (!QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("library.db");  // Ensure this is a valid path
    } else {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
}

bool DatabaseManager::openDatabase() {
    qDebug() << "Checking database file: " << db.databaseName();

    QFile dbFile(db.databaseName());
    bool databaseExists = dbFile.exists();

    if (!db.open()) {
        qDebug() << "Database connection failed: " << db.lastError().text();
        return false;
    }

    qDebug() << "Database opened successfully.";

    // If the database file was missing, create tables
    if (!databaseExists) {
        qDebug() << "No existing database found. Creating a new one...";
        initializeDatabase();
    }

    return true;
}

void DatabaseManager::initializeDatabase() {
    QSqlQuery query;
    qDebug() << "Initializing database tables...";

    // Create Users Table
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "user_id INTEGER PRIMARY KEY, "
                    "name TEXT NOT NULL, "
                    "email TEXT UNIQUE NOT NULL, "
                    "password TEXT NOT NULL, "
                    "is_admin BOOLEAN DEFAULT 0)")) {
        qDebug() << "Error creating 'users' table: " << query.lastError().text();
    }

    // Create Books Table
    if (!query.exec("CREATE TABLE IF NOT EXISTS books ("
                    "book_id INTEGER PRIMARY KEY, "
                    "title TEXT NOT NULL, "
                    "author TEXT NOT NULL, "
                    "genre TEXT, "
                    "publication_year INTEGER, "
                    "status TEXT CHECK(status IN ('Available', 'Borrowed')) DEFAULT 'Available', "
                    "stock INTEGER NOT NULL DEFAULT 1)")) {
        qDebug() << "Error creating 'books' table: " << query.lastError().text();
    }

    // Create Transaction Record Table
    if (!query.exec("CREATE TABLE IF NOT EXISTS transactionrecord ("
                    "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "email TEXT NOT NULL, "
                    "book_id INTEGER NOT NULL, "
                    "amount INTEGER NOT NULL DEFAULT 0, "
                    "FOREIGN KEY (email) REFERENCES users(email), "
                    "FOREIGN KEY (book_id) REFERENCES books(book_id))")) {
        qDebug() << "Error creating 'transactionrecord' table: " << query.lastError().text();
    }

    // Check for initial admin account
    if (!query.exec("SELECT COUNT(*) FROM users WHERE is_admin = 1")) {
        qDebug() << "Error checking admin account: " << query.lastError().text();
    } else if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO users (name, email, password, is_admin) VALUES "
                   "('Admin', 'admin@library.com', 'admin123', 1)");
        qDebug() << "Initial admin account created (email: admin@library.com, password: admin123)";
    }

    qDebug() << "Database initialization complete!";
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::checkCredentials(const QString &email, const QString &password, int &userId, QString &name, bool &isAdmin) {
    if (!openDatabase()) return false;

    QSqlQuery query;
    query.prepare("SELECT user_id, name, is_admin FROM users WHERE email = :email AND password = :password");
    query.bindValue(":email", email);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        userId = query.value(0).toInt();
        name = query.value(1).toString();
        isAdmin = query.value(2).toBool();
        return true;
    }
    return false;
}

bool DatabaseManager::borrowBook(int userId, int bookId, QString &errorMessage) {
    if (!openDatabase()) {
        errorMessage = "Database connection failed.";
        return false;
    }

    QSqlQuery query;
    QSqlDatabase::database().transaction(); // Start transaction

    // Get user email from userId
    query.prepare("SELECT email FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    if (!query.exec() || !query.next()) {
        errorMessage = "Error fetching user email: " + query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }
    QString userEmail = query.value(0).toString();

    // Check total books borrowed by user
    query.prepare("SELECT COUNT(*) FROM transactionrecord WHERE email = :email");
    query.bindValue(":email", userEmail);
    if (!query.exec() || !query.next()) {
        errorMessage = "Error fetching user's borrowed books count: " + query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    int totalBooksBorrowed = query.value(0).toInt();
    if (totalBooksBorrowed >= 3) {
        errorMessage = "You have already borrowed the maximum of 3 books.";
        QSqlDatabase::database().rollback();
        return false;
    }

    // Check copies of the same book borrowed by user
    query.prepare("SELECT COUNT(*) FROM transactionrecord WHERE email = :email AND book_id = :book_id");
    query.bindValue(":email", userEmail);
    query.bindValue(":book_id", bookId);
    if (!query.exec() || !query.next()) {
        errorMessage = "Error fetching borrowed copies count: " + query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    int copiesOfSameBook = query.value(0).toInt();
    if (copiesOfSameBook >= 2) {
        errorMessage = "You have already borrowed 2 copies of this book.";
        QSqlDatabase::database().rollback();
        return false;
    }

    // Check book availability and stock
    query.prepare("SELECT stock FROM books WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (!query.exec() || !query.next()) {
        errorMessage = "Error fetching book stock: " + query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    int stock = query.value(0).toInt();
    if (stock <= 0) {
        errorMessage = "This book is out of stock.";
        QSqlDatabase::database().rollback();
        return false;
    }

    // Insert a new transaction record
    query.prepare("INSERT INTO transactionrecord (email, book_id, amount) VALUES (:email, :book_id, 1)");
    query.bindValue(":email", userEmail);
    query.bindValue(":book_id", bookId);
    if (!query.exec()) {
        errorMessage = "Error inserting transaction record: " + query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    // Update book stock
    query.prepare("UPDATE books SET stock = stock - 1 WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (!query.exec()) {
        errorMessage = "Error updating book stock: " + query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    // Update status only if stock is now zero
    if (stock - 1 == 0) {
        query.prepare("UPDATE books SET status = 'Borrowed' WHERE book_id = :book_id");
        query.bindValue(":book_id", bookId);
        if (!query.exec()) {
            errorMessage = "Error updating book status: " + query.lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }
    }

    // Commit transaction if all queries succeed
    if (!QSqlDatabase::database().commit()) {
        errorMessage = "Transaction commit failed!";
        return false;
    }

    return true;
}

bool DatabaseManager::returnBook(int userId, int bookId) {
    if (!openDatabase()) return false;

    QSqlQuery query;

    // Get user email from userId
    query.prepare("SELECT email FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    if (!query.exec() || !query.next()) {
        qDebug() << "Error fetching user email: " << query.lastError().text();
        return false;
    }
    QString userEmail = query.value(0).toString();

    // Find an active borrow transaction for this user and book
    query.prepare("SELECT transaction_id FROM transactionrecord WHERE email = :email AND book_id = :book_id AND amount = 1");
    query.bindValue(":email", userEmail);
    query.bindValue(":book_id", bookId);

    if (!query.exec() || !query.next()) {
        qDebug() << "No active borrow record found for this book!";
        return false;
    }

    int transactionId = query.value(0).toInt();

    // Delete the transaction record since it's completed
    query.prepare("DELETE FROM transactionrecord WHERE transaction_id = :transaction_id");
    query.bindValue(":transaction_id", transactionId);
    if (!query.exec()) {
        qDebug() << "Error deleting transaction record: " << query.lastError().text();
        return false;
    }

    // Increase book stock and update status to Available
    query.prepare("UPDATE books SET stock = stock + 1, status = 'Available' WHERE book_id = :book_id");
    query.bindValue(":book_id", bookId);
    if (!query.exec()) {
        qDebug() << "Error updating book: " << query.lastError().text();
        return false;
    }

    qDebug() << "Book returned successfully!";
    return true;
}

void DatabaseManager::printBookTableInfo() {
    QSqlQuery query("PRAGMA table_info(books)");
    qDebug() << "Books Table Structure:";
    while (query.next()) {
        qDebug() << "Column:" << query.value(1).toString() << " Type:" << query.value(2).toString();
    }
}

void DatabaseManager::printBookDataADMIN(){
    QSqlQuery query("SELECT * FROM books");

    qDebug() << "Books Table Data: ";
    while (query.next()){
        qDebug() << "Book ID: " << query.value("book_id").toInt()
        << "Title: " << query.value("title")
        << "Author: " << query.value("author")
        << "Genre: " << query.value("genre");
    }
}

void DatabaseManager::printTableDataADMIN(){
    QSqlQuery query("SELECT * FROM users");

    qDebug() << "Users Table Data:";
    while (query.next()) {
        qDebug() << "ID:" << query.value("user_id").toInt()
        << " Name:" << query.value("name").toString()
        << " Email:" << query.value("email").toString()
        << " Password:" << query.value("password").toString()
        << " Admin:" << query.value("is_admin").toBool();
    }
}

void DatabaseManager::printRecordTableDataADMIN(){
    QSqlQuery query("SELECT * FROM transactionrecord");

    qDebug() << "Transaction Table Data:";
    while (query.next()) {
        qDebug() << "ID:" << query.value("transaction_id").toInt()
        << " Email:" << query.value("email").toString()
        << " Book ID:" << query.value("book_id").toString()
        << " Amount:" << query.value("amount").toString();
    }
}
