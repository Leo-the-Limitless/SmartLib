#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QDebug>

class DatabaseManager {
public:
    DatabaseManager();
    bool openDatabase();
    void closeDatabase();
    void initializeDatabase(); // Function to create tables if they don’t exist
    bool borrowBook(int userId, int bookId, QString &errorMessage);
    bool returnBook(int userId, int bookId);
    void printBookTableInfo();
    bool checkCredentials(const QString &email, const QString &password, int &userId, QString &name, bool &isAdmin);
    void printTableDataADMIN();
    void printRecordTableDataADMIN();
    void printBookDataADMIN();
private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
