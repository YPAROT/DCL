#ifndef FILTERTABLEVIEW_H
#define FILTERTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QList>
#include <QLineEdit>

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


protected slots:

    void sqlModelChanged(QAbstractItemModel* model);

private:
    Ui::FilterTableView *ui;
    QList<QLineEdit*> m_filters_edit;
};

#endif // FILTERTABLEVIEW_H
