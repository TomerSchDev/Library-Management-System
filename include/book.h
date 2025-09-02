#ifndef LIBRARY_MANAGEMENT_SYSTEM_BOOK_H
#define LIBRARY_MANAGEMENT_SYSTEM_BOOK_H

#include <QString>

class Book {
private:
    QString m_title;
    QString m_author;
    int m_year;
    int m_copies;

public:
    Book(const QString& title, const QString& author, int year, int copies);

    // Getters
    QString title() const;
    QString author() const;
    int year() const;
    int copies() const;
    QString toString() const;

    // Setters
    void setCopies(int copies);
};

#endif //LIBRARY_MANAGEMENT_SYSTEM_BOOK_H
