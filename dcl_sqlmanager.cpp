#include "dcl_sqlmanager.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QtSql>

DCL_SQLManager::DCL_SQLManager()
{
    m_databasecounter = 0;
}

QSqlError DCL_SQLManager::newDCL(QString filename)
{
    QSqlError err;

    if (!filename.isEmpty()) {
        if (m_db.isOpen())
            m_db.close();

        err = openDB(filename);
        if (err.type() == QSqlError::NoError) {
            QFile file(":/files/createTables.sql");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream in(&file);
            QString fileContent = in.readAll();
            file.close();
            QSqlQuery query("", m_db);
            QStringList queries = fileContent.split(";", Qt::SkipEmptyParts);
            for (int i = 0; i < queries.count(); ++i) {
                if (!query.exec(queries.at(i) + ";")) {
                    m_lastError = query.lastError().text();
                    qWarning() << "ERREUR: " << query.lastError().text() << query.lastQuery();
                    query.finish();
                }
            }
        } else {
            m_lastError = err.text();
            qWarning() << "ERREUR: " << m_lastError;
        }
    }
    return err;
}

bool DCL_SQLManager::saveAs(QString filename)
{
    if (m_filename.isEmpty())
        return false;
    if (m_db.isOpen()) {
        close();
        if (QFile::exists(filename)) {
            if (QFile::remove(filename) == false) {
                openDB(m_filename);
                return false;
            }
        }

        if (QFile::copy(m_filename, filename)) {
            QFile::remove(m_filename);
            openDB(filename);
            return m_db.isOpen();
        } else {
            openDB(m_filename);
            return false;
        }
    }
    return false;
}

QSqlError DCL_SQLManager::openDB(QString filename)
{
    QSqlError err;
    m_db = QSqlDatabase::addDatabase("QSQLITE", QString("DCL%1").arg(++m_databasecounter));
    m_db.setDatabaseName(filename);

    if (!m_db.open()) {
        err = m_db.lastError();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("DCL%1").arg(m_databasecounter));
    } else {
        m_filename = filename;
        QFileInfo fileInfo(filename);

        QString backupDirectory = QApplication::applicationDirPath() + "/dbBackups/";
        QDir dir;
        dir.mkpath(backupDirectory);
        QString backupFilename = backupDirectory + fileInfo.completeBaseName()
                                 + QDateTime::currentDateTime().toString("_yyyyMMdd_hhmmss.")
                                 + fileInfo.suffix();
        QFile::copy(filename, backupFilename);
    }

    return err;
}

QStringList DCL_SQLManager::getListFromQuery(QString queryStr)
{
    QStringList list;
    if (m_db.isOpen()) {
        QSqlQuery query("", m_db);
        if (!query.exec(queryStr)) {
            m_lastError = query.lastError().text();
            qWarning() << "ERREUR: " << query.lastError().text() << query.lastQuery();
            query.finish();
        } else if (query.first()) {
            list << query.value(0).toString();
            while (query.next()) {
                list << query.value(0).toString();
            }
            query.finish();
        }
    }
    return list;
}

QStringList DCL_SQLManager::execQueryAndGetResult(QString queryStr)
{
    QStringList list;
    if (m_db.isOpen()) {
        QSqlQuery query("", m_db);
        if (!query.exec(queryStr)) {
            m_lastError = query.lastError().text();
            qWarning() << "ERREUR: " << query.lastError().text() << query.lastQuery();
            query.finish();
        } else if (query.first()) {
            QString entry;
            if (query.record().count() > 0) {
                entry = query.value(0).toString();
                for (int i = 1; i < query.record().count(); ++i)
                    entry = entry + ";" + query.value(i).toString();
                list << entry;
            }
            while (query.next()) {
                if (query.record().count() > 0) {
                    entry = query.value(0).toString();
                    for (int i = 1; i < query.record().count(); ++i)
                        entry = entry + ";" + query.value(i).toString();
                    list << entry;
                }
            }
            query.finish();
        }
    }
    return list;
}

bool DCL_SQLManager::execQuery(QString queryStr)
{
    if (m_db.isOpen()) {
        QSqlQuery query("", m_db);

        if (!query.exec(queryStr)) {
            m_lastError = query.lastError().text();
            qWarning() << "ERREUR: " << query.lastError().text() << query.lastQuery();
            query.finish();
            return false;
        } else {
            query.finish();
            return true;
        }
    }
    return false;
}

void DCL_SQLManager::close()
{
    m_db.commit();
    m_db.close();
}

QSqlDatabase DCL_SQLManager::currentDatabase() const
{
    return m_db;
}

QStringList DCL_SQLManager::getBoardList()
{
    return getListFromQuery("SELECT Board_Name FROM Board GROUP BY Board_Name;");
}

QStringList DCL_SQLManager::getModelForBoard(QString board)
{
    return getListFromQuery("SELECT Model FROM Board WHERE Board_Name=\"" + board + "\";");
}

QStringList DCL_SQLManager::getSerialForBoard(QString board, QString model)
{
    return getListFromQuery("SELECT Board_serial FROM Board WHERE Board_Name=\"" + board
                            + "\" AND Model=\"" + model + "\";");
}
