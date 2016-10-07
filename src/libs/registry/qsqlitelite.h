#ifndef QSQLITELITE_H
#define QSQLITELITE_H

#include <QString>
#include <QStringList>

#include <sqlite3.h>

class QSQLiteLite
{
public:
    explicit QSQLiteLite(const QString &path);
    ~QSQLiteLite();
    bool isOpen();
    QString lastError();
    QStringList tables();
    bool execute(const QString &queryStr);
    bool next();
    void finalize();
    QString stringValue(int index);
    sqlite3_int64 intValue(int index);
    sqlite3* handle();

private:
    void updateLastError();
    QString m_lastError;
    sqlite3* m_db = nullptr;
    sqlite3_stmt *m_stmt = nullptr;
};

#endif // QSQLITELITE_H
