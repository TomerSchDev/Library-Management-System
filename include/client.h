#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QList>
#include <QUuid>

class Book; // Forward declaration

class Client
{
public:
    Client(int id = -1,
           const QString& name = "",
           const QString& surname = "",
           const QString& family = "");
    Client(const QString& name, const QString& surname, const QString& family);

    int id() const;
    QString name() const;
    bool operator==(const Client& other) const;
    QString surname() const;
    QString family() const;
    QList<Book> borrowedBooks() const;

    void setName(const QString& name);
    void setSurname(const QString& surname);
    void setFamily(const QString& family);
    void setBorrowedBooks(const QList<Book>& books);

    QString toString() const;

private:
    int _id;
    QString _name;
    QString _surname;
    QString _family;
    QList<Book> _borrowedBooks;
};

#endif // CLIENT_H
