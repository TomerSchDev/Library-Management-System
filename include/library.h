#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDate>
#include <QUuid>
#include "book.h"
#include "client.h"
#include "dbmanager.h"

// Forward declaration
struct BorrowRecord;

enum class TransactionResult
{
    Success,
    Failure_DBFailed,
    Failure_ClientNotFound,
    Failure_BookNotFound,
    Failure_ClientAlreadyExists,
    Failure_BookAlreadyExists,
    Failure_NotAvailableBook,
    Failure_NoCopies,
    Failure_AlreadyBorrowed,

};

class Library final : public QObject {
    Q_OBJECT

public:
    ~Library() override;
    static Library* instance()
    {
        static Library instance;
        return &instance;
    }
    // Book management
    void addBook(const QString& title, const QString& author, int year, int copies);
    void loadBooks();
    [[nodiscard]] const QList<Book>& allBooks() const;
    [[nodiscard]] QList<Book> getAvailableBooks() const;
    void removeBook(int index);
    void addCopies(int index, int numCopies);
    void removeCopy(int index);

    // Client management
    void addClient(const Client& client);
    void addClient(QString name, QString surname, QString family);
    void removeClient(const Client& client);
    void loadClients();
    [[nodiscard]] Client getClientById(const QString& id) const;
    [[nodiscard]] const QList<Client>& allClients() const;
    [[nodiscard]] const QList<QString>& allFamilies() const;
    [[nodiscard]] QList<Client> getClientsByFamilyName(const QString& familyName) const;
    bool updateClient(int id, const QString& name, const QString& surname, const QString& family) const;
    void on_familyListWidget_doubleClicked(const Client& client) const;

    // Borrow management
    TransactionResult borrowBook(int clientId, const BorrowRecord& record);
    TransactionResult returnBook(const int& borrowRecordId) const;
    bool extendBorrowTime(const int& borrowRecordId, int days) const;
    Book* getBookById(int id) const;
    [[nodiscard]] QList<BorrowRecord> getBorrowRecordsByClientId(int clientId) const;
    QList<Book> getBorrowedBooksByClient(const QString& clientId) const;

signals:
    void booksUpdated();
    void clientsUpdated();
    void familiesUpdated();

private:
    Library();
    bool connectToDatabase();
    void saveFamily(const QString& family) const;
    void loadFamilies();
    // Helper methods
    Book* findBookByTitleAndAuthor(const QString& title, const QString& author);

private:
    QList<Book> _books;
    QList<Client> _clients;
    QList<QString> _families;
    QSqlDatabase _db;
    DbManager* _dbManager; // Pointer to the new DbManager
};

struct BorrowRecord {
    int id;
    int bookId;
    int clientId;
    QDate borrowDate;
    QDate returnDate;
    bool isReturned;
};
#endif // LIBRARY_H
