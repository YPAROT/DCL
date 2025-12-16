#ifndef SQLTABLEFORM_H
#define SQLTABLEFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QTimer>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QTableView>


namespace Ui {
class SQLTableForm;
}

class SQLTableForm : public QWidget
{
    Q_OBJECT

public:
    explicit SQLTableForm(QWidget *parent = nullptr);
    ~SQLTableForm();

    void setEditable(bool editable);
    void execQuery(QString queryStr, QSqlDatabase db);
    void showTable(const QString &t, QSqlDatabase db, QString defaultInsertQuery);
    QStringList selectedRowsString(int columnIndex);
    QTableView* sqlTableView();
    void setColumnHidden(int column, bool hide);
    void refreshTable(bool newTable = false);
    QString tableName() {return m_tableName;}

protected slots:
    void resultSectionResized(int logicalIndex, int oldSize, int newSize);

    void lineEditorChanged();
    void lineEditorJustChanged();
    void insertRow();
    void deleteRow();
    void resizeTable();
    void comitAll();
    void revertAll();
    void sortIndicatorChanged(int column, Qt::SortOrder order);

signals:
    void statusMessage(const QString &message);
    void itemDoubleClicked(const QModelIndex &index);
private slots:
    void on_sqlTableView_doubleClicked(const QModelIndex &index);

private:
    Ui::SQLTableForm *ui;

    void updateHeaders();

    QSqlTableModel *m_sqlTableModel;
    QAction *m_insertRowAction;
    QAction *m_deleteRowAction;
    QAction *m_commitAction;
    QAction *m_revertAction;
    QAction *m_resizeTableAction;
    bool m_isEditable;
    QVector<QLineEdit*> m_lineEditors;
    QTimer *m_lineEditorChangeTimer;
    QVector<QSortFilterProxyModel *> m_proxies;

    QString m_defaultInsertQuery;
    QSqlDatabase m_db;
    QString m_tableName;
};


class CustomModel: public QSqlTableModel
{
    Q_OBJECT
public:
    explicit CustomModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase())
        : QSqlTableModel(parent, db) {}

    QVariant data(const QModelIndex &idx, int role) const override
    {
        if (role == Qt::BackgroundRole && isDirty(idx))
            return QBrush(QColor(Qt::yellow));
        return QSqlTableModel::data(idx, role);
    }
};
#endif // SQLTABLEFORM_H
