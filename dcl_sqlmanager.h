#ifndef DCL_SQLMANAGER_H
#define DCL_SQLMANAGER_H

#include <QObject>
#include <QSqlTableModel>

class DCL_SQLManager
{
public:
    DCL_SQLManager();

    QSqlError newDCL(QString filename);
    QSqlError openDB(QString filename);
    QStringList getBoardList();
    QStringList getModelForBoard(QString board);
    QStringList getSerialForBoard(QString board, QString model);
    QStringList execQueryAndGetResult(QString queryStr);
    QSqlDatabase currentDatabase() const;
    QStringList getListFromQuery(QString query);
    bool execQuery(QString queryStr);
    QString lastError() { return m_lastError; }
    void close();
    bool saveAs(QString filename);

private:
    int m_databasecounter;

    QString m_filename;
    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DCL_SQLMANAGER_H
