//
// Created by Tomer on 02/09/2025.
//

#ifndef CLIENT_H
#define CLIENT_H



#include <QString>
#include <QList>
#include "book.h"

class Client {
public:
    Client(const QString& name, const QString& surname, const QString& family, const QString& id = "");

    // Getters
    QString id() const;
    QString name() const;
    QString surname() const;
    QString family() const;
    const QList<Book>& takenBooks() const;

    // Setters
    void setId(const QString& id);
    void takeBook(const Book& book);
    void returnBook(const Book& book);
    QString toString() const;

private:
    QString m_id;
    QString m_name;
    QString m_surname;
    QString m_family;
    QList<Book> m_takenBooks;
};

#endif //CLIENT_H
