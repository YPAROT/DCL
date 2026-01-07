#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialisation du timer vérifiant l'état de la db
    p_timer_db_valid = new QTimer(this);
    connect(p_timer_db_valid,&QTimer::timeout, this,&MainWindow::checkDBStatus);
    p_timer_db_valid->start(1000);

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
