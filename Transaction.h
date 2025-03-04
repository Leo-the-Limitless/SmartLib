#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>

class Transaction {
public:
    Transaction(int id, int userId, int bookId, const QString &borrowDate, const QString &returnDate, const QString &status);
    int getId() const;
    int getUserId() const;
    int getBookId() const;
    QString getBorrowDate() const;
    QString getReturnDate() const;
    QString getStatus() const;

private:
    int id;
    int userId;
    int bookId;
    QString borrowDate;
    QString returnDate;
    QString status;
};

#endif // TRANSACTION_H
