#include "../include/library.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QSet>
#include <QUuid>
#include <QVariantMap>

#include "familyviewdialog.h"
#include "clientDetailDialog.h"
#include "../include/book.h"
#include "../include/client.h"

Library::Library()
{
    if (connectToDatabase()) {
        _dbManager = new DbManager(_db);
        if (!_dbManager->createTables()) {
            qDebug() << "Error: Failed to create database tables.";
        }
        loadBooks();
        loadClients();
        loadFamilies();
    }
}

Library::~Library()
{
    delete _dbManager;
    if (_db.isOpen()) {
        _db.close();
    }
}

bool Library::connectToDatabase()
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("library.db");

    if (!_db.open()) {
        qDebug() << "Error: connection with database failed:" << _db.lastError().text();
        return false;
    } else {
        qDebug() << "Database connection successful!";
        return true;
    }
}

void Library::loadBooks()
{
    _books.clear();
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::Books,{});
    while (query.next()) {
        _books.append(Book(query.value("id").toString(),
                           query.value("title").toString(),
                           query.value("author").toString(),
                           query.value("year").toInt(),
                           query.value("copies").toInt()));
    }
    emit booksUpdated();
}

const QList<Book>& Library::allBooks() const
{
    return _books;
}

QList<Book> Library::getAvailableBooks() const
{
    QList<Book> ret;
    for (const Book& book : _books)
    {
        if (book.copies() > 0)
            ret.append(book);
    }
    return ret;

}

void Library::loadClients()
{
    _clients.clear();
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::Clients,{});
    while (query.next()) {
        _clients.append(Client(query.value("id").toString(),
                               query.value("name").toString(),
                               query.value("surname").toString(),
                               query.value("family").toString()));
    }
    emit clientsUpdated();
}

void Library::loadFamilies()
{
    _families.clear();
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::Families,{});
    while (query.next()) {
        _families.append(query.value("name").toString());
    }
    emit familiesUpdated();
}

void Library::addBook(const QString& title, const QString& author, int year, int copies)
{
    QVariantMap args;
    args["title"] = title;
    args["author"] = author;
    args["year"] = year;
    args["copies"] = copies;
    _dbManager->executeAction(DbAction::Insert, DbTable::Books, args);
    loadBooks();
}

void Library::removeBook(int index)
{
    if (index >= 0 && index < _books.size()) {
        QVariantMap args;
        args["id"] = _books[index].id();
        _dbManager->executeAction(DbAction::Delete, DbTable::Books, args);
        _books.removeAt(index);
    }
}

void Library::addCopies(int index, int numCopies)
{
    if (index >= 0 && index < _books.size()) {
        Book& book = _books[index];
        book.setCopies(book.copies() + numCopies);

        QVariantMap args;
        args["id"] = book.id();
        args["copies"] = book.copies();
       QSqlQuery q = _dbManager->executeAction(DbAction::Update, DbTable::Books, args);
    }
    emit booksUpdated();
}

void Library::removeCopy(int index)
{
    if (index >= 0 && index < _books.size()) {
        Book& book = _books[index];
        if (book.copies() > 0) {
            book.setCopies(book.copies() - 1);
            QVariantMap args;
            args["id"] = book.id();
            args["copies"] = book.copies();
            QSqlQuery q =_dbManager->executeAction(DbAction::Update, DbTable::Books, args);
        }
    }
    emit booksUpdated();
}

void Library::addClient(const Client& client)
{
    saveFamily(client.family());

    QVariantMap args;
    args["id"] = client.id();
    args["name"] = client.name();
    args["surname"] = client.surname();
    args["family"] = client.family();
     QSqlQuery q =_dbManager->executeAction(DbAction::Insert, DbTable::Clients, args);
    loadClients();
}
void Library::addClient(const QString name, const QString surname, const QString family)
{
    saveFamily(family);

    QVariantMap args;
    args["name"] = name;
    args["surname"] = surname;
    args["family"] = family;
     QSqlQuery q =_dbManager->executeAction(DbAction::Insert, DbTable::Clients, args);
    loadClients();
}

void Library::removeClient(const Client& client)
{
    QVariantMap args;
    args["id"] = client.id();
     QSqlQuery q =_dbManager->executeAction(DbAction::Delete, DbTable::Clients, args);
    loadClients();
}

Client Library::getClientById(const QString& id) const
{
    QVariantMap args;
    args["id"] = id;
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::Clients, args);
    if (query.next()) {
        return Client(query.value("id").toString(),
                      query.value("name").toString(),
                      query.value("surname").toString(),
                      query.value("family").toString());
    }
    return {};
}

const QList<Client>& Library::allClients() const
{
    return _clients;
}

const QList<QString>& Library::allFamilies() const
{
    return _families;
}

QList<Client> Library::getClientsByFamilyName(const QString& familyName) const
{
    QList<Client> familyClients;
    QVariantMap args;
    args["family"] = familyName;
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::Clients, args);
    while (query.next()) {
        familyClients.append(Client(query.value("id").toString(),
                                    query.value("name").toString(),
                                    query.value("surname").toString(),
                                    query.value("family").toString()));
    }
    return familyClients;
}

bool Library::updateClient(const QString& id, const QString& name, const QString& surname, const QString& family) const
{
    saveFamily(family);

    QVariantMap args;
    args["id"] = id;
    args["name"] = name;
    args["surname"] = surname;
    args["family"] = family;
    QSqlQuery query = _dbManager->executeAction(DbAction::Update, DbTable::Clients, args);
    return query.exec();
}

void Library::saveFamily(const QString& family) const
{
    QVariantMap args;
    args["name"] = family;
    QSqlQuery checkQuery = _dbManager->executeAction(DbAction::Select, DbTable::Families, args);
    if (!checkQuery.next()) { // Family doesn't exist
         QSqlQuery q =_dbManager->executeAction(DbAction::Insert, DbTable::Families, args);
    }
}



void Library::on_familyListWidget_doubleClicked(const Client& client) const
{
    FamilyViewDialog dialog;
    dialog.setFamilyInfo(client.family(), getClientsByFamilyName(client.family()));
    dialog.exec();
}

bool Library::borrowBook(const QString& clientId, Book& book) const
{
    QVariantMap args;
    args["client_id"] = clientId;
    args["book_id"] = book.id();
    args["borrow_date"] = QDate::currentDate().toString(Qt::ISODate);
    args["return_date"] = QDate::currentDate().addMonths(1).toString(Qt::ISODate);
    args["is_returned"] = 0;
    QSqlQuery query = _dbManager->executeAction(DbAction::Insert, DbTable::BorrowRecords, args);
    book.reduceCopies(1);
    return query.exec();
}

bool Library::returnBook(int borrowRecordId)
{
    QVariantMap args;
    args["id"] = borrowRecordId;
    args["is_returned"] = 1;
    QSqlQuery query = _dbManager->executeAction(DbAction::Update, DbTable::BorrowRecords, args);
    return query.exec();
}

bool Library::extendBorrowTime(int borrowRecordId, int days)
{
    QVariantMap selectArgs;
    selectArgs["id"] = borrowRecordId;
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, selectArgs);
    if (query.next()) {
        QDate currentReturnDate = query.value("return_date").toDate();
        QDate newReturnDate = currentReturnDate.addDays(days);

        QVariantMap updateArgs;
        updateArgs["id"] = borrowRecordId;
        updateArgs["return_date"] = newReturnDate.toString(Qt::ISODate);
        QSqlQuery updateQuery = _dbManager->executeAction(DbAction::Update, DbTable::BorrowRecords, updateArgs);
        return updateQuery.exec();
    }
    return false;
}

Book* Library::getBookById(QString id) const
{
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::Books, {{"id", id}});
    if (query.next())
    {
        return new Book(query.value("id").toString(),
                        query.value("title").toString(),
                        query.value("author").toString(),
                        query.value("year").toInt(),
                        query.value("copies").toInt());
    }
    return nullptr;
}

QList<BorrowRecord> Library::getBorrowRecordsByClientId(const QString& clientId) const
{
    QList<BorrowRecord> records;
    QVariantMap args;
    args["client_id"] = clientId;
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, args);
    while (query.next()) {
        BorrowRecord record;
        record.id = query.value("id").toInt();
        record.clientId = query.value("client_id").toString();
        record.bookId = query.value("book_id").toString();
        record.borrowDate = query.value("borrow_date").toDate();
        record.returnDate = query.value("return_date").toDate();
        record.isReturned = query.value("is_returned").toBool();
        records.append(record);
    }
    return records;
}

QList<Book> Library::getBorrowedBooksByClient(const QString& clientId) const
{
    QList<Book> books;
    QVariantMap args;
    args["client_id"] = clientId;
    QSqlQuery query = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, args);

    if (query.exec()) {
        while (query.next()) {
            QString bookId = query.value("book_id").toString();
            // Assuming you have a way to get a Book object from its ID
            for (const auto& book : _books) {
                if (book.id() == bookId) {
                    books.append(book);
                    break;
                }
            }
        }
    } else {
        qDebug() << "Error getting borrowed books:" << query.lastError().text();
    }
    return books;
}

Book* Library::findBookByTitleAndAuthor(const QString& title, const QString& author)
{
    for (auto& book : _books) {
        if (book.title() == title && book.author() == author) {
            return &book;
        }
    }
    return nullptr;
}
