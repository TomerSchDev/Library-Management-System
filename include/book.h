#ifndef LIBRARY_MANAGEMENT_SYSTEM_BOOK_H
#define LIBRARY_MANAGEMENT_SYSTEM_BOOK_H

#include <qsqlquery.h>
#include <QString>

class Book {
private:
    QString m_id;
    QString m_title;
    QString m_author;
    int m_year;
    int m_copies;
    int m_borrowed_count;

public:
    Book(QString  id, const QString& title, const QString& author, int year, int copies);

    // Getters
    [[nodiscard]] QString id() const;
    QString title() const;
    QString author() const;
    int year() const;
    int copies() const;
    QString toString() const;
    void setCopies(int copies);
    void addCopies(int num_copies);
    void reduceCopies(int i);
    void incrementBorrowedCount() { m_borrowed_count++; }
    int borrowedCount() const { return m_borrowed_count; }
    int availableCopies() const { return m_copies - m_borrowed_count; }
    void returnedBook()  { m_borrowed_count--; }
};

#endif //LIBRARY_MANAGEMENT_SYSTEM_BOOK_H
