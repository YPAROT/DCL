#ifndef IMPORTCOMPONENTSDIALOG_H
#define IMPORTCOMPONENTSDIALOG_H

#include <QDialog>

namespace Ui {
class ImportComponentsDialog;
}

class ImportComponentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportComponentsDialog(QWidget *parent = nullptr);
    ~ImportComponentsDialog();
    int exec(QString tableName, QStringList tableHeaders, QStringList csvHeaders, QStringList statusList, QStringList procurementCompanyList);
    QString status();
    QString fournisseur();
    QList<QStringList> correspondances();

public slots:
    void accept();

private:
    Ui::ImportComponentsDialog *ui;
};

#endif // IMPORTCOMPONENTSDIALOG_H
