#ifndef LIBRARY_H
#define LIBRARY_H

#include <QSqlDatabase>
#include <QList>
#include <QSqlRecord>
#include <QUuid>
#include <QObject>
#include "book.h"
#include "client.h"

class Library : public QObject
{
    Q_OBJECT

public:
    Library();
    ~Library();

    QList<Book> allBooks() const;
    QList<Client> allClients() const;
    QList<QString> allFamilies() const;

    QList<Client> getClientsByFamily(const QString& familyName) const;
    QList<Book> getBorrowedBooksByClient(const QString& clientId) const;

    void addBook(const QString& title, const QString& author, int year, int copies);
    void removeBook(int index);
    void addCopies(int index, int numCopies);

    void addClient(const QString& name, const QString& surname, const QString& family);
    void on_familyListWidget_doubleClicked_rep(QString family, QWidget* parent) const;

    static Library* getInstance()
    {
        static Library instance;
        return &instance;
    };
    signals:
        void booksUpdated();
    void clientsUpdated();
    void familiesUpdated();


private:
    bool createBookTable();
    bool createClientTable();
    bool createFamilyTable();
    void loadBooks();
    void loadClients();
    void loadFamilies();
    void saveFamily(const QString& family);

    QSqlDatabase _db;
    QList<Book> _books;
    QList<Client> _clients;
    QList<QString> _families;
};

#endif // LIBRARY_H
