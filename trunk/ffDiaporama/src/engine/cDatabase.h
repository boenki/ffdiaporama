/*======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray<levray.dominique@bbox.fr>

    This program is free software;you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation;either version 2 of the License,or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY;without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program;if not,write to the Free Software Foundation,Inc.,
    51 Franklin Street,Fifth Floor,Boston,MA 02110-1301 USA.
  ======================================================================*/

#ifndef CDATABASE_H
#define CDATABASE_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

//*****************************************************************************
// Enum of all table type
//*****************************************************************************
enum eTypeTable {
    TypeTable_Undefined,
    TypeTable_SettingsTable,
    TypeTable_FolderTable
};

//*****************************************************************************
// cDatabase : encapsulate a SQLite3 database
//*****************************************************************************
class cDatabaseTable;
class cDatabase {
public:
    QSqlDatabase            db;
    QString                 dbPath;
    QList<cDatabaseTable *> Tables;

                            cDatabase(QString DBFNAME);
    virtual                 ~cDatabase();

    bool                    OpenDB();
    bool                    CloseDB();
    bool                    ResetDB();
    bool                    CheckDatabaseVersion();
    bool                    ValidateTables();
    QSqlError               LastError();
    cDatabaseTable          *GetTable(eTypeTable TableType);
};

//**********************************************************************************************
// cDatabaseTable : encapsulate a table
//      this class must not been used directly but must be use as ancestor by all table class
//**********************************************************************************************
class cDatabaseTable {
public:
    cDatabase               *Database;
    QString                 TableName;
    QString                 IndexKeyName;
    QString                 CreateTableQuery;
    QStringList             CreateIndexQuery;
    eTypeTable              TypeTable;
    qlonglong               NextIndex;

    explicit                cDatabaseTable(cDatabase *Database);
    virtual                 ~cDatabaseTable();

    virtual bool            CreateTable();
    virtual bool            ValidateTable();
    virtual bool            DoUpgradeTableVersion(qlonglong CurrentVersion);
};

//**********************************************************************************************
// cSettingsTable : encapsulate the settings table
//**********************************************************************************************
class cSettingsTable : public cDatabaseTable {
public:

    explicit                cSettingsTable(cDatabase *Database);

    virtual bool            CreateTable();

    virtual qlonglong       GetIntValue(QString SettingName,qlonglong DefaultValue);
    virtual bool            SetIntValue(QString SettingName,qlonglong Value);
    virtual QString         GetTextValue(QString SettingName,QString DefaultValue);
    virtual bool            SetTextValue(QString SettingName,QString Value);
    virtual bool            GetIntAndTextValue(QString SettingName,qlonglong *IntValue,QString *TextValue);
    virtual bool            SetIntAndTextValue(QString SettingName,qlonglong IntValue,QString TextValue);
};

//**********************************************************************************************
// cFolderTable : encapsulate folders in the table
//**********************************************************************************************
class cFolderTable : public cDatabaseTable {
public:

    explicit                cFolderTable(cDatabase *Database);

    qlonglong               GetFolderKey(QString FolderPath);
    QString                 GetFolderPath(qlonglong FolderKey);
};

#endif // CDATABASE_H
