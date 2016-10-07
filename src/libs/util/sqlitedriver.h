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

#ifndef SQLiteDriver_H
#define SQLiteDriver_H

#include <QString>
#include <QStringList>

#include <sqlite3.h>

namespace Zeal {
namespace Util {

class SQLiteDriver
{
public:
    explicit SQLiteDriver(const QString &path);
    ~SQLiteDriver();
    bool isOpen();
    QString lastError();
    QStringList tables();
    bool execute(const QString &queryStr);
    bool next();
    void finalize();
    QString stringValue(int index);
    qint64 intValue(int index);
    sqlite3* handle();

private:
    void updateLastError();
    QString m_lastError;
    sqlite3* m_db = nullptr;
    sqlite3_stmt *m_stmt = nullptr;
};

} // namespace Util
} // namespace Zeal

#endif // SQLiteDriver_H
