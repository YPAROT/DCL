#include "tableviewmanager.h"
#include <QSettings>
#include <QApplication>
#define FULL_DCL_INDEX  0
#define BOARD_DCL_INDEX  1

TableViewManager::TableViewManager()
{
    m_customTablesList.append(QStringList() << "DCL complète" << defaultRequestForFullDCLTable());
    m_customTablesList.append(QStringList() << "DCL par carte" << defaultRequestForBoardDCLTable());
}

QString TableViewManager::defaultRequestForFullDCLTable()
{
    return "SELECT Composants.Part_Number,"
           "Composants.Family,"
           "Composants.Group_code,"
           "Composants.Description,"
           "Composants.Value,"
           "Composants.Tolerance,"
           "Composants.Maximum_rating,"
           "Composants.Package,"
           "Composants.Quality_level,"
           "Composants.Date_Code,"
           "Composants.ITAR_EAS,"
           "Manufacturer.Name"
           " FROM Composants"
           " INNER JOIN Manufacturer ON Composants.Manuf_ID=Manufacturer.Manuf_ID;";
}

QString TableViewManager::defaultRequestForBoardDCLTable()
{
    return "SELECT DCL.Ref_Topo,"
           "DCL.Serial_Number,"
           "Composants.Part_Number,"
           "Composants.Family,"
           "Composants.Group_code,"
           "Composants.Description,"
           "Composants.Value,"
           "Composants.Tolerance,"
           "Composants.Maximum_rating,"
           "Composants.Package,"
           "Composants.Quality_level,"
           "Composants.Date_Code,"
           "Composants.Relifed,"
           "Composants.ITAR_EAS,"
           "Manufacturer.Name"
           " FROM DCL"
           " INNER JOIN Composants ON Composants.ID=DCL.Composant_ID"
           " INNER JOIN Manufacturer ON Composants.Manuf_ID=Manufacturer.Manuf_ID"
           " WHERE DCL.Board_ID=(SELECT Board_ID FROM Board WHERE Board_Name=\"$BOARD_NAME\""
           " AND Model=\"$BOARD_MODEL\""
           " AND Board_serial=\"$BOARD_SERIAL\");"
           "##SELECT Composants.Part_Number, GROUP_CONCAT(DCL.Ref_Topo) AS \"Ref_Topo\", DCL.Serial_Number,Composants.Family,Composants.Group_code,Composants.Description,Composants.Value,Composants.Tolerance,Composants.Maximum_rating,Composants.Package,Composants.Quality_level,Composants.Date_Code,Composants.Relifed,Composants.ITAR_EAS,Manufacturer.Name FROM DCL INNER JOIN Composants ON Composants.ID=DCL.Composant_ID INNER JOIN Manufacturer ON Composants.Manuf_ID=Manufacturer.Manuf_ID WHERE DCL.Board_ID=(SELECT Board_ID FROM Board WHERE Board_Name=\"$BOARD_NAME\" AND Model=\"$BOARD_MODEL\" AND Board_serial=\"$BOARD_SERIAL\") GROUP BY Composants.Part_Number;";
}


QString TableViewManager::requestForFullDCLTable()
{
    return requestForTableIndex(FULL_DCL_INDEX);
}

QString TableViewManager::requestForBoardDCLTable()
{
    return requestForTableIndex(BOARD_DCL_INDEX);
}

QString TableViewManager::requestForTableIndex(int tableIndex)
{
    if (tableIndex < m_customTablesList.count())
        return m_customTablesList.at(tableIndex).last();
    else
        return "";
}

QString TableViewManager::nameForTableIndex(int tableIndex)
{
    if (tableIndex < m_customTablesList.count())
        return m_customTablesList.at(tableIndex).first();
    else
        return "";
}

bool TableViewManager::setNameAndRequestForTableIndex(int tableIndex,QString newTableName,QString request)
{
    if (tableIndex < m_customTablesList.count())
   {
        m_customTablesList.removeAt(tableIndex);
        m_customTablesList.insert(tableIndex,QStringList() << newTableName << request);
        return true;
    }
    else
        return false;
}

bool TableViewManager::addNewTable(QString tableName,QString request)
{
    m_customTablesList.append(QStringList() << tableName << request);
    return true;
}


bool TableViewManager::loadConfFromFile(QString filename)
{
    if (QFile::exists(filename))
    {
        QSettings settings(filename,QSettings::IniFormat);

        m_customTablesList.clear();
        m_customTablesList.append(QStringList() << "DCL complète" << settings.value("fullDCLRequest", defaultRequestForFullDCLTable()).toString());
        m_customTablesList.append(QStringList() << "DCL par carte" << settings.value("boardDCLRequest", defaultRequestForBoardDCLTable()).toString());

        int count = settings.beginReadArray("tables");
        for (int index = 0; index < count;++index)
        {
            settings.setArrayIndex(index);
            m_customTablesList.append(
                        QStringList() << settings.value("name","Table "+QString::number(index)).toString()
                        << settings.value("request").toString());
        }
        settings.endArray();
        return true;
    }
    else
        return false;
}

void TableViewManager::saveConfToFile(QString filename)
{
    QSettings settings(filename,QSettings::IniFormat);
    settings.setValue("fullDCLRequest", requestForFullDCLTable());
    settings.setValue("boardDCLRequest", requestForBoardDCLTable());

    settings.beginWriteArray("tables",m_customTablesList.count() - 2);
    for (int index = 2; index < m_customTablesList.count();++index)
    {
        settings.setArrayIndex(index - 2);
        settings.setValue("name",m_customTablesList.at(index).first());
        settings.setValue("request",m_customTablesList.at(index).last());
    }
    settings.endArray();
}

QStringList TableViewManager::tablesNames()
{
    QStringList names;
    for (int index = 0; index< m_customTablesList.count();++index)
        names << m_customTablesList.at(index).first();

    return names;
}

bool TableViewManager::removeTableIndex(int tableIndex)
{
    if (tableIndex < m_customTablesList.count())
    {
        m_customTablesList.removeAt(tableIndex);
        return true;
    }
    else
        return false;
}
