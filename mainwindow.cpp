#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "datedelegate.h"
#include "foreignkeydelegate.h"
#include "ouinondelegate.h"
#include "componentproxymodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Mise en place des objets graphiques
    ui->setupUi(this);

    //Renommage des vues afin de les rendre unique. (Non unicité du fait de la promotion de certains widgets en FilterTableView)
    // qDebug()<<"Non unique donné:"<<setUniqueUIName(ui->vue_edition_composant->getTableView());
    // qDebug()<<"Non unique donné:"<<setUniqueUIName(ui->dclCompleteSqlTableWidget->getTableView());
    setUniqueUINameforObjectType<QTableView>(this);

    //Rendre non éditable les vues de visualisation pures
    ui->dclCompleteSqlTableWidget->getTableView()->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //initialisation du timer vérifiant l'état de la db
    p_timer_db_valid = new QTimer(this);
    connect(p_timer_db_valid,&QTimer::timeout, this,&MainWindow::checkDBStatus);
    p_timer_db_valid->start(1000);

    //création de la factory pour les modèles
    m_factory = new TableModelFactory(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionClose_triggered()
{
    close();
}



void MainWindow::on_actionNew_DCL_triggered()
{

}

void MainWindow::on_actionLoad_DCL_triggered()
{
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnectionName(),false);
    if(!db.isValid())
    {
        qDebug()<<"La connection n'existe pas";
        return;
    }

    QString filename =QFileDialog::getOpenFileName(nullptr,"Selectionner le fichier de base de donnée",QApplication::applicationDirPath(),"*.db");
    if(filename.isEmpty())
        return;

    qDebug()<<"Fichier ouvert: "<<filename;

    //si la db ouverte est la même on ne fait rien
    if (filename == db.databaseName() && db.isOpen())
    {
        qWarning()<<"La Base de donnée est la même que celle en cours";
        return;
    }


    //fermeture de la base de donnée
    if (db.isOpen())
        on_actionFermer_une_DCL_triggered();

    db.setDatabaseName(filename);
    if(!db.open())
    {
        ui->statusbar->showMessage("erreur d'ouverture de la base de donnée",5000);
        return;
    }

    //Création des modèles et rattachement aux vues via la factory
    ComponentProxyModel* proxy;
    //-> Table Composants
    m_factory->createModel("Composants",db);
    //Vue edition
    m_factory->attachView("Composants",ui->vue_edition_composant->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Composants",ui->vue_edition_composant->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->vue_edition_composant,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);
    //delegates
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),9,new ForeignKeyDelegate(this,"Manufacturer","Name","Manuf_ID",db));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),13,new OuiNonDelegate(this));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),14,new ForeignKeyDelegate(this,"Procurement Company","Name","Proc_ID",db));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),15,new OuiNonDelegate(this));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),17,new OuiNonDelegate(this));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),19,new ForeignKeyDelegate(this,"Status","Status","ID",db));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),20,new DateDelegate(this));
    m_factory->setDelegate("Composants",ui->vue_edition_composant->getTableView()->objectName(),22,new ForeignKeyDelegate(this,"Stockage","Location","ID",db));
    //Vue normale
    m_factory->attachView("Composants",ui->dclCompleteSqlTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->dclCompleteSqlTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);
    //delegates
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),9,new ForeignKeyDelegate(this,"Manufacturer","Name","Manuf_ID",db));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),13,new OuiNonDelegate(this));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),14,new ForeignKeyDelegate(this,"Procurement Company","Name","Proc_ID",db));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),15,new OuiNonDelegate(this));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),17,new OuiNonDelegate(this));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),19,new ForeignKeyDelegate(this,"Status","Status","ID",db));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),20,new DateDelegate(this));
    m_factory->setDelegate("Composants",ui->dclCompleteSqlTableWidget->getTableView()->objectName(),22,new ForeignKeyDelegate(this,"Stockage","Location","ID",db));


    //-> Table carte
    m_factory->createModel("Board",db);
    //Vue edition
    m_factory->attachView("Board",ui->editBoardSqlTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Board",ui->editBoardSqlTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->editBoardSqlTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);

    //-> Table Fournisseurs
    m_factory->createModel("Procurement Company",db);
    //Vue edition
    m_factory->attachView("Procurement Company",ui->editCompanySqlTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Procurement Company",ui->editCompanySqlTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->editCompanySqlTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);

    //-> Table Contact Fournisseurs
    m_factory->createModel("Procurement Company Contact",db);
    //Vue edition
    m_factory->attachView("Procurement Company Contact",ui->editContactSqlTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Procurement Company Contact",ui->editContactSqlTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->editContactSqlTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);
    //delegates
    m_factory->setDelegate("Procurement Company Contact",ui->editContactSqlTableWidget->getTableView()->objectName(),5,new ForeignKeyDelegate(this,"Procurement Company","Name","Proc_ID",db));

    //-> Table Fabricants
    m_factory->createModel("Manufacturer",db);
    //Vue edition
    m_factory->attachView("Manufacturer",ui->editManufacturerSqlTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Manufacturer",ui->editManufacturerSqlTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->editManufacturerSqlTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);

    //-> Table Status
    m_factory->createModel("Status",db);
    //Vue edition
    m_factory->attachView("Status",ui->statusSQLTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Status",ui->statusSQLTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->statusSQLTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);

    //-> Table stockage
    m_factory->createModel("Stockage",db);
    //Vue edition
    m_factory->attachView("Stockage",ui->editStockageSqlTableWidget->getTableView());
    proxy = new ComponentProxyModel();
    m_factory->attachSortFilterProxyModel("Stockage",ui->editStockageSqlTableWidget->getTableView()->objectName(),proxy);
    //Connection des filtres au proxy
    connect(ui->editStockageSqlTableWidget,&FilterTableView::filtersChanged,proxy,&ComponentProxyModel::setFilters);


    //select sur toutes les tables contenues dans la factory
    m_factory->selectOnAllModels();

}

void MainWindow::on_actionSave_as_triggered()
{

}



void MainWindow::on_submitButton_clicked()
{

}

void MainWindow::on_clearButton_clicked()
{

}

void MainWindow::on_comboBox_carte_currentIndexChanged(int index)
{

}

void MainWindow::on_comboBox_Model_currentIndexChanged(int index)
{

}

void MainWindow::on_comboBox_Serial_currentIndexChanged(int index)
{

}


void MainWindow::on_groupByDesignationCheckBox_toggled(bool checked)
{

}

void MainWindow::on_importComponentsPushButton_clicked()
{

}

void MainWindow::on_editionImportRadioButton_clicked()
{

}

void MainWindow::on_editionManualRadioButton_clicked()
{

}


void MainWindow::on_editionAddPushButton_clicked()
{

}

void MainWindow::on_comboBox_carte_2_currentIndexChanged(int index)
{

}

void MainWindow::on_comboBox_Model_2_currentIndexChanged(int index)
{

}

void MainWindow::on_comboBox_Serial_2_currentIndexChanged(int index)
{

}

void MainWindow::on_editionValidateColumnsPushButton_clicked()
{

}

void MainWindow::on_selectCSVPushButton_clicked()
{

}

void MainWindow::on_editionCanceledPushButton_clicked()
{

}

void MainWindow::on_editionEndedPushButton_clicked()
{

}


void MainWindow::on_customTableComboBox_currentIndexChanged(int index)
{

}

void MainWindow::on_exportSqlToCsvPushButton_clicked()
{

}

void MainWindow::on_refreshCustomTablePushButton_clicked()
{

}

void MainWindow::on_exportCustomDCLToCsvPushButton_clicked()
{

}

void MainWindow::on_refreshFullTablePushButton_clicked()
{

}

void MainWindow::on_refreshBoardTablePushButton_clicked()
{

}

void MainWindow::on_exportAllToCsvPushButton_clicked()
{

}

void MainWindow::on_exportBoardDCLToCsvPushButton_clicked()
{

}


void MainWindow::on_importTableConfPushButton_clicked()
{


}

void MainWindow::on_exportTableConfPushButton_clicked()
{

}

void MainWindow::on_deletePushButton_clicked()
{

}

void MainWindow::on_editTabWidget_currentChanged(int index)
{

}

void MainWindow::checkDBStatus()
{
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnectionName(),false);
    QString db_open_str="Ouverte";
    if (!db.isOpen())
        db_open_str = "Fermée";

    QString connection_open_str="Ouvert";
    if (!db.isValid())
        connection_open_str = "Fermé";

    QString status = QString("Etat du lien: %1   Base de donnée: %2").arg(connection_open_str,db_open_str);
    ui->statusbar->showMessage(status);
    return;
}

void MainWindow::on_actionFermer_une_DCL_triggered()
{
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnectionName(),false);

    //fermeture de la db
    if(!db.isOpen())
        return;
    db.close();

    m_factory->clearAllModels();
}

QString MainWindow::getUniqueUIName(QWidget *widget) const
{
    {
        QString unique_name = widget->objectName();
        QWidget *current_widget = widget;
        int counter = 0;
        while(current_widget->parentWidget()!=nullptr && counter<3)
        {
            unique_name.prepend(current_widget->parentWidget()->objectName());
            current_widget = current_widget->parentWidget();
            counter++;
        }

        return unique_name;
    }
}

QString MainWindow::setUniqueUIName(QWidget *widget) const
{
    QString unique_UI_name = getUniqueUIName(widget);

    if(unique_UI_name == widget->objectName())
        qWarning()<<"Le nom unique et le nom original de l'objet sont les mêmes. Attention le nom unique ne l'est peut-être pas";

    qDebug()<<"Objet "<<widget->objectName()<<" renommé "<<unique_UI_name;
    widget->setObjectName(unique_UI_name);

    return unique_UI_name;
}


template<typename T>
void MainWindow::setUniqueUINameforObjectType(QWidget *parent)
{
    QList<T*> objet_trouvés = parent->findChildren<T*>();
    for(T* objet : objet_trouvés)
    {
        setUniqueUIName(objet);
    }
}
