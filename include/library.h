#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "book.h"
#include "client.h"

class Library : public QObject {
    Q_OBJECT

public:
    Library();
    ~Library();

    void addBook(const QString& title, const QString& author, int year, int copies);
    void loadBooks();
    const QList<Book>& allBooks() const;
    void removeBook(int index);
    void addCopies(int index, int numCopies);
    void removeCopy(int index);

    void addClient(const QString& name, const QString& surname, const QString& family);
    void loadClients();
    const QList<Client>& allClients() const;
    const QList<QString>& allFamilies() const;
    QList<Client> getClientsByFamilyName(const QString& familyName) const;

    signals:
        void booksUpdated();
    void clientsUpdated();
    void familiesUpdated();

private:
    bool connectToDatabase();
    bool createBookTable();
    bool createClientTable();
    bool createFamilyTable();
    void saveFamily(const QString& family) const;
    void loadFamilies();

private:
    QList<Book> _books;
    QList<Client> _clients;
    QList<QString> _families;
    QSqlDatabase _db;
};

#endif // LIBRARY_H
