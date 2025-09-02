#include "client.h"
#include <QUuid>

Client::Client(const QString& name, const QString& surname, const QString& family, const QString& id)
    : m_name(name), m_surname(surname), m_family(family)
{
    if (id.isEmpty()) {
        m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    } else {
        m_id = id;
    }
}

QString Client::id() const {
    return m_id;
}

QString Client::name() const {
    return m_name;
}

QString Client::surname() const {
    return m_surname;
}

QString Client::family() const {
    return m_family;
}

const QList<Book>& Client::takenBooks() const {
    return m_takenBooks;
}

void Client::setId(const QString& id) {
    m_id = id;
}

void Client::takeBook(const Book& book) {
    m_takenBooks.append(book);
}

void Client::returnBook(const Book& book) {
    // This is a simple implementation. In a real-world app, you might match by ID.
    for (int i = 0; i < m_takenBooks.size(); ++i) {
        if (m_takenBooks.at(i).title() == book.title()) {
            m_takenBooks.removeAt(i);
            break;
        }
    }
}

QString Client::toString() const {
    return QString("%1 %2 (Family: %3, ID: %4)").arg(m_name, m_surname, m_family, m_id);
}
