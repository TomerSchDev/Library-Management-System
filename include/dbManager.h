#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QVariantMap>

// Define enums for actions and tables
enum class DbAction {
    Insert,
    Select,
    Update,
    Delete
};

enum class DbTable {
    Books,
    Clients,
    Families,
    BorrowRecords
};

// Define structs for table schemas
struct BookSchema {
    int id;
    QString title;
    QString isbn;
    int year;
    int copies;
    int borrowed_count;
};

struct ClientSchema {
    int id;
    QString name;
    QString surname;
    QString family;
};

struct FamilySchema {
    int id;
    QString name;
};

struct BorrowRecordSchema {
    int id;
    int book_id;
    int client_id;
    QString borrow_date;
    QString return_date;
    bool is_returned;
};

class DbManager
{
public:
    explicit DbManager(QSqlDatabase& db);
    ~DbManager();
    bool createTables() const;
    static QVariantMap getSchemaForTable(DbTable table);
    std::pair<bool, QSqlQuery> executeAction(DbAction action, DbTable table, const QVariantMap& args) const;

private:
    QSqlDatabase& m_db;
    static QString tableSchemaToSql(DbTable table);
    [[nodiscard]] QUuid generateUUID(const QString& args) const;
    QUuid generateUUID(const DbTable table, const QVariantMap& args) const;
    static QString getTableName(DbTable table);
    QString _key = "library_management_system_key";
};

#endif // DBMANAGER_H
