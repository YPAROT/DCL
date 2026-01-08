#ifndef FILTERTABLEVIEW_H
#define FILTERTABLEVIEW_H

#include <QWidget>
#include <QTableView>

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
};

#endif // FILTERTABLEVIEW_H
