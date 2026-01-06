#include "dcl_sqlmanager.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QtSql>

DCL_SQLManager::DCL_SQLManager()
{
}

QSqlError DCL_SQLManager::newDCL(QString filename)
{
    QSqlError err;
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    if (!filename.isEmpty()) {

        err = openDB(filename);
        if (err.type() == QSqlError::NoError) {
            QFile file(":/files/createTables.sql");
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qWarning()<<"Erreur d'ouverture du fichier de configuration SQL";
                return err;
            }
            QTextStream in(&file);
            QString fileContent = in.readAll();
            file.close();
            QSqlQuery query("", db);
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
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    if (db.isOpen()) {
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
            return db.isOpen();
        } else {
            openDB(m_filename);
            return false;
        }
    }
    return false;
}

QSqlError DCL_SQLManager::openDB(QString filename)
{
    //La base de donnée est une Base SQLITE.
    //Si on la ferme elle passe sur une base en mémoire
    //Cela fait que db.isOpen() renvoie toujours true tant que la connexion existe

    QSqlError err;
    //m_db = QSqlDatabase::addDatabase("QSQLITE", QString("DCL%1").arg(++m_databasecounter));
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    db.setDatabaseName(filename);

    if (!db.open()) {
        err = db.lastError();
        //m_db = QSqlDatabase();
        //QSqlDatabase::removeDatabase(QString("DCL%1").arg(m_databasecounter));
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
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    if (db.isOpen()) {
        QSqlQuery query("", db);
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
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    QStringList list;
    if (db.isOpen()) {
        QSqlQuery query("", db);
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
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    if (db.isOpen()) {
        QSqlQuery query("", db);

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
    QSqlDatabase db = QSqlDatabase::database(); //utilisation de la connexion par défaut
    db.commit();
    db.close();
}

QSqlDatabase DCL_SQLManager::currentDatabase() const
{
    return QSqlDatabase::database(); //utilisation de la connexion par défaut;
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
