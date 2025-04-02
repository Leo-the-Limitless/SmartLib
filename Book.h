#ifndef BOOK_H
#define BOOK_H

#include <QString>

class Book {
public:
    Book(int id, const QString &title, const QString &author, const QString &genre, int stock);
    int getId() const;
    QString getTitle() const;
    QString getAuthor() const;
    QString getGenre() const;
    int getStock() const;
    void setStock(int newStock);

private:
    int id;
    QString title;
    QString author;
    QString genre;
    int stock;
};

#endif // BOOK_H
