/****************************************************************************
**
** Copyright (C) 2016 Jerzy Kozera
** Contact: https://go.zealdocs.org/l/contact
**
** This file is part of Zeal.
**
** Zeal is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Zeal is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Zeal. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "SQLiteDriver.h"
#include <sqlite3.h>

using namespace Zeal::Util;

SQLiteDriver::SQLiteDriver(const QString &path)
{
    sqlite3_initialize();
    if (sqlite3_open(path.toUtf8().constData(), &m_db) != SQLITE_OK) {
        updateLastError();
        if (m_db != nullptr) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }
}

SQLiteDriver::~SQLiteDriver()
{
    if (m_db != nullptr)
        sqlite3_close(m_db);
}

bool SQLiteDriver::isOpen()
{
    return m_db != nullptr;
}

void SQLiteDriver::updateLastError() {
    if (!m_db)
        return;
    m_lastError = QString(reinterpret_cast<const QChar *>(sqlite3_errmsg16(m_db)));
}

QString SQLiteDriver::lastError()
{
    return m_lastError;
}

QStringList SQLiteDriver::tables() {
    QStringList res;
    if (!isOpen())
        return res;

    QString sql = QLatin1String("SELECT name FROM sqlite_master WHERE type='table' "
                                "UNION ALL SELECT name FROM sqlite_temp_master WHERE type='table'");

    if (!sql.isEmpty() && execute(sql)) {
        while(next())
            res.append(stringValue(0));
    }

    return res;
}

void SQLiteDriver::finalize() {
    if (m_stmt == nullptr)
        return;

    sqlite3_finalize(m_stmt);
    m_stmt = 0;
}

bool SQLiteDriver::execute(const QString &queryStr)
{
    if (m_db == nullptr) {
        return false;
    }
    if (m_stmt != nullptr) {
        finalize();
    }
    m_lastError = QString();

    const void *pzTail = NULL;
    if (sqlite3_prepare16_v2(m_db, queryStr.constData(), (queryStr.size() + 1) * sizeof(QChar),
                             &m_stmt, &pzTail) != SQLITE_OK) {
       // "Unable to execute statement"
       updateLastError();
       finalize();
       return false;
   } else if (pzTail && !QString(reinterpret_cast<const QChar *>(pzTail)).trimmed().isEmpty()) {
       // Unable to execute multiple statements at a time
       updateLastError();
       finalize();
       return false;
   }
   return true;
}

bool SQLiteDriver::next()
{
    if (m_stmt == nullptr)
        return false;

    int res = sqlite3_step(m_stmt);

    switch(res) {
    case SQLITE_ROW:
        return true;
    case SQLITE_DONE:
    case SQLITE_CONSTRAINT:
    case SQLITE_ERROR:
    case SQLITE_MISUSE:
    case SQLITE_BUSY:
    default:
        updateLastError();
        return false;
    }
    return false;
}

QString SQLiteDriver::stringValue(int i)
{
    if (m_stmt == nullptr) {
        return QString();
    }
    return QString(reinterpret_cast<const QChar *>(
                       sqlite3_column_text16(m_stmt, i)),
                       sqlite3_column_bytes16(m_stmt, i) / sizeof(QChar));
}


sqlite3_int64 SQLiteDriver::intValue(int i)
{
    if (m_stmt == nullptr) {
        return 0;
    }
    return sqlite3_column_int64(m_stmt, i);
}
