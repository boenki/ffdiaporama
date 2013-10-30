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

#include "cDatabase.h"

#define DATABASEVERSION 1       // Current database version

void DisplayLastSQLError(QSqlQuery *Query) {
    ToLog(LOGMSG_CRITICAL,Query->lastQuery());
    ToLog(LOGMSG_CRITICAL,Query->lastError().text());
}

//**************************************************************************************************************************
// cDatabase : encapsulate a SQLite3 database
//**************************************************************************************************************************

cDatabase::cDatabase(QString DBFNAME) {
    dbPath=DBFNAME;
}

//=====================================================================================================

cDatabase::~cDatabase() {
    while (!Tables.isEmpty()) delete Tables.takeLast();
    CloseDB();
}

//=====================================================================================================

bool cDatabase::OpenDB() {
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    QDir ApplicationFolder(QFileInfo(dbPath).absolutePath());
    if (!ApplicationFolder.exists()) ApplicationFolder.mkpath(ApplicationFolder.path());
    db.setDatabaseName(dbPath);

    // Open databasee
    if (!db.open()) return false;

    // Disable journalisation to speed query
    QSqlQuery Query(db);
    if (!Query.exec("PRAGMA journal_mode=OFF")) {
        DisplayLastSQLError(&Query);
        return false;
    }
    return true;
}

//=====================================================================================================
// Close database

bool cDatabase::CloseDB() {
    if (db.isOpen()) db.close();
    return true;
}

//=====================================================================================================
// Reset database and recreate it

bool cDatabase::ResetDB() {
    bool Ret=CloseDB();
    if (QFileInfo(dbPath).exists()) Ret=Ret && QFile::remove(dbPath);
    Ret=Ret && OpenDB();
    if (db.isOpen()) foreach (cDatabaseTable *Table,Tables) Ret=Ret && Table->CreateTable();
    return db.isOpen();
}

//=====================================================================================================
// Return table object corresponding to TableName

cDatabaseTable *cDatabase::GetTable(eTypeTable TableType) {
    foreach (cDatabaseTable *Table,Tables) if (Table->TypeTable==TableType) return Table;
    return NULL;
}

//=====================================================================================================
// Get database version from setting table and upgrade database if needed

bool cDatabase::CheckDatabaseVersion() {
    cSettingsTable  *SettingsTable=(cSettingsTable *)GetTable(TypeTable_SettingsTable);
    bool            Ret=SettingsTable!=NULL;
    if (Ret) {
        qlonglong DatabaseVersion=SettingsTable->GetIntValue("Version",0);
        if (DatabaseVersion!=DATABASEVERSION) {
            foreach (cDatabaseTable *Table,Tables) Ret=Ret && Table->DoUpgradeTableVersion(DatabaseVersion);
            SettingsTable->SetIntValue("Version",DATABASEVERSION);
        }
    }
    return Ret;
}

//=====================================================================================================
// Validate each table one by one

bool cDatabase::ValidateTables() {
    bool Ret=db.isOpen();
    foreach (cDatabaseTable *Table,Tables) Ret=Ret && Table->ValidateTable();
    return Ret;
}

//=====================================================================================================

QSqlError cDatabase::LastError() {
    return db.lastError();
}

//**************************************************************************************************************************
// cDatabaseTable : encapsulate a table
//      this class must not been used directly but must be use as ancestor by all table class
//**************************************************************************************************************************

cDatabaseTable::cDatabaseTable(cDatabase *Database) {
    this->Database   =Database;
    NextIndex        =0;
    TypeTable        =TypeTable_Undefined;
}

//=====================================================================================================

cDatabaseTable::~cDatabaseTable() {
}

//=====================================================================================================

bool cDatabaseTable::ValidateTable() {
    QSqlQuery Query(Database->db);
    if (Query.exec(QString("SELECT MAX(%1) FROM %2").arg(IndexKeyName).arg(TableName))) {
        bool Ret=true;
        while (Query.next()) {
            QVariant Value=Query.value(0);
            if (!Value.isNull()) NextIndex=Value.toLongLong(&Ret);
        }
        if (!Ret) DisplayLastSQLError(&Query);
        return Ret;
    } else {
        if (Query.lastError().number()==1) return CreateTable(); else {
            DisplayLastSQLError(&Query);
            return false;
        }
    }
}

//=====================================================================================================

bool cDatabaseTable::CreateTable() {
    if (!CreateTableQuery.isEmpty()) {
        QSqlQuery   Query(Database->db);
        bool        Ret=Query.exec(CreateTableQuery);
        if (!Ret) DisplayLastSQLError(&Query);
        foreach (QString IndexQuery,CreateIndexQuery)
            Ret=Ret && Query.exec(IndexQuery);
        if (!Ret) DisplayLastSQLError(&Query);
        return Ret;
    } else return false;
}

//=====================================================================================================

bool cDatabaseTable::DoUpgradeTableVersion(qlonglong) {
    return true;
}

//**************************************************************************************************************************
// cSettingsTable : encapsulate the settings table
//**************************************************************************************************************************

cSettingsTable::cSettingsTable(cDatabase *Database):cDatabaseTable(Database) {
    TypeTable       =TypeTable_SettingsTable;
    TableName       ="Settings";
    IndexKeyName    ="Key";
    CreateTableQuery="create table Settings ("\
                            "Key                bigint primary key,"\
                            "Name               varchar(100),"\
                            "IntValue           bigint,"\
                            "TextValue          text"
                     ")";
    CreateIndexQuery.append("CREATE INDEX idx_Settings_Key ON Settings (Key)");
    CreateIndexQuery.append("CREATE INDEX idx_Settings_Name ON Settings (Name)");
}

//=====================================================================================================

bool cSettingsTable::CreateTable() {
    return (cDatabaseTable::CreateTable())&&(SetIntValue("Version",DATABASEVERSION));
}

//=====================================================================================================

qlonglong cSettingsTable::GetIntValue(QString SettingName,qlonglong DefaultValue) {
    QSqlQuery   Query(Database->db);
    qlonglong   RetValue=DefaultValue;

    Query.prepare(QString("SELECT IntValue FROM %1 WHERE Name=:Name").arg(TableName));
    Query.bindValue(":Name",SettingName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        QVariant Value=Query.value(0);
        if (!Value.isNull()) {
            bool Ret=false;
            RetValue=Value.toLongLong(&Ret);
            if (!Ret) RetValue=DefaultValue;
        }
    }
    return RetValue;
}

//=====================================================================================================

bool cSettingsTable::SetIntValue(QString SettingName,qlonglong Value) {
    QSqlQuery   Query(Database->db);
    bool        Ret=true;

    Query.prepare(QString("UPDATE %1 set IntValue=:IntValue WHERE Name=:Name").arg(TableName));
    Query.bindValue(":IntValue",Value,      QSql::In);
    Query.bindValue(":Name",    SettingName,QSql::In);
    if ((!Query.exec())||(Query.numRowsAffected()==0)) {
        Query.prepare(QString("INSERT INTO %1 (Key,Name,IntValue) VALUES (:Key,:Name,:IntValue)").arg(TableName));
        Query.bindValue(":Key",     ++NextIndex,QSql::In);
        Query.bindValue(":IntValue",Value,      QSql::In);
        Query.bindValue(":Name",    SettingName,QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query);
    }
    return Ret;
}

//=====================================================================================================

QString cSettingsTable::GetTextValue(QString SettingName,QString DefaultValue) {
    QSqlQuery   Query(Database->db);
    QString     RetValue=DefaultValue;

    Query.prepare(QString("SELECT TextValue FROM %1 WHERE Name=:Name").arg(TableName));
    Query.bindValue(":Name",SettingName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        QVariant Value=Query.value(0);
        if (!Value.isNull()) RetValue=Value.toString();
    }
    return RetValue;
}

//=====================================================================================================

bool cSettingsTable::SetTextValue(QString SettingName,QString Value) {
    QSqlQuery   Query(Database->db);
    bool        Ret=true;

    Query.prepare(QString("UPDATE %1 set TextValue=:IntValue WHERE Name=:Name").arg(TableName));
    Query.bindValue(":TextValue",Value,      QSql::In);
    Query.bindValue(":Name",     SettingName,QSql::In);
    if ((!Query.exec())||(Query.numRowsAffected()==0)) {
        Query.prepare(QString("INSERT INTO %1 (Key,Name,TextValue) VALUES (:Key,:Name,:TextValue)").arg(TableName));
        Query.bindValue(":Key",      ++NextIndex,QSql::In);
        Query.bindValue(":TextValue",Value,      QSql::In);
        Query.bindValue(":Name",     SettingName,QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query);
    }
    return Ret;
}

//=====================================================================================================

bool cSettingsTable::GetIntAndTextValue(QString SettingName,qlonglong *IntValue,QString *TextValue) {
    QSqlQuery   Query(Database->db);
    bool        Ret=false;

    Query.prepare(QString("SELECT IntValue,TextValue FROM %1 WHERE Name=:Name").arg(TableName));
    Query.bindValue(":Name",SettingName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        Ret=true;
        if (Query.value(0).isNull()) Ret=false; else *IntValue =Query.value(0).toLongLong(&Ret);
        if (Query.value(1).isNull()) Ret=false; else *TextValue=Query.value(1).toString();
    }
    return Ret;
}

//=====================================================================================================

bool cSettingsTable::SetIntAndTextValue(QString SettingName,qlonglong IntValue,QString TextValue) {
    bool        Ret=true;
    QSqlQuery   Query(Database->db);

    Query.prepare(QString("UPDATE %1 set TextValue=:TextValue,IntValue=:IntValue WHERE Name=:Name").arg(TableName));
    Query.bindValue(":TextValue",TextValue,  QSql::In);
    Query.bindValue(":IntValue", IntValue,   QSql::In);
    Query.bindValue(":Name",     SettingName,QSql::In);
    if ((!Query.exec())||(Query.numRowsAffected()==0)) {
        Query.prepare(QString("INSERT INTO %1 (Key,Name,IntValue,TextValue) VALUES (:Key,:Name,:IntValue,:TextValue)").arg(TableName));
        Query.bindValue(":Key",      ++NextIndex,QSql::In);
        Query.bindValue(":TextValue",TextValue,  QSql::In);
        Query.bindValue(":IntValue", IntValue,   QSql::In);
        Query.bindValue(":Name",     SettingName,QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query);
    }
    return Ret;
}

//**********************************************************************************************
// cFolderTable : encapsulate folders in the table
//**********************************************************************************************

cFolderTable::cFolderTable(cDatabase *Database):cDatabaseTable(Database) {
    TypeTable       =TypeTable_FolderTable;
    TableName       ="Folders";
    IndexKeyName    ="Key";
    CreateTableQuery="create table Folders ("\
                            "Key                bigint primary key,"\
                            "Name               varchar(512),"\
                            "ParentKey          bigint,"\
                            "Timestamp          bigint"
                     ")";
    CreateIndexQuery.append("CREATE INDEX idx_Folders_Key  ON Folders (Key)");
    CreateIndexQuery.append("CREATE INDEX idx_Folders_Name ON Folders (ParentKey,Name)");
}

//=====================================================================================================
// Get the key associated to a folder path
// If folder not found in the database, then create it and all his parents

qlonglong cFolderTable::GetFolderKey(QString FolderPath) {
    FolderPath=AdjustDirForOS(QDir(FolderPath).absolutePath());
    #ifdef Q_OS_WIN
    // On windows, network share start with \\,  so keep this information in a boolean and remove this "\"
    bool IsNetworkShare=FolderPath.startsWith("\\\\");
    if (IsNetworkShare) FolderPath=FolderPath.mid(QString("\\\\").length());
    #endif
    QStringList FolderList=FolderPath.split(QDir::separator());
    #ifdef Q_OS_WIN
    // On windows, add previously \\ removed before to create the list
    if ((IsNetworkShare)&&(FolderList.count()>0)) FolderList[0]="\\\\"+FolderList[0];
    #endif
    if (FolderList.last().isEmpty()) FolderList.removeLast();
    qlonglong Key=-1;
    QString   ParentPath;   if (FolderList.count()>1) for (int i=0;i<FolderList.count()-1;i++) ParentPath=ParentPath+FolderList[i]+QDir::separator();
    qlonglong ParentKey=(FolderList.count()>1)?GetFolderKey(ParentPath):-1;
    if (FolderList.count()>0) {
        QSqlQuery Query(Database->db);
        bool      Ret=false;
        Query.prepare(QString("SELECT Key FROM %1 WHERE ParentKey=:ParentKey AND Name=:Name").arg(TableName));
        Query.bindValue(":ParentKey",ParentKey,         QSql::In);
        Query.bindValue(":Name",     FolderList.last(), QSql::In);
        if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) if (!Query.value(0).isNull()) {
            Ret=true;
            Key=Query.value(0).toLongLong(&Ret);
        }
        if (!Ret) {
            // Path not found : then add it to the table
            qlonglong Timestamp=QFileInfo(FolderPath).lastModified().toMSecsSinceEpoch();
            Query.prepare(QString("INSERT INTO %1 (Key,Name,ParentKey,Timestamp) VALUES (:Key,:Name,:ParentKey,:Timestamp)").arg(TableName));
            Query.bindValue(":Key",      ++NextIndex,QSql::In);
            Query.bindValue(":Name",     FolderList.last(), QSql::In);
            Query.bindValue(":ParentKey",ParentKey,         QSql::In);
            Query.bindValue(":Timestamp",Timestamp,         QSql::In);
            Ret=Query.exec();
            if (!Ret) DisplayLastSQLError(&Query); else Key=NextIndex;
        }
    }
    return Key;
}

//=====================================================================================================
// Get the path associated to a folder key
// path are always ended with a QDir::separator()

QString cFolderTable::GetFolderPath(qlonglong FolderKey) {
    QSqlQuery   Query(Database->db);
    QString     Path;
    qlonglong   ParentKey=-1;

    Query.prepare(QString("SELECT ParentKey,Name FROM %1 WHERE Key=:Key").arg(TableName));
    Query.bindValue(":Key",FolderKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        if (!Query.value(0).isNull()) {
            bool Ret;
            ParentKey=Query.value(0).toLongLong(&Ret);
            if (!Ret) ParentKey=-1;
        }
        if (!Query.value(1).isNull()) Path=Query.value(1).toString();
    }
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    if (ParentKey!=-1) Path=GetFolderPath(ParentKey)+Path;
    return Path;
}
