#ifndef FILTERTABLEVIEW_H
#define FILTERTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QList>
#include <QLineEdit>
#include <QTimer>

namespace Ui {
class FilterTableView;
}

class FilterTableView : public QWidget
{
    Q_OBJECT

public:
    explicit FilterTableView(QWidget *parent = nullptr);
    ~FilterTableView();

    QTableView* getTableView() const;


signals:

    void filtersChanged(QStringList filters);


protected slots:

    void oneFilterJustChanged();
    void filterRefreshTickTrigger();
    void sqlModelChanged(QAbstractItemModel* model);
    void filterTableHeaderSectionResized(int logicalIndex, int oldSize, int newSize);


private:
    Ui::FilterTableView *ui;
    QList<QLineEdit*> m_filters_edit;
    QTimer* m_filter_refresh_timer;
    QAction* m_insert_row_action;
    QAction* m_delete_row_action;
    QAction* m_commit_action;
    QAction* m_refresh_action;
};

#endif // FILTERTABLEVIEW_H
