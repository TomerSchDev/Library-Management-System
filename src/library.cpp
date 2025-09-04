#include "library.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QUuid>

#include "windows/familyviewdialog.h"
#include "windows/clientDetailDialog.h"
#include "book.h"
#include "client.h"

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
    auto [success, query] = _dbManager->executeAction(DbAction::Select, DbTable::Books,{});
    if (!success)
    {
        qDebug() << "Error loading books from database:" << query.lastError().text();
        return;
    }
    while (query.next()) {
        _books.append(Book(query.value("id").toInt(),
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
    auto [success, query]  = _dbManager->executeAction(DbAction::Select, DbTable::Clients,{});
    if (!success)
    {
        qDebug() << "Error loading clients from database:" << query.lastError().text();
        return;
    }
    while (query.next()) {
        _clients.append(Client(query.value("id").toInt(),
                               query.value("name").toString(),
                               query.value("surname").toString(),
                               query.value("family").toString()));
    }
    emit clientsUpdated();
}

void Library::loadFamilies()
{
    _families.clear();
    auto [success, query] =_dbManager->executeAction(DbAction::Select, DbTable::Families,{});
    if (!success)
    {
        qDebug() << "Error loading families from database:" << query.lastError().text();
        return;
    }
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
    if (auto [success, query] =_dbManager->executeAction(DbAction::Insert, DbTable::Books, args); !success)
    {
        qDebug() << "Error adding book to database:" << query.lastError().text();
        return;
    }
    loadBooks();
}

void Library::removeBook(int index)
{
    if (index >= 0 && index < _books.size()) {
        QVariantMap args;
        args["id"] = _books[index].id();
        auto [success, query] =_dbManager->executeAction(DbAction::Delete, DbTable::Books, args);
        if (!success)
        {
            qDebug() << "Error removing book from database:" << query.lastError().text();
            return;
        }
        emit booksUpdated();
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
        if (auto [success, query] = _dbManager->executeAction(DbAction::Update, DbTable::Books, args); !success)
        {
            qDebug() << "Error updating book copies in database:" << query.lastError().text();
            return;
        }
    }
    emit booksUpdated();
}

void Library::removeCopy(int index)
{
    if (index >= 0 && index < _books.size()) {
        if (Book& book = _books[index]; book.copies() > 0) {
            book.setCopies(book.copies() - 1);
            QVariantMap args;
            args["id"] = book.id();
            args["copies"] = book.copies();
            if (auto [success, query] =_dbManager->executeAction(DbAction::Update, DbTable::Books, args); !success)
            {
                qDebug() << "Error updating book copies in database:" << query.lastError().text();
                return;
            }
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
    if (auto [success, query] =_dbManager->executeAction(DbAction::Insert, DbTable::Clients, args); !success)
    {
        qDebug() << "Error adding client to database:" << query.lastError().text();
        return;
    }
    loadClients();
}
void Library::addClient(const QString name, const QString surname, const QString family)
{
    saveFamily(family);

    QVariantMap args;
    args["name"] = name;
    args["surname"] = surname;
    args["family"] = family;
    if (auto [success, query] =_dbManager->executeAction(DbAction::Insert, DbTable::Clients, args); !success)
    {
        qDebug() << "Error adding client to database:" << query.lastError().text();
        return;
    }
    loadClients();
}

void Library::removeClient(const Client& client)
{
    QVariantMap args;
    args["id"] = client.id();
     if (auto [success, query] =_dbManager->executeAction(DbAction::Delete, DbTable::Clients, args);!success)
     {
         qDebug() << "Error removing client from database:" << query.lastError().text();
         return;
     }
    loadClients();
}

Client Library::getClientById(const int id) const
{
    QVariantMap args;
    args["id"] = id;
    auto [success, query] = _dbManager->executeAction(DbAction::Select, DbTable::Clients, args);
    if (!success)
    {
        qDebug() << "Error retrieving client from database:" << query.lastError().text();
        return {};
    }
    if (query.next()) {
        return Client(query.value("id").toInt(),
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
    auto [success, query] = _dbManager->executeAction(DbAction::Select, DbTable::Clients, args);
    if (!success)
    {
        qDebug() << "Error retrieving clients by family from database:" << query.lastError().text();
        return familyClients;
    }
    while (query.next()) {
        familyClients.append(Client(query.value("id").toInt(),
                                    query.value("name").toString(),
                                    query.value("surname").toString(),
                                    query.value("family").toString()));
    }
    return familyClients;
}

bool Library::updateClient(const int id, const QString& name, const QString& surname, const QString& family) const
{
    saveFamily(family);

    QVariantMap args;
    args["id"] = id;
    args["name"] = name;
    args["surname"] = surname;
    args["family"] = family;
    auto [success, query] = _dbManager->executeAction(DbAction::Update, DbTable::Clients, args);
    return success;
}

void Library::saveFamily(const QString& family) const
{
    QVariantMap args;
    args["name"] = family;
    if (auto [success, query]  =_dbManager->executeAction(DbAction::Select, DbTable::Families, args); !success)
    {
        qDebug() << "Error checking family in database:" << query.lastError().text();
        return;
    }
}



void Library::on_familyListWidget_doubleClicked(QWidget* p,const Client& client)
{
    FamilyViewDialog dialog(p);
    dialog.setFamilyInfo(client.family(), getClientsByFamilyName(client.family()));
    dialog.exec();
}

TransactionResult Library::borrowBook(const int clientId, const BorrowRecord& record)
{
    QVariantMap args;
    const Book *book = getBookById(record.bookId);
    if (!book)
    {
        return TransactionResult::Failure_BookNotFound;
    }
    if (book->copies() <= 0)
    {
        return TransactionResult::Failure_NoCopies;
    }
    // Check if the client has already borrowed this book and not returned it yet
    QVariantMap checkArgs;
    checkArgs["client_id"] = clientId;
    checkArgs["book_id"] = record.bookId;
    checkArgs["is_returned"] = 0; // Not returned yet
    auto [successCheck,queryCheck] = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, checkArgs);
    if (!successCheck)
    {
        return TransactionResult::Failure_DBFailed;
    }
    if (queryCheck.next()) {
        return TransactionResult::Failure_AlreadyBorrowed;
    }
    //check if available copies
    if (book->availableCopies()<=0)
    {
        return TransactionResult::Failure_NotAvailableBook;
    }

    args["client_id"] = clientId;
    args["book_id"] = record.bookId;
    args["borrow_date"] = record.borrowDate;
    args["return_date"] = record.returnDate;
    args["is_returned"] = 0;
    args["borrowed_count"] = book->borrowedCount()+ 1;
    auto [successB,queryB] = _dbManager->executeAction(DbAction::Insert, DbTable::BorrowRecords, args);
    if (!successB)
    {
        return TransactionResult::Failure_DBFailed;
    }
    // Decrease the number of available copies
    QVariantMap updateArgs;
    updateArgs["id"] = book->id();
    updateArgs["borrowed_count"] = book->copies() - 1;
    if (auto [success,queryR] = _dbManager->executeAction(DbAction::Update, DbTable::Books, updateArgs); !success)
    {
        qDebug()<< "Error updating book copies in database after borrowing, removing borrow:" << queryR.lastError().text();
        // Rollback: Remove the borrow record if updating book copies fails
        QVariantMap rollbackArgs;
        rollbackArgs["id"] = queryB.lastInsertId().toInt();
        if (auto [successRoll,queryRoll] =_dbManager->executeAction(DbAction::Delete, DbTable::BorrowRecords, rollbackArgs); !successRoll)
        {
            qDebug() << "Error rolling back borrow record after failed book update:" << queryRoll.lastError().text();
        }

        return TransactionResult::Failure_DBFailed;
    }
    loadBooks();
    return TransactionResult::Success;
}

TransactionResult Library::returnBook(const int& borrowRecordId) const
{
    QVariantMap args;
    args["id"] = borrowRecordId;
    args["is_returned"] = 1;
    if (auto [success,query] = _dbManager->executeAction(DbAction::Update, DbTable::BorrowRecords, args); !success)
    {
        return TransactionResult::Failure_DBFailed;
    }
    return TransactionResult::Success;

}

bool Library::extendBorrowTime(const int& borrowRecordId, const int days) const
{
    QVariantMap selectArgs;
    selectArgs["id"] = borrowRecordId;
    auto [success,query] = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, selectArgs);
    if (!success)
    {
        qDebug() << "Error retrieving borrow record from database:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        const QDate currentReturnDate = query.value("return_date").toDate();
        const QDate newReturnDate = currentReturnDate.addDays(days);

        QVariantMap updateArgs;
        updateArgs["id"] = borrowRecordId;
        updateArgs["return_date"] = newReturnDate.toString(Qt::ISODate);
        auto [success,query] = _dbManager->executeAction(DbAction::Update, DbTable::BorrowRecords, updateArgs);
        if (!success)
        {
            qDebug() << "Error updating borrow record in database:" << query.lastError().text();
            return false;
        }
        return true;
    }
    return false;
}

Book* Library::getBookById(int id) const
{
    QVariantMap args;
    args["id"] = id;
    auto [success,query] = _dbManager->executeAction(DbAction::Select, DbTable::Books, args);
    if (!success)
    {
        qDebug() << "Error retrieving book from database:" << query.lastError().text();
        return nullptr;
    }
    if (query.next())
    {
        return new Book(query.value("id").toInt(),
                        query.value("title").toString(),
                        query.value("author").toString(),
                        query.value("year").toInt(),
                        query.value("copies").toInt());
    }

    for (auto& book : _books)
    {
        if (book.id() == id) {
            return new Book(book.id(), book.title(), book.author(), book.year(), book.copies());
        }
    }

    return nullptr;
}

QList<BorrowRecord> Library::getBorrowRecordsByClientId(const int clientId) const
{
    QList<BorrowRecord> records;
    QVariantMap args;
    args["client_id"] = clientId;
    auto [success,query] = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, args);
    if (!success)
    {
        qDebug() << "Error retrieving borrow records from database:" << query.lastError().text();
        return records;
    }
    while (query.next()) {
        BorrowRecord record;
        record.id = query.value("id").toInt();
        record.clientId = query.value("client_id").toInt();
        record.bookId = query.value("book_id").toInt();
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
    auto [success,query] = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, args);
    if (!success)
    {
        qDebug() << "Error retrieving borrow records from database:" << query.lastError().text();
        return books;
    }
    while (query.next()) {
        const int bookId = query.value("book_id").toInt();
        // Assuming you have a way to get a Book object from its ID
        for (const auto& book : _books) {
            if (book.id() == bookId) {
                books.append(book);
                break;
            }
        }
    }
    return books;
}

QList<BorrowRecord> Library::getBorrowRecordsByBookId(int bookId) const
{
    QList<BorrowRecord> records;
    QVariantMap args;
    args["book_id"] = bookId;
    auto [success,query] = _dbManager->executeAction(DbAction::Select, DbTable::BorrowRecords, args);
    if (!success)
    {
        qDebug() << "Error retrieving borrow records from database:" << query.lastError().text();
        return records;
    }
    while (query.next()) {
        BorrowRecord record;
        record.id = query.value("id").toInt();
        record.clientId = query.value("client_id").toInt();
        record.bookId = query.value("book_id").toInt();
        record.borrowDate = query.value("borrow_date").toDate();
        record.returnDate = query.value("return_date").toDate();
        record.isReturned = query.value("is_returned").toBool();
        records.append(record);
    }
    return records;
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
