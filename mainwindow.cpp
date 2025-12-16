#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QSqlError>
#include <QMessageBox>
#include <QtSql>
#include <qwidgetsettings.h>
#include <QStandardItemModel>
#include "importcomponentsdialog.h"
#include "exportcsvdialog.h"

#include "qtcsv/reader.h"
#include "qtcsv/writer.h"
#include "qtcsv/stringdata.h"
#include "qtcsv/variantdata.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_dclSQLManager = new DCL_SQLManager();
    m_tableViewManager = new TableViewManager();
    m_tableViewManager->loadConfFromFile(QApplication::applicationDirPath()+"\\defaultConf.ini");
    ui->editTableViewWidget->setTableViewManager(m_tableViewManager);
    connect(ui->editTableViewWidget,SIGNAL(tableViewUpdated()),this,SLOT(tableViewUpdated()));
    ui->customTableComboBox->addItems(m_tableViewManager->tablesNames());
    ui->stackedWidget->setCurrentIndex(0);
    ui->editionSQLLogTextEdit->setVisible(false);
    ui->editionImportRadioButton->setChecked(true);
    ui->mainTabWidget->setCurrentIndex(0);
    ui->dclTabWidget->setCurrentIndex(0);
    ui->editTabWidget->setCurrentIndex(0);

    on_editionImportRadioButton_clicked();

    Etiquette* etiq = new Etiquette(this);
    etiq->show();
    ui->verticalLayout_7->addWidget(etiq);
    //connection signal/slot
    QObject::connect(this,SIGNAL(SendParamToEtiquette(QStringList &, QStringList &, QVector<int> &)),etiq,SLOT(NewParameters(QStringList &, QStringList &, QVector<int> &)));
    QObject::connect(ui->pushButton_print_etq,SIGNAL(clicked(bool)),etiq,SLOT(PrintSelectionPDF()));

    connect(ui->dclCompleteSqlTableWidget, SIGNAL(itemDoubleClicked(const QModelIndex &)),this,SLOT(dclCompleteSqlTableDoubleClicked(const QModelIndex &)));
    connect(ui->dclPerBoardSqlTableWidget, SIGNAL(itemDoubleClicked(const QModelIndex &)),this,SLOT(dclPerBoardSqlTableDoubleClicked(const QModelIndex &)));

    QString filename = QWidgetSettings::value("DCL").toString();
    if (!filename.isEmpty())
    {
        QSqlError err = m_dclSQLManager->openDB(filename);
        if (err.type() == QSqlError::NoError)
        {
            setWindowTitle("DCL Manager "+filename);

            refreshEditTables();
            refreshDCLAll();
            ui->comboBox_carte->clear();
            ui->comboBox_carte->addItems(m_dclSQLManager->getBoardList());
            ui->comboBox_carte_2->clear();
            ui->comboBox_carte_2->addItems(m_dclSQLManager->getBoardList());
        }
    }

    ui->mainTabWidget->removeTab(2); // Etiquettes
    emit statusMessage(tr("Ready."));
}

MainWindow::~MainWindow()
{
    m_dclSQLManager->close();
    delete m_dclSQLManager;
    m_tableViewManager->saveConfToFile(QApplication::applicationDirPath()+"\\defaultConf.ini");
    delete m_tableViewManager;
    delete ui;
}

void MainWindow::on_actionClose_triggered()
{
    m_dclSQLManager->close();
    close();
}

void  MainWindow::tableViewUpdated()
{
    qDebug() << __FUNCTION__ << m_tableViewManager->tablesNames();

    QString currentText = ui->customTableComboBox->currentText();
    int currentIndex = ui->customTableComboBox->currentIndex();
    ui->customTableComboBox->clear();
    qDebug() << __FUNCTION__;
    ui->customTableComboBox->addItem("Customiser les tables DCL");
    qDebug() << __FUNCTION__;
    ui->customTableComboBox->addItems(m_tableViewManager->tablesNames());
    qDebug() << __FUNCTION__;
    if (m_tableViewManager->tablesNames().indexOf(currentText)>=0)
        ui->customTableComboBox->setCurrentIndex(m_tableViewManager->tablesNames().indexOf(currentText));
    else if (currentIndex < ui->customTableComboBox->count())
        ui->customTableComboBox->setCurrentIndex(currentIndex);
    on_customTableComboBox_currentIndexChanged(ui->customTableComboBox->currentIndex());
}

void MainWindow::on_actionNew_DCL_triggered()
{
    QString filename = QFileDialog::getSaveFileName(NULL, "Nom de la base de donnée DCL","","*.db");
    if (!filename.isEmpty())
    {
        if ((!QFile::exists(filename)) || (QMessageBox::information(NULL,"Nouvelle DCL","Le fichier existe deja, la BDD contenue dans le fichier va etre effacée, continuer ?",QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes))
        {
            QSqlError err = m_dclSQLManager->newDCL(filename);
            if (err.type() != QSqlError::NoError)
                QMessageBox::warning(this, tr("Unable to create database"), tr("An error occurred while "
                                                                               "creating the connection: ") + err.text());
            else
            {
                QWidgetSettings::setValue("DCL",filename);
                setWindowTitle("DCL Manager "+filename);
            }
        }
    }

    refreshEditTables();
    refreshDCLAll();
    ui->comboBox_carte->clear();
    ui->comboBox_carte->addItems(m_dclSQLManager->getBoardList());
    ui->comboBox_carte_2->clear();
    ui->comboBox_carte_2->addItems(m_dclSQLManager->getBoardList());
}

void MainWindow::on_actionLoad_DCL_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"Ouvrir une DCL",QApplication::applicationDirPath(),"*.db");
    if (!filename.isEmpty())
    {
        QSqlError err = m_dclSQLManager->openDB(filename);
        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                                                         "opening the connection: ") + err.text());
        else
        {
            QWidgetSettings::setValue("DCL",filename);
            setWindowTitle("DCL Manager "+filename);
        }
    }
    refreshEditTables();
    refreshDCLAll();
    ui->comboBox_carte->clear();
    ui->comboBox_carte->addItems(m_dclSQLManager->getBoardList());
    ui->comboBox_carte_2->clear();
    ui->comboBox_carte_2->addItems(m_dclSQLManager->getBoardList());
}

void MainWindow::on_actionSave_as_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"Enregistrer sous...","","*.db");
    if (!filename.isEmpty())
    {
        if (m_dclSQLManager->saveAs(filename) == false)
            QMessageBox::warning(this,"Enregistrer sous...","Erreur lors du renommage du fichier");
        else
        {
            QWidgetSettings::setValue("DCL",filename);
            setWindowTitle("DCL Manager "+filename);
        }
    }
}

void MainWindow::refreshDCLAll()
{
    QString queryStr = m_tableViewManager->requestForFullDCLTable();
    QRegExp fromRx("FROM\\s+Composants\\s+(\\w+)");
    if (fromRx.indexIn(queryStr)>=0)
    {
        queryStr.replace(QRegExp("^select\\s+",Qt::CaseInsensitive), "select "+fromRx.cap(1)+".ID,");
    }
    else
        queryStr.replace(QRegExp("^select\\s+",Qt::CaseInsensitive), "select Composants.ID,");


    ui->dclCompleteSqlTableWidget->execQuery(queryStr, m_dclSQLManager->currentDatabase());
    ui->dclCompleteSqlTableWidget->setColumnHidden(0,true);
    //On s'occupe aussi de la table étiquette
    ui->tableWidget_Etiq->setUpdatesEnabled(false);
    ui->tableWidget_Etiq->setRowCount(0);
    ui->tableWidget_Etiq->setSortingEnabled(false);

    QStringList result = m_dclSQLManager->execQueryAndGetResult("SELECT Composants.Part_Number FROM Composants");
    ui->tableWidget_Etiq->setRowCount(result.count());
    for (int rowIndex=0; rowIndex < result.count();++rowIndex)
        ui->tableWidget_Etiq->setItem(rowIndex,0,new QTableWidgetItem(result.at(rowIndex)));

    ui->tableWidget_Etiq->resizeColumnsToContents();
    ui->tableWidget_Etiq->resizeRowsToContents();
    ui->tableWidget_Etiq->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_Etiq->setSortingEnabled(true);

    //Selection du premier élément si numrow>0
    //    if(ui->tableWidget_Etiq->rowCount()>0)
    //    {
    //        ui->tableWidget_Etiq->selectRow(0);
    //    }

    ui->tableWidget_Etiq->setUpdatesEnabled(true);
}

void MainWindow::on_submitButton_clicked()
{
    ui->manualSqlTableWidget->execQuery(ui->sqlEdit->toPlainText(), m_dclSQLManager->currentDatabase());
    ui->sqlEdit->setFocus();
}

void MainWindow::on_clearButton_clicked()
{
    ui->sqlEdit->clear();
    ui->sqlEdit->setFocus();
}


void MainWindow::on_comboBox_carte_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->comboBox_Model->clear();
    ui->comboBox_Model->addItems(m_dclSQLManager->getModelForBoard(ui->comboBox_carte->currentText()));
    if (ui->groupByDesignationCheckBox->isChecked())
        ui->deletePushButton->setEnabled(false);
    else if (ui->comboBox_carte->currentText().startsWith("trash",Qt::CaseInsensitive))
        ui->deletePushButton->setEnabled(false);
    else
        ui->deletePushButton->setEnabled(true);
}

void MainWindow::on_comboBox_Model_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->comboBox_Serial->clear();
    ui->comboBox_Serial->addItems(m_dclSQLManager->getSerialForBoard(ui->comboBox_carte->currentText(), ui->comboBox_Model->currentText()));
}

void MainWindow::on_comboBox_Serial_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    if (ui->groupByDesignationCheckBox->isChecked())
        ui->deletePushButton->setEnabled(false);
    else if (ui->comboBox_carte->currentText().startsWith("trash",Qt::CaseInsensitive))
        ui->deletePushButton->setEnabled(false);
    else
        ui->deletePushButton->setEnabled(true);

    QString queryStr;
    queryStr = m_tableViewManager->requestForBoardDCLTable().trimmed();
    queryStr.replace("$BOARD_NAME", ui->comboBox_carte->currentText());
    queryStr.replace("$BOARD_MODEL", ui->comboBox_Model->currentText());
    queryStr.replace("$BOARD_SERIAL", ui->comboBox_Serial->currentText());

    QStringList requests = queryStr.split("##");
    if (ui->groupByDesignationCheckBox->isChecked())
        queryStr = requests.last();
    else
        queryStr = requests.first();


    queryStr.replace(QRegExp("^select\\s+",Qt::CaseInsensitive), "select DCL.ID,");

    QStringList results = m_dclSQLManager->getListFromQuery("SELECT COUNT(*)"
                                                            " FROM DCL"
                                                            " WHERE DCL.Board_ID=(SELECT Board_ID FROM Board WHERE Board_Name=\""+ui->comboBox_carte->currentText()+"\""
                                                            +" AND Model=\""+ui->comboBox_Model->currentText()+"\""
                                                            +" AND Board_serial=\""+ui->comboBox_Serial->currentText()+"\");");
    ui->dclPerBoardSqlTableWidget->execQuery(queryStr, m_dclSQLManager->currentDatabase());
    ui->dclPerBoardSqlTableWidget->setColumnHidden(0,true);
    if (!results.isEmpty())
        ui->nbComponentsLabel->setText(results.first());
}


void MainWindow::refreshEditTables()
{
    ui->editBoardSqlTableWidget->showTable("Board", m_dclSQLManager->currentDatabase(), "INSERT INTO Board (Board_Name) VALUES (\"enter board name\");");
    ui->editContactSqlTableWidget->showTable("Procurement Company Contact", m_dclSQLManager->currentDatabase(), "INSERT INTO \"Procurement Company Contact\" (Name,First_name,Proc_company) VALUES (\"enter name\",\"first name\",(SELECT Min(Proc_ID) from \"Procurement Company\"));");
    ui->editCompanySqlTableWidget->showTable("Procurement Company", m_dclSQLManager->currentDatabase(), "INSERT INTO \"Procurement Company\" (Name) VALUES (\"enter company name\");");
    ui->editComponentsSqlTableWidget->showTable("Composants", m_dclSQLManager->currentDatabase(), "INSERT INTO \"Composants\" (Family, Part_Number, Status_ID) VALUES (\"???\", \"???\", (SELECT Min(ID) from Status));");
    ui->editManufacturerSqlTableWidget->showTable("Manufacturer", m_dclSQLManager->currentDatabase(), "INSERT INTO \"Manufacturer\" (Name) VALUES (\"enter manufacturer name\");");
    ui->editStockageSqlTableWidget->showTable("Stockage", m_dclSQLManager->currentDatabase(), "INSERT INTO \"Stockage\" (Location) VALUES (\"enter new location\");");
    ui->statusSQLTableWidget->showTable("Status", m_dclSQLManager->currentDatabase(), "INSERT INTO \"Status\" (Status) VALUES (\"enter new status\");");
}

void MainWindow::on_groupByDesignationCheckBox_toggled(bool checked)
{
    Q_UNUSED(checked);
    on_comboBox_Serial_currentIndexChanged(ui->comboBox_Serial->currentIndex());
}

void MainWindow::on_importComponentsPushButton_clicked()
{
    QString tableName;
    QList<SQLTableForm *> sqlTableForm = ui->editTabWidget->currentWidget()->findChildren<SQLTableForm *>();
    if (!sqlTableForm.isEmpty())
        tableName = sqlTableForm.first()->tableName();
    else
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Importation","","*.csv");
    if (!filename.isEmpty())
    {
        QList<QStringList> data = QtCSV::Reader::readToList(filename, ";", "\"");
        if (data.isEmpty())
            data = QtCSV::Reader::readToList(filename, ",", "\"");
        if (data.isEmpty())
            QMessageBox::warning(this,"Fichier CSV","Le fichier CSV est vide ou le séparateur n'est pas \",\" ou \";\" ....");
        else
        {
            QSqlRecord rec = m_dclSQLManager->currentDatabase().record(tableName);
            QStringList tableFields;
            for (int i = 0; i < rec.count(); ++i)
            {
                QSqlField fld = rec.field(i);
                tableFields << fld.name();
            }

            if (!tableFields.isEmpty())
            {
                QStringList statusList;
                QStringList procurementCompanyList;

                QString startQuery;
                if (ui->editTabWidget->currentIndex() == 3)
                {
                    startQuery = "INSERT INTO \""+tableName+"\" (Proc_company,";

                    procurementCompanyList = m_dclSQLManager->getListFromQuery("select Name from \"Procurement Company\"");
                    tableFields.removeOne("Proc_company");
                    if (procurementCompanyList.isEmpty())
                    {
                        QMessageBox::warning(this,"Import","Aucun fournisseur n'est défini, ajouter des fournisseurs dans l'onglet édition avant d'importer des composants");
                        return;
                    }

                    tableFields.removeFirst(); // on vire le premier qui est l'ID
                }
                else if (ui->editTabWidget->currentIndex() == 0)
                {
                    startQuery = "INSERT INTO \""+tableName+"\" (Status_ID, Procurement_Company,";
                    tableFields.removeOne("Status_ID");
                    tableFields.removeOne("Procurement_Company");
                    tableFields.removeOne("Stock_location");

                    statusList = m_dclSQLManager->getListFromQuery("select Status from status");
                    if (statusList.isEmpty())
                    {
                        QMessageBox::warning(this,"Import","Aucuns statuts ne sont définis, ajouter des statut dans l'onglet édition avant d'importer des composants");
                        return;
                    }
                    procurementCompanyList = m_dclSQLManager->getListFromQuery("select Name from \"Procurement Company\"");
                    if (procurementCompanyList.isEmpty())
                    {
                        QMessageBox::warning(this,"Import","Aucuns fournisseurs ne sont définis, ajouter des fournisseurs dans l'onglet édition avant d'importer des composants");
                        return;
                    }
                    tableFields.removeFirst(); // on vire le premier qui est l'ID
                }
                else
                {
                    startQuery = "INSERT INTO \""+tableName+"\" (";
                    tableFields.removeFirst(); // on vire le premier qui est l'ID
                }

                ImportComponentsDialog* importComponentsDialog = new ImportComponentsDialog();
                if (importComponentsDialog->exec(tableName,tableFields,data.first(),statusList,procurementCompanyList) == QDialog::Accepted)
                {
                    QList<QStringList> correspondances = importComponentsDialog->correspondances();

                    QList<int> indexes;
                    for (int i=0; i < correspondances.count();++i)
                    {
                        if (!correspondances.at(i).last().isEmpty())
                        {
                            if (i>0)
                                startQuery = startQuery + "," ;
                            startQuery = startQuery + correspondances.at(i).first();
                            indexes << data.first().indexOf(correspondances.at(i).last());
                        }
                    }
                    startQuery = startQuery + ") VALUES (";
                    if (ui->editTabWidget->currentIndex() == 0)
                    {
                        QStringList statusId = m_dclSQLManager->getListFromQuery("select ID from status where Status=\""+importComponentsDialog->status()+"\";");
                        startQuery = startQuery + statusId.first()+",";
                        QStringList procurementCompanyId = m_dclSQLManager->getListFromQuery("select Proc_Id from \"Procurement Company\" where Name=\""+importComponentsDialog->fournisseur()+"\";");
                        startQuery = startQuery + procurementCompanyId.first() +",";
                    }
                    else if (ui->editTabWidget->currentIndex() == 3)
                    {
                        QStringList procurementCompanyId = m_dclSQLManager->getListFromQuery("select Proc_Id from \"Procurement Company\" where Name=\""+importComponentsDialog->fournisseur()+"\";");
                        startQuery = startQuery + procurementCompanyId.first() +",";
                    }

                    QString query;
                    // queries....
                    for (int i= 1; i < data.count();++i)
                    {
                        if (!data.at(i).join("").isEmpty())
                        {
                            query = startQuery;
                            for (int index = 0;index < indexes.count();++index)
                            {
                                if (index > 0)
                                    query = query + ",";
                                query = query + "\"" + data.at(i).at(indexes.at(index)) +  "\"";
                            }
                            query = query + ");";
                            if (m_dclSQLManager->execQuery(query) == false)
                                qDebug() << query << m_dclSQLManager->lastError();
                            qDebug() << query;
                        }
                    }
                    refreshEditTables();
                }
            }
        }
    }
}

void MainWindow::on_editionImportRadioButton_clicked()
{
    if (ui->editionImportRadioButton->isChecked())
        ui->editionStackedWidget->setCurrentIndex(1);
    else
        ui->editionStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_editionManualRadioButton_clicked()
{
    if (ui->editionManualRadioButton->isChecked())
        ui->editionStackedWidget->setCurrentIndex(0);
    else
        ui->editionStackedWidget->setCurrentIndex(1);
}

QString MainWindow::splitRanges(QString ranges)
{
    QStringList topo = ranges.split(",",QString::SkipEmptyParts);
    if (!topo.isEmpty())
    {
        for (int topoIndex = topo.count()-1; topoIndex >=0 ;topoIndex--)
        {
            if (topo.at(topoIndex).contains(".."))
            {
                QStringList items = topo.at(topoIndex).split("..");
                QRegExp rx("(\\S*\\D)(\\d+)$");
                int firstIndex=-1;
                int lastIndex=-1;
                QString firstRefStr;
                QString lastRefStr;
                if (rx.indexIn(items.first()) >= 0)
                {
                    firstIndex = rx.cap(2).toInt();
                    firstRefStr = rx.cap(1);
                    if (rx.indexIn(items.last()) >= 0)
                    {
                        lastIndex = rx.cap(2).toInt();
                        lastRefStr = rx.cap(1);
                    }
                }
                if ((lastIndex >= 0) && (firstIndex >= 0) && (firstRefStr == lastRefStr))
                {
                    topo.removeAt(topoIndex);
                    for (int i = qMax(firstIndex,lastIndex); i >= qMin(firstIndex,lastIndex); i--)
                        topo.insert(topoIndex,rx.cap(1)+QString::number(i));
                }
                else
                {
                    return "";
                }
            }
        }
        //remove duplicates
        for (int i = 0; i < topo.count() - 1;++i)
        {
            for (int j = topo.count() - 1; j > i;j--)
            {
                if (topo.at(i).compare(topo.at(j),Qt::CaseInsensitive) == 0)
                    topo.removeAt(j);
            }
        }
    }
    return topo.join(",");
}

void MainWindow::on_editionAddPushButton_clicked()
{    
    if (!ui->editionTopoLineEdit->text().isEmpty())
    {
        QString topo = splitRanges(ui->editionTopoLineEdit->text());
        if (topo.isEmpty())
        {
            QMessageBox::warning(this,"Saisie repères topo", "Erreur, mauvaise saisie : "+ui->editionTopoLineEdit->text());
            return;
        }
        QStringList componentList = ui->dclEditionSqlTableWidget->selectedRowsString(0);
        if (!componentList.isEmpty())
        {
            QString serial;
            if (!ui->editionSerialLineEdit->text().isEmpty())
                serial = " ("+ui->editionSerialLineEdit->text()+")";
            if (topo.contains(","))
                ui->editionLogTextEdit->append("Ajouter les refs \""+topo+"\" de type "+componentList.first()+serial);
            else
                ui->editionLogTextEdit->append("Ajouter la ref \""+topo+"\" de type "+componentList.first()+serial);
        }
        else
            QMessageBox::warning(this,"Choix du composant", "Erreur, pas composant sélectionné...");
    }
    else
        QMessageBox::warning(this,"Saisie reperes topo", "Erreur, pas de reperes topologiques saisis...");
}

void MainWindow::on_comboBox_carte_2_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->comboBox_Model_2->clear();
    ui->comboBox_Model_2->addItems(m_dclSQLManager->getModelForBoard(ui->comboBox_carte_2->currentText()));
}

void MainWindow::on_comboBox_Model_2_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->comboBox_Serial_2->clear();
    ui->comboBox_Serial_2->addItems(m_dclSQLManager->getSerialForBoard(ui->comboBox_carte_2->currentText(), ui->comboBox_Model_2->currentText()));
}

void MainWindow::on_comboBox_Serial_2_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->dclEditionSqlTableWidget->execQuery( "SELECT Composants.Part_Number,"
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
                                             " INNER JOIN Manufacturer ON Composants.Manuf_ID=Manufacturer.Manuf_ID;"
                                             , m_dclSQLManager->currentDatabase());
    ui->dclEditionSqlTableWidget->setEditable(false);

}


void MainWindow::on_editionValidateColumnsPushButton_clicked()
{
    if (m_csvData.isEmpty())
        QMessageBox::warning(this,"Fichier CSV","Pas de fichier csv selectionné ou fichier CSV vide....");
    else if ((ui->editionRefTopoComboBox->currentText() != ui->editionRefCompoComboBox->currentText())
             && (ui->editionSerialCompoComboBox->currentText() != ui->editionRefTopoComboBox->currentText()))
    {
        ui->editionLogTextEdit->clear();
        QWidgetSettings::setValue("EditRefTopo",ui->editionRefTopoComboBox->currentText());
        QWidgetSettings::setValue("EditRefCompo",ui->editionRefCompoComboBox->currentText());
        QWidgetSettings::setValue("EditSerialCompo",ui->editionSerialCompoComboBox->currentText());
        int indexTopo = m_csvData.first().indexOf(ui->editionRefTopoComboBox->currentText());
        int indexCompo = m_csvData.first().indexOf(ui->editionRefCompoComboBox->currentText());
        int indexSerial = m_csvData.first().indexOf(ui->editionRefCompoComboBox->currentText()) - 1;
        for (int i=1; i < m_csvData.count();++i)
        {
            QString refTopo = m_csvData.at(i).at(indexTopo);
            QString refCompo = m_csvData.at(i).at(indexCompo);
            QString serial;
            if (indexSerial >= 0)
                serial = m_csvData.at(i).at(indexSerial);
            if (!serial.isEmpty())
                serial = " ("+serial+")";

            if (!refCompo.isEmpty() && !refTopo.isEmpty())
            {
                QString topo = splitRanges(refTopo);
                if (topo.isEmpty())
                    ui->editionLogTextEdit->append("Impossible d'ajouter les refs "+refTopo);
                else if (topo.contains(","))
                    ui->editionLogTextEdit->append("Ajouter les refs \""+topo+"\" de type "+refCompo+serial);
                else
                    ui->editionLogTextEdit->append("Ajouter la ref \""+topo+"\" de type "+refCompo+serial);
            }
        }
    }
    else
        QMessageBox::warning(this,"Selection des colonnes","Choisir des colonnes differentes dans le CSV....");
}

void MainWindow::on_selectCSVPushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Importation","","*.csv");
    if (!filename.isEmpty())
    {
        ui->editionCSVFilenameLineEdit->setText(filename);
        m_csvData = QtCSV::Reader::readToList(filename, ";", "\"");
        if (m_csvData.isEmpty())
            m_csvData = QtCSV::Reader::readToList(filename, ",", "\"");
        if (m_csvData.isEmpty())
            QMessageBox::warning(this,"Fichier CSV","Le fichier CSV est vide ou le séparateur n'est pas \",\" ou \";\" ....");
        else
        {
            QStringList header = m_csvData.first();

            header.removeAll("");
            ui->editionRefTopoComboBox->clear();
            ui->editionRefCompoComboBox->clear();
            ui->editionSerialCompoComboBox->clear();
            ui->editionRefTopoComboBox->addItems(header);
            ui->editionRefCompoComboBox->addItems(header);
            ui->editionSerialCompoComboBox->addItem("No serial");
            ui->editionSerialCompoComboBox->addItems(header);
            int index = header.indexOf(QWidgetSettings::value("EditRefTopo","").toString());
            if (index >= 0)
                ui->editionRefTopoComboBox->setCurrentIndex(index);
            index = header.indexOf(QWidgetSettings::value("EditRefCompo","").toString());
            if (index >= 0)
                ui->editionRefCompoComboBox->setCurrentIndex(index);
            index = header.indexOf(QWidgetSettings::value("EditSerialCompo","").toString());
            if (index >= 0)
                ui->editionSerialCompoComboBox->setCurrentIndex(index);
        }
    }
}

void MainWindow::on_editionCanceledPushButton_clicked()
{
    //    if (!ui->editionLogTextEdit->toPlainText().isEmpty())
    //        if (QMessageBox::warning(this,"Nouvelle édition","Attention, l'édition en cours n'a pas été rajouté à la base de données, voulez vous vraiment remettre à zero cette fenetre d'edition ?", QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
    //            return;
    ui->editionLogTextEdit->clear();
    ui->editionSQLLogTextEdit->setVisible(false);

}

void MainWindow::on_editionEndedPushButton_clicked()
{
    ui->editionSQLLogTextEdit->clear();
    ui->editionSQLLogTextEdit->setVisible(true);

    QStringList entries = ui->editionLogTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
    if (!entries.isEmpty())
    {
        QStringList boardIds = m_dclSQLManager->getListFromQuery("SELECT Board_ID from Board WHERE Board_Name=\""+ui->comboBox_carte_2->currentText()+"\""
                                                                 +" AND Model=\""+ui->comboBox_Model_2->currentText()+"\""
                                                                 +" AND Board_serial=\""+ui->comboBox_Serial_2->currentText()+"\";");
        if (boardIds.isEmpty())
        {
            ui->editionSQLLogTextEdit->append("Erreur: Pas de carte selectionné ou impossible de retrouver la carte dans la base de données");
            return;
        }
        QString boardId = boardIds.first();
        QStringList refTopoList = m_dclSQLManager->getListFromQuery("SELECT Ref_Topo"
                                                                    " FROM DCL"
                                                                    " WHERE DCL.Board_ID="+boardId+";");

        for (int i=0; i < entries.count();++i)
        {
            QRegExp rx("\\\"(\\S*)\\\"\\s*de\\s+type\\s+(\\S+)");
            QRegExp serialRx("\\((.+)\\)\\s*$");
            if (rx.indexIn(entries.at(i)) >= 0)
            {
                QStringList topos = rx.cap(1).split(",");
                QStringList IDs = m_dclSQLManager->getListFromQuery("SELECT ID FROM Composants where Part_Number=\""+rx.cap(2)+"\";");
                if (IDs.isEmpty())
                    ui->editionSQLLogTextEdit->append(entries.at(i)+" => NOK, "+rx.cap(2)+" n'existe pas");
                else
                {
                    QString ID = IDs.first();
                    for (int j = 0; j < topos.count();++j)
                    {
                        if (refTopoList.indexOf(topos.at(j))>=0)
                            ui->editionSQLLogTextEdit->append(entries.at(i)+" => NOK, "+topos.at(j)+" existe deja");
                        else
                        {
                            QString queryStr;
                            if (serialRx.indexIn(entries.at(i)) < 0)
                                queryStr = "INSERT into DCL (Composant_ID, Board_ID, Ref_Topo) VALUES ("+ID+","+boardId+",\""+topos.at(j)+"\");";
                            else
                                queryStr = "INSERT into DCL (Composant_ID, Board_ID, Ref_Topo, Serial_Number) VALUES ("+ID+","+boardId+",\""+topos.at(j)+"\",\""+serialRx.cap(1)+"\");";

                            if (m_dclSQLManager->execQuery(queryStr))
                            {
                                refTopoList.append(topos.at(j));
                                ui->editionSQLLogTextEdit->append(entries.at(i)+" => OK pour "+topos.at(j));
                            }
                            else
                                ui->editionSQLLogTextEdit->append(entries.at(i)+" => NOK, erreur mySql lors de l'insertion pour "+topos.at(j) +" ("+m_dclSQLManager->lastError()+")");
                        }
                    }
                }
            }
            else
            {
                ui->editionSQLLogTextEdit->append(entries.at(i)+" => NOK");
            }
        }
        on_comboBox_Serial_currentIndexChanged(ui->comboBox_Serial->currentIndex());
    }
}

void MainWindow::on_lineEdit_projectName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    PrepareSendToEtiquette();
}

void MainWindow::on_spinBox_Etiq_Projet_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    PrepareSendToEtiquette();
}

void MainWindow::on_spinBox_Etiq_param_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    PrepareSendToEtiquette();
}

void MainWindow::on_tableWidget_Etiq_itemSelectionChanged()
{
    PrepareSendToEtiquette();
}

void MainWindow::PrepareSendToEtiquette()
{
    QStringList ParamNameList= {"Projet","Désignation","Fabricant","Fournisseur","Valeur","Tolérance",
                                "Max. rating","Boîtier","Date Code","Qualifié spatial","ITAR/EAS","Relifed","Quantité commandée","Lieu de stockage"};
    QStringList ParamValueList;
    QVector<int> ParamPoliceList;

    int titlepolice = ui->spinBox_Etiq_Projet->value();
    int parampolice = ui->spinBox_Etiq_param->value();

    ParamPoliceList<<titlepolice;
    //ParamPoliceList<<10;//Ajout de l'espace (taille 10) entre titre et le reste (se voit dans la liste de titre aussi)
    for(int i=0;i<14;i++)
    {
        ParamPoliceList<<parampolice;
    }
    ParamValueList<<ui->lineEdit_projectName->text();
    //ParamValueList<<"";//Ajout espace entre titre et le reste (se voit dans la liste de titre aussi)

    //recup du composant sélectionné
    if(ui->tableWidget_Etiq->selectedItems().size()==0)
        return;
    QString PN_str=ui->tableWidget_Etiq->selectedItems().at(0)->text(); //même si il y en a plusieurs on selectionne le premier seulement

    //récupération des valeurs dans la db.
    QString queryStr = "SELECT Composants.Part_Number,"
                       " Manufacturer.Name,"
                       " \"Procurement Company\".Name,"
                       " Composants.Value,"
                       " Composants.Tolerance,"
                       " Composants.Maximum_rating,"
                       " Composants.Package,"
                       " Composants.Date_Code,"
                       " Composants.Space_Qualified,"
                       " Composants.ITAR_EAS,"
                       " Composants.Relifed,"
                       " Composants.Quantity_ordered,"
                       " Stockage.Location"
                       " FROM Composants"
                       " INNER JOIN Manufacturer on Composants.Manuf_ID=Manufacturer.Manuf_ID"
                       " INNER JOIN \"Procurement Company\" ON \"Procurement Company\".Proc_ID=Composants.Procurement_Company"
                       " INNER JOIN Stockage on Composants.Stock_location=Stockage.ID"
                       " WHERE Composants.Part_Number=\""+PN_str+"\";";
    QStringList result = m_dclSQLManager->execQueryAndGetResult(queryStr);
    if (!result.isEmpty())
    {
        QStringList values = result.first().split(";",QString::KeepEmptyParts);

        for(int i=0;i<8;i++)
        {
            ParamValueList<<values.at(i);
        }

        for(int i=8;i<11;i++)
        {
            if(values.at(i) != "0")
            {
                ParamValueList<<"Oui";
            }
            else
            {
                ParamValueList<<"Non";
            }
        }

        for(int i=11;i<values.count();i++)
        {
            ParamValueList<<values.at(i);
        }
    }
    emit SendParamToEtiquette(ParamNameList,ParamValueList,ParamPoliceList);
}


void MainWindow::on_pushButton_etq_all_clicked()
{
    //Il faut imprimer toutes les etiquettes...
    int nb_etiquettes = ui->tableWidget_Etiq->rowCount();
    int nb_pages = nb_etiquettes/12+1; //12 etiquettes par page
    int index_etiquette =0;

    //Selection du fichier de sortie
    QPrinter printer(QPrinter::PrinterResolution);
    QString FileOutput = QFileDialog::getSaveFileName(0,"etiquettes",QString(),"*.pdf");
    printer.setOutputFileName(FileOutput);

    QPainter painter;
    QPen pen;
    QFont font;
    QVector<double> rapport_font;

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::A4);

    double marge_gauche1=4.7; //(en mm sur du A4)
    double marge_gauche2=106.3; //(en mm sur du A4)
    double marge_haut=21.5; //(en mm sur du A4)
    double largeur_etiquette=99.1; //(en mm sur du A4)
    double hauteur_etiquette=42.3; //(en mm sur du A4)

    //transformation en pxl
    QRect taille_page_px = printer.pageRect();
    QRectF taille_page_mm = printer.pageRect(QPrinter::Millimeter);

    int marge_gauche1_pxl = marge_gauche1/taille_page_mm.width()*taille_page_px.width();
    int marge_gauche2_pxl = marge_gauche2/taille_page_mm.width()*taille_page_px.width();
    int marge_haut_pxl = marge_haut/taille_page_mm.height()*taille_page_px.height();
    int largeur_etiquette_pxl = largeur_etiquette/taille_page_mm.width()*taille_page_px.width();
    int hauteur_etiquette_pxl = hauteur_etiquette/taille_page_mm.height()*taille_page_px.height();

    Etiquette_Data donnees_etiquette;

    bool fit_ok = false;
    int police_begin = 10;
    int nb_ligne_etiquette = 0;
    QStringList New_Lignes;
    QVector<int> New_ParametersPolice;
    int y_size_texte = 0;
    QString TempStr;
    QStringList SplitList;

    // Dessin
    pen.setWidth(3);
    //mise en place du painter
    painter.begin(&printer);
    painter.setPen(pen);
    int x_coord=0;
    int y_coord=0;
    for(int page = 0;page<nb_pages;page++)
    {
        if (page>0)
        {printer.newPage();}

        y_coord=marge_haut_pxl;
        for (index_etiquette=0;index_etiquette<12;index_etiquette++)
        {
            //Si on a dépassé le nb d'etiquette on sort
            if(page*12+index_etiquette>nb_etiquettes-1)
            {break;}

            if(index_etiquette%2==0)
            {
                x_coord=marge_gauche1_pxl;
            }
            else
            {
                x_coord=marge_gauche2_pxl;
            }

            //Dessin du contour étiquette
            painter.drawRoundedRect(x_coord,y_coord,largeur_etiquette_pxl,hauteur_etiquette_pxl,5,5);

            //Récupération des données étiquettes
            //qDebug()<<"index etiquette: "<<page*12+index_etiquette;
            donnees_etiquette = GetEtiquetteData(ui->tableWidget_Etiq->item(page*12+index_etiquette,0)->text());

            //Calcul du font size
            //On prends les rapport de taille définis par l'utilisateur avec le titre en référence
            rapport_font.clear();
            for(int i=0;i<donnees_etiquette.ParamPoliceList.size();i++)
            {
                rapport_font << static_cast<double>(donnees_etiquette.ParamPoliceList.at(i))/donnees_etiquette.ParamPoliceList.at(0);
            }

            //on commence à regarder si avec la police définie, l'étiquette est dans le cadre
            fit_ok = false;
            police_begin = donnees_etiquette.ParamPoliceList.at(0);
            nb_ligne_etiquette = donnees_etiquette.ParamNameList.size();
            while(!fit_ok)
            {
                New_Lignes.clear();
                New_ParametersPolice.clear();

                //On taille le texte en x pour que ça rentre
                for(int j=0;j<nb_ligne_etiquette;j++)
                {

                    int font_value = police_begin*rapport_font.at(j);
                    font.setPointSize(font_value);
                    QFontMetrics TempFontSize(font);

                    TempStr=donnees_etiquette.ParamNameList.at(j)+": "+donnees_etiquette.ParamValueList.at(j);

                    if(TempFontSize.width(TempStr)>largeur_etiquette_pxl-20) //10pxl de chaque côté du cadre
                    {
                        //Split en fonction des espaces
                        SplitList=TempStr.split(QRegExp("\\s+")); //blancs enlevés
                        //on ajoute tant que on ne dépasse pas puis on passe à la ligne etc
                        //le premier mot doit y aller
                        TempStr=SplitList.takeFirst();
                        while(SplitList.size()>0)
                        {

                            if(TempFontSize.width(TempStr+" "+SplitList.at(0))<=largeur_etiquette_pxl-20)//10pxl de chaque côté du cadre
                            {
                                TempStr+=" "+SplitList.takeFirst();
                            }
                            else
                            {
                                //ajout dans la liste
                                New_Lignes<<TempStr;
                                New_ParametersPolice<<font_value;
                                //reprise de la concaténation
                                TempStr.clear();
                            }
                        }

                    }
                    else
                    {
                        New_Lignes<<TempStr;
                        New_ParametersPolice<<font_value;
                    }
                }
                //on calcule en y
                y_size_texte = 0;
                for(int j=0;j<New_Lignes.size();j++)
                {
                    font.setPointSize(New_ParametersPolice.at(j));
                    QFontMetrics TempFontSize(font);
                    y_size_texte += TempFontSize.height();
                }

                //si OK => fitOK
                if (y_size_texte<=hauteur_etiquette_pxl)
                {fit_ok=true;}
                //sinon police_begin-1 avec un minimum à 6 (arbitraire)
                else
                {
                    if(police_begin>6)
                    {police_begin-=1;}
                }
            }

            //ecriture texte
            int y_offset=10;
            for(int i=0;i<New_Lignes.size();i++)
            {
                font.setPointSize(New_ParametersPolice.at(i));
                QFontMetrics TempFontSize(font);
                painter.setFont(font);
                painter.drawText(10+x_coord,y_coord+y_offset,New_Lignes.at(i));
                y_offset += TempFontSize.height();
            }

            //preparation coordonnées pour la suivante
            if(index_etiquette%2!=0)
            {y_coord += hauteur_etiquette_pxl;}

            //qDebug()<< "etiquette: " <<index_etiquette<<"position y: "<<y_coord;

        }

    }
    painter.end();
}


Etiquette_Data MainWindow::GetEtiquetteData(QString PN_str)
{
    Etiquette_Data resultat;
    resultat.ParamNameList <<"Projet"<<"Désignation"<<"Fabricant"<<"Fournisseur"<<"Valeur"<<"Tolérance"<<"Max. rating"<<"Boîtier"<<"Date Code"<<"Qualifié spatial"<<"ITAR/EAS"<<"Relifed"<<"Quantité commandée"<<"Lieu de stockage";
    QString queryStr;

    int titlepolice = ui->spinBox_Etiq_Projet->value();
    int parampolice = ui->spinBox_Etiq_param->value();

    resultat.ParamPoliceList<<titlepolice;
    for(int i=0;i<14;i++)
    {
        resultat.ParamPoliceList<<parampolice;
    }
    resultat.ParamValueList<<ui->lineEdit_projectName->text();


    //récupération des valeurs dans la db.
    queryStr = "SELECT Composants.Part_Number,"
               " Manufacturer.Name,"
               " \"Procurement Company\".Name,"
               " Composants.Value,"
               " Composants.Tolerance,"
               " Composants.Maximum_rating,"
               " Composants.Package,"
               " Composants.Date_Code,"
               " Composants.Space_Qualified,"
               " Composants.ITAR_EAS,"
               " Composants.Relifed,"
               " Composants.Quantity_ordered,"
               " Stockage.Location"
               " FROM Composants"
               " INNER JOIN Manufacturer on Composants.Manuf_ID=Manufacturer.Manuf_ID"
               " INNER JOIN \"Procurement Company\" ON \"Procurement Company\".Proc_ID=Composants.Procurement_Company"
               " INNER JOIN Stockage on Composants.Stock_location=Stockage.ID"
               " WHERE Composants.Part_Number=\""+PN_str+"\";";
    QStringList result = m_dclSQLManager->execQueryAndGetResult(queryStr);
    if (!result.isEmpty())
    {

        QStringList values = result.first().split(";",QString::KeepEmptyParts);

        for(int i=0;i<8;i++)
        {
            resultat.ParamValueList<<values.at(i);
        }

        for(int i=8;i<11;i++)
        {
            if(values.at(i) != "0")
            {
                resultat.ParamValueList<<"Oui";
            }
            else
            {
                resultat.ParamValueList<<"Non";
            }
        }

        for(int i=11;i<values.count();i++)
        {
            resultat.ParamValueList<<values.at(i);
        }
    }

    return resultat;
}

void MainWindow::on_pushButton_print_select_etq_clicked()
{
    //Il faut imprimer la selection
    int nb_etiquettes = ui->tableWidget_Etiq->selectedItems().size();
    int nb_pages = nb_etiquettes/12+1; //12 etiquettes par page
    int index_etiquette =0;

    //Selection du fichier de sortie
    QPrinter printer(QPrinter::PrinterResolution);
    QString FileOutput = QFileDialog::getSaveFileName(0,"etiquettes",QString(),"*.pdf");
    printer.setOutputFileName(FileOutput);

    QPainter painter;
    QPen pen;
    QFont font;
    QVector<double> rapport_font;

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::A4);

    double marge_gauche1=4.7; //(en mm sur du A4)
    double marge_gauche2=106.3; //(en mm sur du A4)
    double marge_haut=21.5; //(en mm sur du A4)
    double largeur_etiquette=99.1; //(en mm sur du A4)
    double hauteur_etiquette=42.3; //(en mm sur du A4)

    //transformation en pxl
    QRect taille_page_px = printer.pageRect();
    QRectF taille_page_mm = printer.pageRect(QPrinter::Millimeter);

    int marge_gauche1_pxl = marge_gauche1/taille_page_mm.width()*taille_page_px.width();
    int marge_gauche2_pxl = marge_gauche2/taille_page_mm.width()*taille_page_px.width();
    int marge_haut_pxl = marge_haut/taille_page_mm.height()*taille_page_px.height();
    int largeur_etiquette_pxl = largeur_etiquette/taille_page_mm.width()*taille_page_px.width();
    int hauteur_etiquette_pxl = hauteur_etiquette/taille_page_mm.height()*taille_page_px.height();

    Etiquette_Data donnees_etiquette;

    bool fit_ok = false;
    int police_begin = 10;
    int nb_ligne_etiquette = 0;
    QStringList New_Lignes;
    QVector<int> New_ParametersPolice;
    int y_size_texte = 0;
    QString TempStr;
    QStringList SplitList;

    QStringList printselection;
    for(int i = 0;i<nb_etiquettes;i++)
    {
        printselection<<ui->tableWidget_Etiq->selectedItems().at(i)->text();
    }

    // Dessin
    pen.setWidth(3);
    //mise en place du painter
    painter.begin(&printer);
    painter.setPen(pen);
    int x_coord=0;
    int y_coord=0;
    for(int page = 0;page<nb_pages;page++)
    {
        if (page>0)
        {printer.newPage();}

        y_coord=marge_haut_pxl;
        for (index_etiquette=0;index_etiquette<12;index_etiquette++)
        {
            //Si on a dépassé le nb d'etiquette on sort
            if(page*12+index_etiquette>nb_etiquettes-1)
            {break;}

            if(index_etiquette%2==0)
            {
                x_coord=marge_gauche1_pxl;
            }
            else
            {
                x_coord=marge_gauche2_pxl;
            }

            //Dessin du contour étiquette
            painter.drawRoundedRect(x_coord,y_coord,largeur_etiquette_pxl,hauteur_etiquette_pxl,5,5);

            //Récupération des données étiquettes
            //qDebug()<<"index etiquette: "<<page*12+index_etiquette;
            donnees_etiquette = GetEtiquetteData(printselection.at(page*12+index_etiquette));

            //Calcul du font size
            //On prends les rapport de taille définis par l'utilisateur avec le titre en référence
            rapport_font.clear();
            for(int i=0;i<donnees_etiquette.ParamPoliceList.size();i++)
            {
                rapport_font << static_cast<double>(donnees_etiquette.ParamPoliceList.at(i))/donnees_etiquette.ParamPoliceList.at(0);
            }

            //on commence à regarder si avec la police définie, l'étiquette est dans le cadre
            fit_ok = false;
            police_begin = donnees_etiquette.ParamPoliceList.at(0);
            nb_ligne_etiquette = donnees_etiquette.ParamNameList.size();
            while(!fit_ok)
            {
                New_Lignes.clear();
                New_ParametersPolice.clear();

                //On taille le texte en x pour que ça rentre
                for(int j=0;j<nb_ligne_etiquette;j++)
                {
                    int font_value = police_begin*rapport_font.at(j);
                    font.setPointSize(font_value);
                    QFontMetrics TempFontSize(font);

                    TempStr=donnees_etiquette.ParamNameList.at(j)+": "+donnees_etiquette.ParamValueList.at(j);

                    if(TempFontSize.width(TempStr)>largeur_etiquette_pxl-20) //10pxl de chaque côté du cadre
                    {
                        //Split en fonction des espaces
                        SplitList=TempStr.split(QRegExp("\\s+")); //blancs enlevés
                        //on ajoute tant que on ne dépasse pas puis on passe à la ligne etc
                        //le premier mot doit y aller
                        TempStr=SplitList.takeFirst();
                        while(SplitList.size()>0)
                        {

                            if(TempFontSize.width(TempStr+" "+SplitList.at(0))<=largeur_etiquette_pxl-20)//10pxl de chaque côté du cadre
                            {
                                TempStr+=" "+SplitList.takeFirst();
                            }
                            else
                            {
                                //ajout dans la liste
                                New_Lignes<<TempStr;
                                New_ParametersPolice<<font_value;
                                //reprise de la concaténation
                                TempStr.clear();
                            }
                        }

                    }
                    else
                    {
                        New_Lignes<<TempStr;
                        New_ParametersPolice<<font_value;
                    }
                }
                //on calcule en y
                y_size_texte = 0;
                for(int j=0;j<New_Lignes.size();j++)
                {
                    font.setPointSize(New_ParametersPolice.at(j));
                    QFontMetrics TempFontSize(font);
                    y_size_texte += TempFontSize.height();
                }

                //si OK => fitOK
                if (y_size_texte<=hauteur_etiquette_pxl)
                {fit_ok=true;}
                //sinon police_begin-1 avec un minimum à 6 (arbitraire)
                else
                {
                    if(police_begin>6)
                    {police_begin-=1;}
                }
            }

            //ecriture texte
            int y_offset=10;
            for(int i=0;i<New_Lignes.size();i++)
            {
                font.setPointSize(New_ParametersPolice.at(i));
                QFontMetrics TempFontSize(font);
                painter.setFont(font);
                painter.drawText(10+x_coord,y_coord+y_offset,New_Lignes.at(i));
                y_offset += TempFontSize.height();
            }

            //preparation coordonnées pour la suivante
            if(index_etiquette%2!=0)
            {
                y_coord += hauteur_etiquette_pxl;
            }
            //qDebug()<< "etiquette: " <<index_etiquette<<"position y: "<<y_coord;
        }
    }
    painter.end();
}


void MainWindow::dclPerBoardSqlTableDoubleClicked(const QModelIndex &index)
{
    //Affichage des détails du composant double cliqué.
    QString dclID;
    QTableWidgetItem *tableItem;

    QTableWidget* t = new QTableWidget( this );
    t->setWindowFlags( Qt::Window );
    t->setAttribute( Qt::WA_DeleteOnClose );
    t->setWindowTitle("Détails sur le composant");
    t->setColumnCount(1);
    t->setRowCount(24);
    t->setVerticalHeaderLabels({"Désignation","Famille","Group Code","Description","Valeur","Tolérance","Max. Rating","Boîtier","Spéc. Générale","Spec. Détaillée","Niveau de Qualité","Qualifié Spatial","ITAR/EAS","Date Code","Relifed","Poids","Date de réception","Qté. Commandée","Fabricant","Site web Fab.","Fournisseur","Site web Fourn.","Statut","Pièce de stockage"});
    t->verticalHeader()->setVisible(true);
    t->horizontalHeader()->setVisible(false);

    //Récupération de tous les éléments!!!
    dclID=  ui->dclPerBoardSqlTableWidget->sqlTableView()->model()->data(ui->dclPerBoardSqlTableWidget->sqlTableView()->model()->index(index.row(),0)).toString();
    QString queryStr = "SELECT Composants.Part_Number,"
                       " Composants.Family,"
                       " Composants.Group_code,"
                       " Composants.Description,"
                       " Composants.Value,"
                       " Composants.Tolerance,"
                       " Composants.Maximum_rating,"
                       " Composants.Package,"
                       " Composants.Generic_specification,"
                       " Composants.Detailed_specification,"
                       " Composants.Quality_level,"
                       " Composants.Space_Qualified,"
                       " Composants.ITAR_EAS,"
                       " Composants.Date_Code,"
                       " Composants.Relifed,"
                       " Composants.Weight,"
                       " Composants.Date_reception,"
                       " Composants.Quantity_ordered,"
                       " Manufacturer.Name,"
                       " Manufacturer.Site_Web,"
                       " \"Procurement Company\".Name,"
                       " \"Procurement Company\".Site_web,"
                       " Status.Status,"
                       " Stockage.Location"
                       " FROM"
                       " Composants"
                       " INNER JOIN Manufacturer ON Composants.Manuf_ID=Manufacturer.Manuf_ID"
                       " INNER JOIN \"Procurement Company\" ON \"Procurement Company\".Proc_ID=Composants.Procurement_Company"
                       " INNER JOIN Status ON Status.ID=Composants.Status_ID"
                       " INNER JOIN Stockage ON Stockage.ID=Composants.Stock_location"
                       " WHERE"
                       " Composants.ID=(SELECT Composant_ID FROM DCL WHERE ID=\""+dclID+"\");";

    QStringList result = m_dclSQLManager->execQueryAndGetResult(queryStr);

    // Now the Code from yann ;)
    if(!result.isEmpty())
    {
        QStringList values = result.first().split(";",QString::KeepEmptyParts);

        for(int i=0;i<11;i++)
        {
            tableItem = new QTableWidgetItem(values.at(i));
            t->setItem(i,0,tableItem);
        }

        for(int i=11;i<13;i++)
        {
            if(values.at(i)!= "0")
            {
                tableItem = new QTableWidgetItem("Oui");
            }
            else
            {
                tableItem = new QTableWidgetItem("Non");
            }
            t->setItem(i,0,tableItem);
        }

        tableItem = new QTableWidgetItem(values.at(13));
        t->setItem(13,0,tableItem);

        if(values.at(14)!= "0")
        {
            tableItem = new QTableWidgetItem(values.at(14));
        }
        else
        {
            tableItem = new QTableWidgetItem("Non");
        }
        t->setItem(14,0,tableItem);

        for(int i=15;i<values.count();i++)
        {
            tableItem = new QTableWidgetItem(values.at(i));
            t->setItem(i,0,tableItem);
        }
    }
    t->resize(600,800);
    t->resizeColumnsToContents();
    t->resizeRowsToContents();
    t->horizontalHeader()->setStretchLastSection(true);
    t->show();
}

void MainWindow::dclCompleteSqlTableDoubleClicked(const QModelIndex &index)
{
    //Affichage des détails du composant double cliqué.
    QString composantID;
    QTableWidgetItem *tableItem;

    QTableWidget* t = new QTableWidget( this );
    t->setWindowFlags( Qt::Window );
    t->setAttribute( Qt::WA_DeleteOnClose );
    t->setWindowTitle("Détails sur le composant");
    t->setColumnCount(1);
    t->setRowCount(24);
    t->setVerticalHeaderLabels({"Désignation","Famille","Group Code","Description","Valeur","Tolérance","Max. Rating","Boîtier","Spéc. Générale","Spec. Détaillée","Niveau de Qualité","Qualifié Spatial","ITAR/EAS","Date Code","Relifed","Poids","Date de réception","Qté. Commandée","Fabricant","Site web Fab.","Fournisseur","Site web Fourn.","Statut","Pièce de stockage"});
    t->verticalHeader()->setVisible(true);
    t->horizontalHeader()->setVisible(false);

    //Récupération de tous les éléments!!!
    composantID=  ui->dclCompleteSqlTableWidget->sqlTableView()->model()->data(ui->dclCompleteSqlTableWidget->sqlTableView()->model()->index(index.row(),0)).toString();
    QString queryStr = "SELECT Composants.Part_Number,"
                       " Composants.Family,"
                       " Composants.Group_code,"
                       " Composants.Description,"
                       " Composants.Value,"
                       " Composants.Tolerance,"
                       " Composants.Maximum_rating,"
                       " Composants.Package,"
                       " Composants.Generic_specification,"
                       " Composants.Detailed_specification,"
                       " Composants.Quality_level,"
                       " Composants.Space_Qualified,"
                       " Composants.ITAR_EAS,"
                       " Composants.Date_Code,"
                       " Composants.Relifed,"
                       " Composants.Weight,"
                       " Composants.Date_reception,"
                       " Composants.Quantity_ordered,"
                       " Manufacturer.Name,"
                       " Manufacturer.Site_Web,"
                       " \"Procurement Company\".Name,"
                       " \"Procurement Company\".Site_web,"
                       " Status.Status,"
                       " Stockage.Location"
                       " FROM"
                       " Composants"
                       " INNER JOIN Manufacturer ON Composants.Manuf_ID=Manufacturer.Manuf_ID"
                       " INNER JOIN \"Procurement Company\" ON \"Procurement Company\".Proc_ID=Composants.Procurement_Company"
                       " INNER JOIN Status ON Status.ID=Composants.Status_ID"
                       " INNER JOIN Stockage ON Stockage.ID=Composants.Stock_location"
                       " WHERE"
                       " Composants.ID=\""+composantID+"\";";

    QStringList result = m_dclSQLManager->execQueryAndGetResult(queryStr);

    // Now the Code from yann ;)
    if(!result.isEmpty())
    {
        QStringList values = result.first().split(";",QString::KeepEmptyParts);

        for(int i=0;i<11;i++)
        {
            tableItem = new QTableWidgetItem(values.at(i));
            t->setItem(i,0,tableItem);
        }

        for(int i=11;i<13;i++)
        {
            if(values.at(i)!= "0")
            {
                tableItem = new QTableWidgetItem("Oui");
            }
            else
            {
                tableItem = new QTableWidgetItem("Non");
            }
            t->setItem(i,0,tableItem);
        }

        tableItem = new QTableWidgetItem(values.at(13));
        t->setItem(13,0,tableItem);

        if(values.at(14)!= "0")
        {
            tableItem = new QTableWidgetItem(values.at(14));
        }
        else
        {
            tableItem = new QTableWidgetItem("Non");
        }
        t->setItem(14,0,tableItem);

        for(int i=15;i<values.count();i++)
        {
            tableItem = new QTableWidgetItem(values.at(i));
            t->setItem(i,0,tableItem);
        }
    }
    t->resize(600,800);
    t->resizeColumnsToContents();
    t->resizeRowsToContents();
    t->horizontalHeader()->setStretchLastSection(true);
    t->show();
}

void MainWindow::on_customTableComboBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    if (index == 0)
        ui->stackedWidget->setCurrentIndex(0);
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
        QString queryStr = m_tableViewManager->requestForTableIndex(ui->customTableComboBox->currentIndex() - 1).trimmed();
        queryStr.replace("$BOARD_NAME", ui->comboBox_carte->currentText());
        queryStr.replace("$BOARD_MODEL", ui->comboBox_Model->currentText());
        queryStr.replace("$BOARD_SERIAL", ui->comboBox_Serial->currentText());
        ui->customSqlTableWidget->execQuery(queryStr, m_dclSQLManager->currentDatabase());
    }
}

void MainWindow::on_exportSqlToCsvPushButton_clicked()
{
    exportToCsv(ui->manualSqlTableWidget->sqlTableView(),"custom");
}

void MainWindow::on_refreshCustomTablePushButton_clicked()
{
    ui->customSqlTableWidget->execQuery(m_tableViewManager->requestForTableIndex(ui->customTableComboBox->currentIndex() - 1), m_dclSQLManager->currentDatabase());
}

void MainWindow::on_exportCustomDCLToCsvPushButton_clicked()
{
    exportToCsv(ui->customSqlTableWidget->sqlTableView(),ui->customTableComboBox->currentText());
}

void MainWindow::on_refreshFullTablePushButton_clicked()
{
    refreshDCLAll();
}

void MainWindow::on_refreshBoardTablePushButton_clicked()
{
    ui->comboBox_carte->clear();
    ui->comboBox_carte->addItems(m_dclSQLManager->getBoardList());
    ui->comboBox_carte_2->clear();
    ui->comboBox_carte_2->addItems(m_dclSQLManager->getBoardList());
    on_comboBox_Serial_currentIndexChanged(ui->comboBox_Serial->currentIndex());
}

void MainWindow::on_exportAllToCsvPushButton_clicked()
{
    exportToCsv(ui->dclCompleteSqlTableWidget->sqlTableView(),"FullDCL");
}

void MainWindow::on_exportBoardDCLToCsvPushButton_clicked()
{
    exportToCsv(ui->dclPerBoardSqlTableWidget->sqlTableView(),"boardDCL");
}

void MainWindow::exportToCsv(QTableView *sqlTable,QString tableName)
{
    exportCsvDialog* _exportCsvDialog = new exportCsvDialog();
    QStringList headers;
    for(int i = 0; i < sqlTable->model()->columnCount(); i++)
    {
        headers.append(sqlTable->model()->headerData(i, Qt::Horizontal).toString());
    }
    if (_exportCsvDialog->exec(headers,tableName) == QDialog::Accepted)
    {
        QList<int> columnIndexes = _exportCsvDialog->columnIndexes();
        if (columnIndexes.count() > 0)
        {
            QString filename = QFileDialog::getSaveFileName(this,"Enregistrer sous...","","*.csv");
            if (!filename.isEmpty())
            {
                QStringList strList;
                QtCSV::StringData strData;
                strData.addRow(_exportCsvDialog->columnNames());
                for (int row=0; row < sqlTable->model()->rowCount();++row)
                {
                    strList.clear();
                    for (int column = 0; column < columnIndexes.count();++column)
                        strList <<  sqlTable->model()->index(row,columnIndexes.at(column)).data().toString();
                    strData.addRow(strList);
                }
                if (QtCSV::Writer::write(filename,strData,";") == false)
                    QMessageBox::warning(this,"Enregistrement en csv","Erreur lors de l'enregistrement du csv...");
            }
        }
    }
}

void MainWindow::on_importTableConfPushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Tables configuration file",QApplication::applicationDirPath(),"*.ini;*.txt");
    if (!filename.isEmpty())
    {
        m_tableViewManager->loadConfFromFile(filename);
        ui->editTableViewWidget->setTableViewManager(m_tableViewManager);
    }
}

void MainWindow::on_exportTableConfPushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,"Tables configuration file",QApplication::applicationDirPath(),"*.ini;*.txt");
    if (!filename.isEmpty())
        m_tableViewManager->saveConfToFile(filename);
}

void MainWindow::on_deletePushButton_clicked()
{
    QStringList boardIds = m_dclSQLManager->getListFromQuery("SELECT Board_ID from Board WHERE Board_Name=\"Trash "+ui->comboBox_carte->currentText()+"\""
                                                             +" AND Model=\""+ui->comboBox_Model->currentText()+"\""
                                                             +" AND Board_serial=\""+ui->comboBox_Serial->currentText()+"\";");
    if (boardIds.isEmpty())
    {
        qDebug() << "creation de la trash";
        m_dclSQLManager->execQuery("INSERT INTO Board (Board_Name,Model,Board_serial) VALUES (\"Trash "+ui->comboBox_carte->currentText()+"\", \""+ui->comboBox_Model->currentText()+"\", \""+ui->comboBox_Serial->currentText()+"\");");
        QString trashTable = "Trash "+ui->comboBox_carte->currentText();
        if (ui->comboBox_carte->findText(trashTable) == -1)
        {
            ui->comboBox_carte->addItem(trashTable);
            ui->comboBox_carte_2->addItem(trashTable);
        }
        ui->editBoardSqlTableWidget->refreshTable(false);
    }

    boardIds = m_dclSQLManager->getListFromQuery("SELECT Board_ID from Board WHERE Board_Name=\"Trash "+ui->comboBox_carte->currentText()+"\""
                                                 +" AND Model=\""+ui->comboBox_Model->currentText()+"\""
                                                 +" AND Board_serial=\""+ui->comboBox_Serial->currentText()+"\";");
    qDebug() << "trash id" << boardIds;
    if (!boardIds.isEmpty())
    {
        QModelIndexList currentSelection = ui->dclPerBoardSqlTableWidget->sqlTableView()->selectionModel()->selectedIndexes();
        for (int i = 0; i < currentSelection.count(); ++i) {
            if (currentSelection.at(i).column() != 0)
                continue;

            m_dclSQLManager->execQuery("UPDATE DCL SET Board_ID =\""+boardIds.first()+"\" WHERE ID=\""+currentSelection.at(i).data().toString()+"\";");
        }
    }
    on_comboBox_Serial_currentIndexChanged(0);
}

void MainWindow::on_editTabWidget_currentChanged(int index)
{
  //  ui->importComponentsPushButton->setEnabled((index == 0) || (index == 2) || (index == 4));
}
