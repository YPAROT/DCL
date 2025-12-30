#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include "dcl_sqlmanager.h"
#include "etiquette.h"

#include "tableviewmanager.h"

QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QSqlError)
QT_FORWARD_DECLARE_CLASS(QTableWidgetItem)

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct Etiquette_Data
{
    QStringList ParamNameList;
    QStringList ParamValueList;
    QVector<int> ParamPoliceList;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Etiquette_Data GetEtiquetteData(QString PN_str);

    void exportToCsv(QTableView *sqlTable, QString tableName);

private slots:

    void on_actionClose_triggered();

    void on_actionNew_DCL_triggered();

    void on_actionLoad_DCL_triggered();

    void on_comboBox_carte_currentIndexChanged(int index);

    void on_comboBox_Model_currentIndexChanged(int index);

    void on_comboBox_Serial_currentIndexChanged(int index);

    void on_comboBox_carte_2_currentIndexChanged(int index);

    void on_comboBox_Model_2_currentIndexChanged(int index);

    void on_comboBox_Serial_2_currentIndexChanged(int index);

    void on_submitButton_clicked();

    void on_clearButton_clicked();

    void on_groupByDesignationCheckBox_toggled(bool checked);

    void on_importComponentsPushButton_clicked();

    void on_editionImportRadioButton_clicked();

    void on_editionManualRadioButton_clicked();

    void on_editionEndedPushButton_clicked();

    void on_editionAddPushButton_clicked();

    void on_editionValidateColumnsPushButton_clicked();

    void on_selectCSVPushButton_clicked();

    void on_editionCanceledPushButton_clicked();

    void on_actionSave_as_triggered();

    void on_lineEdit_projectName_textChanged(const QString &arg1);

    void PrepareSendToEtiquette(void);

    void on_spinBox_Etiq_Projet_valueChanged(int arg1);

    void on_spinBox_Etiq_param_valueChanged(int arg1);

    void on_tableWidget_Etiq_itemSelectionChanged();

    void on_pushButton_etq_all_clicked();

    void on_pushButton_print_select_etq_clicked();

    void dclCompleteSqlTableDoubleClicked(const QModelIndex &index);
    void dclPerBoardSqlTableDoubleClicked(const QModelIndex &index);

    void on_exportAllToCsvPushButton_clicked();

    void on_exportBoardDCLToCsvPushButton_clicked();

    void on_customTableComboBox_currentIndexChanged(int index);

    void on_exportSqlToCsvPushButton_clicked();

    void on_refreshCustomTablePushButton_clicked();

    void on_exportCustomDCLToCsvPushButton_clicked();

    void on_refreshFullTablePushButton_clicked();

    void on_refreshBoardTablePushButton_clicked();

    void on_importTableConfPushButton_clicked();

    void on_exportTableConfPushButton_clicked();

    void on_deletePushButton_clicked();

    void tableViewUpdated();
    void on_editTabWidget_currentChanged(int index);

signals:
    void SendParamToEtiquette(QStringList &Name, QStringList &Value, QVector<int> &Police);

    void statusMessage(const QString &message);

private:
    Ui::MainWindow *ui;

    void refreshDCLAll();
    void refreshEditTables();
    QString splitRanges(QString ranges);

    DCL_SQLManager *m_dclSQLManager;
    QList<QStringList> m_csvData;
    TableViewManager *m_tableViewManager;
};

#endif // MAINWINDOW_H
