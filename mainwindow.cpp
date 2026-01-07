#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

void MainWindow::on_lineEdit_projectName_textChanged(const QString &arg1)
{

}

void MainWindow::on_spinBox_Etiq_Projet_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_Etiq_param_valueChanged(int arg1)
{

}

void MainWindow::on_tableWidget_Etiq_itemSelectionChanged()
{

}

void MainWindow::on_pushButton_etq_all_clicked()
{

}



void MainWindow::on_pushButton_print_select_etq_clicked()
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
