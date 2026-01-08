#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include <QTableView>
#include <QAbstractItemModel>


class CustomTableView : public QTableView
{
    Q_OBJECT

public:
    explicit CustomTableView(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model) override;

signals:
    void modelChanged(QAbstractItemModel *newModel);
};

#endif // CUSTOMTABLEVIEW_H
