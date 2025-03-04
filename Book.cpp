#include "Book.h"

Book::Book(int id, const QString &title, const QString &author, const QString &genre, int stock)
    : id(id), title(title), author(author), genre(genre), stock(stock) {}

int Book::getId() const { return id; }
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
QString Book::getGenre() const { return genre; }
int Book::getStock() const { return stock; }
void Book::setStock(int newStock) { stock = newStock; }
