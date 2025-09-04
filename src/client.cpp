#include "client.h"
#include "book.h"



Client::Client(int id, const QString& name, const QString& surname, const QString& family) : _id(id), _name(name), _surname(surname), _family(family)
{
}

Client::Client(const QString& name, const QString& surname, const QString& family)
    : _name(name), _surname(surname), _family(family)
{
     _id = -1; // Indicate that the ID is not set
}
int Client::id() const
{
    return _id;
}

QString Client::name() const
{
    return _name;
}
bool Client::operator ==(const Client& other) const
{
    return _id == other._id;
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
