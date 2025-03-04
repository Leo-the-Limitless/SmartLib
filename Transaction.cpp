#include "Transaction.h"

Transaction::Transaction(int id, int userId, int bookId, const QString &borrowDate, const QString &returnDate, const QString &status)
    : id(id), userId(userId), bookId(bookId), borrowDate(borrowDate), returnDate(returnDate), status(status) {}

int Transaction::getId() const { return id; }
int Transaction::getUserId() const { return userId; }
int Transaction::getBookId() const { return bookId; }
QString Transaction::getBorrowDate() const { return borrowDate; }
QString Transaction::getReturnDate() const { return returnDate; }
QString Transaction::getStatus() const { return status; }
