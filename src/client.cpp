#include "../include/client.h"
#include "../include/book.h"

Client::Client(const QString& id, const QString& name, const QString& surname, const QString& family)
    : _id(id), _name(name), _surname(surname), _family(family)
{
}

QString Client::id() const
{
    return _id;
}

QString Client::name() const
{
    return _name;
}

QString Client::surname() const
{
    return _surname;
}

QString Client::family() const
{
    return _family;
}

QList<Book> Client::borrowedBooks() const
{
    return _borrowedBooks;
}

void Client::setName(const QString& name)
{
    _name = name;
}

void Client::setSurname(const QString& surname)
{
    _surname = surname;
}

void Client::setFamily(const QString& family)
{
    _family = family;
}

void Client::setBorrowedBooks(const QList<Book>& books)
{
    _borrowedBooks = books;
}

QString Client::toString() const
{
    return QString("%1 %2").arg(_name, _surname);
}
