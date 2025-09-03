#include "dbManager.h"
#include <QSqlError>
#include <QUuid>

DbManager::DbManager(QSqlDatabase& db)
    : m_db(db)
{
}

DbManager::~DbManager()
{
}

bool DbManager::createTables() const
{
    if (!m_db.isOpen()) {
        qDebug() << "Database is not open.";
        return false;
    }

    QSqlQuery query(m_db);

    // Create tables based on their schemas
    if (!query.exec(tableSchemaToSql(DbTable::Books))) {
        qDebug() << "Failed to create Books table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Books table created or already exists.";

    if (!query.exec(tableSchemaToSql(DbTable::Clients))) {
        qDebug() << "Failed to create Clients table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Clients table created or already exists.";

    if (!query.exec(tableSchemaToSql(DbTable::Families))) {
        qDebug() << "Failed to create Families table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Families table created or already exists.";

    if (!query.exec(tableSchemaToSql(DbTable::BorrowRecords))) {
        qDebug() << "Failed to create BorrowRecords table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Borrow records table created or already exists.";

    return true;
}



auto DbManager::getSchemaForTable(const DbTable table) -> QVariantMap
{
    QVariantMap schema;
    switch (table) {
    case DbTable::Books:
        schema["id"] = "TEXT PRIMARY KEY";//
        schema["title"] = "TEXT NOT NULL";
        schema["author"] = "TEXT NOT NULL";
        schema["year"] = "INTEGER NOT NULL";
        schema["copies"] = "INTEGER NOT NULL";
        schema["borrowed_count"] = "INTEGER DEFAULT 0";
        break;
    case DbTable::Clients:
        schema["id"] = "TEXT PRIMARY KEY";
        schema["name"] = "TEXT NOT NULL";
        schema["surname"] = "TEXT NOT NULL";
        schema["family"] = "TEXT NOT NULL";
        break;
    case DbTable::Families:
        schema["name"] = "TEXT PRIMARY KEY";
        break;
    case DbTable::BorrowRecords:
        schema["id"] = "INTEGER PRIMARY KEY AUTOINCREMENT";
        schema["client_id"] = "TEXT NOT NULL";
        schema["book_id"] = "TEXT NOT NULL";
        schema["borrow_date"] = "TEXT NOT NULL";
        schema["return_date"] = "TEXT NOT NULL";
        schema["is_returned"] = "INTEGER DEFAULT 0";
        break;
    default:
        break;
    }
    return schema;
}


QString DbManager::tableSchemaToSql(DbTable table)
{
    QVariantMap schema = getSchemaForTable(table);
    if (schema.isEmpty()) {
        return "";
    }

    QStringList columns;
    for (auto it = schema.constBegin(); it != schema.constEnd(); ++it) {
        columns << QString("%1 %2").arg(it.key(), it.value().toString());
    }

    QString foreignKeys;
    if (table == DbTable::BorrowRecords) {
        foreignKeys = ", FOREIGN KEY (client_id) REFERENCES clients(id), FOREIGN KEY (book_id) REFERENCES books(id)";
    }

    return QString("CREATE TABLE IF NOT EXISTS %1 (%2%3);")
        .arg(getTableName(table), columns.join(", "), foreignKeys);
}
QUuid DbManager::generateUUID(const QString& args) const
{
    return QUuid::createUuidV3(QUuid::fromString(args), _key);
}

QUuid DbManager::generateUUID(const DbTable table, const QVariantMap& args) const
{
    QString tableName = getTableName(table);
    QString argsString;
    for (auto it = args.constBegin(); it != args.constEnd(); ++it)
    {
        argsString += QString("_%1").arg(it.value().toString());
    }
    return generateUUID(tableName + argsString);
}

QString DbManager::getTableName(DbTable table)
{
    switch (table) {
    case DbTable::Books: return "books";
    case DbTable::Clients: return "clients";
    case DbTable::Families: return "families";
    case DbTable::BorrowRecords: return "borrow_records";
    }
    return "";
}
QSqlQuery DbManager::executeAction(DbAction action, DbTable table, const QVariantMap& args)
{
    QSqlQuery query(m_db);
    QString tableName = getTableName(table);
    QString sql;

    switch (action) {
    case DbAction::Insert: {
        // Get the schema for the table to ensure we use the correct keys and order
        QVariantMap schema = getSchemaForTable(table);

        QStringList fields;
        QStringList values;
        QVariantMap argsMap;

        // Iterate through the schema to build the SQL query
        for (auto it = schema.begin(); it != schema.end(); ++it) {
            QString fieldName = it.key();
            // Handle ID generation if it's not provided in the arguments
            if (fieldName == "id" &&  (!args.contains("id") || args["id"].toString().isEmpty())) {
                argsMap[fieldName] = generateUUID(table,args);
            }

            // Populate the lists for the SQL query
            fields << fieldName;
            values << ":" + fieldName;
        }

        sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(fields.join(", ")).arg(values.join(", "));
        query.prepare(sql);

        // Bind values from the args map
        for (auto it = args.begin(); it != args.end(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }
        break;
    }

    case DbAction::Select: {
        QStringList conditions;
        sql = QString("SELECT * FROM %1").arg(tableName);
        for (auto it = args.begin(); it != args.end(); ++it) {
            conditions << QString("%1 = :%1").arg(it.key());
        }
        if (!conditions.isEmpty()) {
            sql += " WHERE " + conditions.join(" AND ");
            query.prepare(sql);
            for (auto it = args.begin(); it != args.end(); ++it) {
                query.bindValue(":" + it.key(), it.value());
            }
        } else {
            query.prepare(sql);
        }
        break;
    }

    case DbAction::Update: {
        QStringList fields;
        sql = QString("UPDATE %1 SET ").arg(tableName);
        for (auto it = args.begin(); it != args.end(); ++it) {
            if (it.key() != "id") {
                fields << QString("%1 = :%1").arg(it.key());
            }
        }
        sql += fields.join(", ") + " WHERE id = :id";
        query.prepare(sql);
        for (auto it = args.begin(); it != args.end(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }
        break;
    }

    case DbAction::Delete: {
        sql = QString("DELETE FROM %1 WHERE id = :id").arg(tableName);
        query.prepare(sql);
        query.bindValue(":id", args.value("id"));
        break;
    }
    }

    if (!query.exec()) {
        qDebug() << "Error executing action" << (int)action << "on table" << (int)table << ":" << query.lastError().text();
    }
    return query;
}


