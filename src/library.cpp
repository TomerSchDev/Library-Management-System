#include "library.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSet>
#include <qwidget.h>

#include "familyviewdialog.h"

Library::Library()
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("library.db");

    if (!_db.open()) {
        qDebug() << "Error: connection with database failed" << _db.lastError().text();
    } else {
        qDebug() << "Database: connection ok";
        if (createBookTable() && createClientTable() && createFamilyTable()) {
            loadBooks();
            loadClients();
        }
    }
}

Library::~Library()
{
    if (_db.isOpen()) {
        _db.close();
    }
}

bool Library::createBookTable()
{
    QSqlQuery query(_db);
    return query.exec("CREATE TABLE IF NOT EXISTS books (id TEXT PRIMARY KEY , title TEXT, author TEXT, year INTEGER, copies INTEGER)");
}

bool Library::createClientTable()
{
    QSqlQuery query(_db);
    return query.exec("CREATE TABLE IF NOT EXISTS clients (id TEXT PRIMARY KEY, name TEXT, surname TEXT, family TEXT)");
}

bool Library::createFamilyTable()
{
    QSqlQuery query(_db);
    return query.exec("CREATE TABLE IF NOT EXISTS families (name TEXT PRIMARY KEY)");
}

QList<Book> Library::allBooks() const
{
    return _books;
}

QList<Client> Library::allClients() const
{
    return _clients;
}

QList<QString> Library::allFamilies() const
{
    return _families;
}

QList<Client> Library::getClientsByFamily(const QString& familyName) const
{
    QList<Client> clientsInFamily;
    for (const auto& client : _clients) {
        if (client.family() == familyName) {
            clientsInFamily.append(client);
        }
    }
    return clientsInFamily;
}

QList<Book> Library::getBorrowedBooksByClient(const QString& clientId) const
{
    // This is a placeholder. You'll need to implement a separate table
    // for borrowed books and join on that.
    QList<Book> borrowedBooks;
    for (const auto& client : _clients) {
        if (client.id() == clientId) {
            return client.borrowedBooks();
        }
    }
    return borrowedBooks;
}

void Library::addBook(const QString& title, const QString& author, int year, int copies)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO books (id, title, author, year, copies) VALUES (?,?, ?, ?, ?)");
    QString id = QUuid::createUuid().toString();
    query.addBindValue( id);
    query.addBindValue(title);
    query.addBindValue(author);
    query.addBindValue(year);
    query.addBindValue(copies);
    if (query.exec()) {
        loadBooks();
    } else {
        qDebug() << "Error adding book:" << query.lastError().text();
    }
}

void Library::removeBook(int index)
{
    if (index >= 0 && index < _books.size()) {
        QSqlQuery query(_db);
        query.prepare("DELETE FROM books WHERE id = ?");
        query.addBindValue(_books.at(index).id());
        if (query.exec()) {
            _books.removeAt(index);
        } else {
            qDebug() << "Error removing book:" << query.lastError().text();
        }
    }
}

void Library::addCopies(int index, int numCopies)
{
    if (index >= 0 && index < _books.size()) {
        Book book = _books.at(index);
        book.addCopies(numCopies);
        QSqlQuery query(_db);
        query.prepare("UPDATE books SET copies = ? WHERE id = ?");
        query.addBindValue(book.copies());
        query.addBindValue(book.id());
        if (query.exec()) {
            loadBooks();
        } else {
            qDebug() << "Error adding copies:" << query.lastError().text();
        }
    }
}

void Library::addClient(const QString& name, const QString& surname, const QString& family)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO clients (id, name, surname, family) VALUES (?, ?, ?, ?)");
    QString id = QUuid::createUuid().toString();
    query.addBindValue(id);
    query.addBindValue(name);
    query.addBindValue(surname);
    query.addBindValue(family);
    if (query.exec()) {
        loadClients();
        saveFamily(family);
        loadFamilies();
    } else {
        qDebug() << "Error adding client:" << query.lastError().text();
    }
}

void Library::saveFamily(const QString& family)
{
    if (!_families.contains(family)) {
        QSqlQuery query(_db);
        query.prepare("INSERT INTO families (name) VALUES (?)");
        query.addBindValue(family);
        if (!query.exec()) {
            qDebug() << "Error saving family:" << query.lastError().text();
        }
    }
}

void Library::loadFamilies()
{
    _families.clear();
    QSqlQuery query("SELECT name FROM families", _db);
    while (query.next()) {
        _families.append(query.value(0).toString());
    }
}

void Library::loadBooks()
{
    _books.clear();
    QSqlQuery query("SELECT * FROM books", _db);
    int idCol = query.record().indexOf("id");
    int titleCol = query.record().indexOf("title");
    int authorCol = query.record().indexOf("author");
    int yearCol = query.record().indexOf("year");
    int copiesCol = query.record().indexOf("copies");

    while (query.next()) {
        _books.append(Book(
            query.value(idCol).toString(),
            query.value(titleCol).toString(),
            query.value(authorCol).toString(),
            query.value(yearCol).toInt(),
            query.value(copiesCol).toInt()
        ));
    }
    emit booksUpdated();
}

void Library::loadClients()
{
    _clients.clear();
    QSqlQuery query("SELECT * FROM clients", _db);
    int idCol = query.record().indexOf("id");
    int nameCol = query.record().indexOf("name");
    int surnameCol = query.record().indexOf("surname");
    int familyCol = query.record().indexOf("family");

    while (query.next()) {
        _clients.append(Client(
            query.value(idCol).toString(),
            query.value(nameCol).toString(),
            query.value(surnameCol).toString(),
            query.value(familyCol).toString()
        ));
    }
    loadFamilies();
}
void Library::on_familyListWidget_doubleClicked_rep(QString family,QWidget* parent) const
{
    if (family.isEmpty()) return;
    FamilyViewDialog dialog(parent);
    dialog.setFamilyInfo(family,this->getClientsByFamily(family));
    dialog.exec();
}
