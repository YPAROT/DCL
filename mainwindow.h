#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


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

    void on_editTabWidget_currentChanged(int index);

public slots:
    void checkDBStatus();




private:
    Ui::MainWindow *ui;
    QTimer* p_timer_db_valid;
};

#endif // MAINWINDOW_H
