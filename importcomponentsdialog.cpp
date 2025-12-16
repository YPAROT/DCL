#include "ImportComponentsDialog.h"
#include "ui_ImportComponentsDialog.h"
#include <QComboBox>
#include <QDebug>
#include <qwidgetsettings.h>

ImportComponentsDialog::ImportComponentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportComponentsDialog)
{
    ui->setupUi(this);
}

ImportComponentsDialog::~ImportComponentsDialog()
{
    delete ui;
}


int ImportComponentsDialog::exec(QString tableName, QStringList tableHeaders, QStringList csvHeaders, QStringList statusList
                                 ,QStringList procurementCompanyList)
{
    csvHeaders.removeAll(QString(""));
    csvHeaders.insert(0,"Pas de correspondance");
//    tableHeaders.removeAll("Status_ID");
//    tableHeaders.removeAll("Procurement_Company");
//    tableHeaders.removeAll("Stock_location");
    ui->correspondanceTableWidget->clear();
    ui->correspondanceTableWidget->setRowCount(tableHeaders.count());
    ui->correspondanceTableWidget->setColumnCount(2);

    ui->fournisseurComboBox->setEnabled(!procurementCompanyList.isEmpty());
    ui->statusComboBox->setEnabled(!statusList.isEmpty());

    ui->fournisseurComboBox->clear();
    ui->statusComboBox->clear();

    ui->fournisseurComboBox->addItems(procurementCompanyList);
    ui->statusComboBox->addItems(statusList);

    for (int row = 0; row < tableHeaders.count(); ++row)
    {
        QTableWidgetItem* tableItem = new QTableWidgetItem(tableHeaders.at(row));
        ui->correspondanceTableWidget->setItem(row,0,tableItem);

        QComboBox *comboBox = new QComboBox();
        comboBox->addItems(csvHeaders);
        int index = csvHeaders.indexOf(QWidgetSettings::value(tableName+"_"+tableItem->text(),"").toString());
        if (index >= 0)
            comboBox->setCurrentIndex(index);

        ui->correspondanceTableWidget->setCellWidget(row,1,comboBox);
    }

    return QDialog::exec();
}

void ImportComponentsDialog::accept()
{
    for (int row = 0; row < ui->correspondanceTableWidget->rowCount(); ++row)
    {
        QComboBox *comboBox= qobject_cast<QComboBox *>(ui->correspondanceTableWidget->cellWidget(row,1));
        if (comboBox)
        {
            if (comboBox->currentIndex() > 0)
            {
                QWidgetSettings::setValue("Comp_"+ui->correspondanceTableWidget->item(row,0)->text(),comboBox->currentText());
            }
        }
    }

    ui->correspondanceTableWidget->resizeRowsToContents();
    ui->correspondanceTableWidget->resizeColumnsToContents();
    ui->correspondanceTableWidget->horizontalHeader()->stretchLastSection();
    QDialog::accept();
}

QString ImportComponentsDialog::status()
{
    return ui->statusComboBox->currentText();
}

QString ImportComponentsDialog::fournisseur()
{
    return ui->fournisseurComboBox->currentText();
}

QList<QStringList> ImportComponentsDialog::correspondances()
{
    QList<QStringList> _correspondances;
    for (int row = 0; row < ui->correspondanceTableWidget->rowCount(); ++row)
    {
        QStringList correspondance;
        correspondance << ui->correspondanceTableWidget->item(row,0)->text();
        QComboBox *comboBox= qobject_cast<QComboBox *>(ui->correspondanceTableWidget->cellWidget(row,1));
        if (comboBox)
        {
            if (comboBox->currentIndex() > 0)
                correspondance << comboBox->currentText();
            else
                correspondance << "";
        }
        else
            correspondance << "";

        _correspondances << correspondance;
    }
    return _correspondances;
}
