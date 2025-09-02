#include "book.h"

Book::Book(const QString& title, const QString& author, int year, int copies)
    : m_title(title), m_author(author), m_year(year), m_copies(copies) {}

QString Book::title() const {
    return m_title;
}

QString Book::author() const {
    return m_author;
}

int Book::year() const {
    return m_year;
}

int Book::copies() const {
    return m_copies;
}

void Book::setCopies(int copies) {
    m_copies = copies;
}

QString Book::toString() const {
    if (m_copies > 0) {
        return QString("%1 by %2 (%3) - %4 copies").arg(m_title, m_author, QString::number(m_year), QString::number(m_copies));
    } else {
        return QString("%1 by %2 (%3) - Not Available").arg(m_title, m_author, QString::number(m_year));
    }
}
