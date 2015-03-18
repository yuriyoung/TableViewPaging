/**************************************************************************
** Copyright (C) 2013 Southgis Co.,Ltd. Written by Young Yori, All rights reserved.
**--------------------------------------------------------------------------------
**   Filename:LinearList.h
**   Author: Young Yori
**   Data: Administrator 2013/12/25 2013
**   Description:
**          This file is part of the Data Development Department Project and shall
**      only be used, modified, and distributed under the terms of the Data
**      Development Department this Source.
**
**      Included in the build system of the FreeType library.
**--------------------------------------------------------------------------------
**                      www.newdebug.com, 12319597@qq.com
**************************************************************************/

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    enum Status
    {
        Read,
        Write
    };

    explicit DatabaseManager(QObject *parent = 0);
    bool createConnect();
    bool createTable();

    inline QSqlDatabase database() const { return m_db; }
    Status status() const { return m_status; }
    void setStatus(DatabaseManager::Status status = Read) { m_status = status; }

signals:

public slots:

private:
    QSqlDatabase m_db;
    Status       m_status;
};

#endif // DATABASEMANAGER_H
