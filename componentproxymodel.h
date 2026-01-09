#ifndef COMPONENTPROXYMODEL_H
#define COMPONENTPROXYMODEL_H

#include <QSortFilterProxyModel>

class ComponentProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ComponentProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // COMPONENTPROXYMODEL_H
