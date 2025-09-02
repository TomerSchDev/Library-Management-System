#include "library.h"
#include "book.h"
#include "client.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QSet>

Library::Library() {
    if (connectToDatabase()) {
        if (createBookTable() && createClientTable() && createFamilyTable()) {
            loadBooks();
            loadClients();
            loadFamilies();
        }
    }
}

Library::~Library() {
    if (_db.isOpen()) {
        _db.close();
    }
}

bool Library::connectToDatabase() {
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

bool Library::createBookTable() {
    QSqlQuery query(_db);
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS books ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "title TEXT NOT NULL, "
                               "author TEXT NOT NULL, "
                               "year INTEGER NOT NULL, "
                               "copies INTEGER NOT NULL"
                               ");";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Error: failed to create books table:" << query.lastError().text();
        return false;
    } else {
        qDebug() << "Books table created or already exists.";
        return true;
    }
}

bool Library::createClientTable() {
    QSqlQuery query(_db);
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS clients ("
                               "id TEXT PRIMARY KEY, "
                               "name TEXT NOT NULL, "
                               "surname TEXT NOT NULL, "
                               "family TEXT NOT NULL"
                               ");";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Error: failed to create clients table:" << query.lastError().text();
        return false;
    } else {
        qDebug() << "Clients table created or already exists.";
        return true;
    }
}

bool Library::createFamilyTable() {
    QSqlQuery query(_db);
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS families ("
                               "family_name TEXT PRIMARY KEY"
                               ");";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Error: failed to create families table:" << query.lastError().text();
        return false;
    } else {
        qDebug() << "Families table created or already exists.";
        return true;
    }
}

void Library::addBook(const QString& title, const QString& author, int year, int copies) {
    if (!_db.isOpen()) {
        qDebug() << "Database is not open. Cannot add book.";
        return;
    }

    QSqlQuery query(_db);
    query.prepare("INSERT INTO books (title, author, year, copies) VALUES (:title, :author, :year, :copies)");
    query.bindValue(":title", title);
    query.bindValue(":author", author);
    query.bindValue(":year", year);
    query.bindValue(":copies", copies);

    if (!query.exec()) {
        qDebug() << "Error inserting book:" << query.lastError().text();
    } else {
        _books.append(Book(title, author, year, copies));
        emit booksUpdated();
    }
}

void Library::loadBooks() {
    if (!_db.isOpen()) {
        qDebug() << "Database is not open. Cannot load books.";
        return;
    }

    _books.clear();

    QSqlQuery query("SELECT title, author, year, copies FROM books", _db);
    if (!query.exec()) {
        qDebug() << "Error loading books:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        int year = query.value("year").toInt();
        int copies = query.value("copies").toInt();
        _books.append(Book(title, author, year, copies));
    }

    qDebug() << "Books loaded from database.";
}

void Library::addClient(const QString& name, const QString& surname, const QString& family) {
    if (!_db.isOpen()) {
        qDebug() << "Database is not open. Cannot add client.";
        return;
    }

    Client newClient(name, surname, family);
    QSqlQuery query(_db);
    query.prepare("INSERT INTO clients (id, name, surname, family) VALUES (:id, :name, :surname, :family)");
    query.bindValue(":id", newClient.id());
    query.bindValue(":name", newClient.name());
    query.bindValue(":surname", newClient.surname());
    query.bindValue(":family", newClient.family());

    if (!query.exec()) {
        qDebug() << "Error inserting client:" << query.lastError().text();
    } else {
        _clients.append(newClient);
        if (!_families.contains(family)) {
            _families.append(family);
            _families.sort();
            saveFamily(family);
        }
        emit clientsUpdated();
        emit familiesUpdated();
    }
}

void Library::loadClients() {
    if (!_db.isOpen()) {
        qDebug() << "Database is not open. Cannot load clients.";
        return;
    }

    _clients.clear();
    QSet<QString> familySet;

    QSqlQuery query("SELECT id, name, surname, family FROM clients", _db);
    if (!query.exec()) {
        qDebug() << "Error loading clients:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString id = query.value("id").toString();
        QString name = query.value("name").toString();
        QString surname = query.value("surname").toString();
        QString family = query.value("family").toString();
        _clients.append(Client(name, surname, family, id));
        familySet.insert(family);
    }
    _families.clear();
    for (const QString& family : familySet) {
        _families.append(family);
    }
    _families.sort();

    qDebug() << "Clients loaded from database.";
}

void Library::saveFamily(const QString& family) const
{
    if (!_db.isOpen()) {
        qDebug() << "Database is not open. Cannot save family.";
        return;
    }

    QSqlQuery query(_db);
    query.prepare("INSERT OR IGNORE INTO families (family_name) VALUES (:family_name)");
    query.bindValue(":family_name", family);
    if (!query.exec()) {
        qDebug() << "Error saving family:" << query.lastError().text();
    }
}

void Library::loadFamilies() {
    if (!_db.isOpen()) {
        qDebug() << "Database is not open. Cannot load families.";
        return;
    }

    _families.clear();
    QSqlQuery query("SELECT family_name FROM families", _db);
    if (!query.exec()) {
        qDebug() << "Error loading families:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        _families.append(query.value(0).toString());
    }
    _families.sort();
    qDebug() << "Families loaded from database.";
}

const QList<Book>& Library::allBooks() const {
    return _books;
}

const QList<Client>& Library::allClients() const {
    return _clients;
}

const QList<QString>& Library::allFamilies() const {
    return _families;
}

QList<Client> Library::getClientsByFamilyName(const QString& familyName) const {
    QList<Client> familyClients;
    for (const Client& client : _clients) {
        if (client.family() == familyName) {
            familyClients.append(client);
        }
    }
    return familyClients;
}

void Library::removeBook(int index) {
    if (index >= 0 && index < _books.size()) {
        const Book& bookToRemove = _books.at(index);
        QString title = bookToRemove.title();
        QString author = bookToRemove.author();
        int year = bookToRemove.year();

        _books.removeAt(index);

        if (_db.isOpen()) {
            QSqlQuery query(_db);
            query.prepare("DELETE FROM books WHERE title = :title AND author = :author AND year = :year");
            query.bindValue(":title", title);
            query.bindValue(":author", author);
            query.bindValue(":year", year);

            if (!query.exec()) {
                qDebug() << "Error removing book from database:" << query.lastError().text();
            } else {
                qDebug() << "Book removed from database.";
            }
        }
    }
    emit booksUpdated();
}

void Library::addCopies(int index, int numCopies) {
    if (index >= 0 && index < _books.size()) {
        Book& book = _books[index];
        book.setCopies(book.copies() + numCopies);

        if (_db.isOpen()) {
            QSqlQuery query(_db);
            query.prepare("UPDATE books SET copies = :copies WHERE title = :title AND author = :author AND year = :year");
            query.bindValue(":copies", book.copies());
            query.bindValue(":title", book.title());
            query.bindValue(":author", book.author());
            query.bindValue(":year", book.year());

            if (!query.exec()) {
                qDebug() << "Error updating book copies:" << query.lastError().text();
            } else {
                qDebug() << "Copies updated in database.";
            }
        }
    }
    emit booksUpdated();
}

void Library::removeCopy(int index) {
    if (index >= 0 && index < _books.size()) {
        Book& book = _books[index];
        if (book.copies() > 0) {
            book.setCopies(book.copies() - 1);

            if (_db.isOpen()) {
                QSqlQuery query(_db);
                query.prepare("UPDATE books SET copies = :copies WHERE title = :title AND author = :author AND year = :year");
                query.bindValue(":copies", book.copies());
                query.bindValue(":title", book.title());
                query.bindValue(":author", book.author());
                query.bindValue(":year", book.year());

                if (!query.exec()) {
                    qDebug() << "Error updating book copies:" << query.lastError().text();
                } else {
                    qDebug() << "Copy removed from database.";
                }
            }
        }
    }
    emit booksUpdated();
}
