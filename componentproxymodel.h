#ifndef COMPONENTPROXYMODEL_H
#define COMPONENTPROXYMODEL_H

#include "customsortfilterproxymodel.h"

class ComponentProxyModel : public CustomSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ComponentProxyModel(QObject *parent = nullptr);

protected:
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QStringList m_filters;
};

#endif // COMPONENTPROXYMODEL_H
