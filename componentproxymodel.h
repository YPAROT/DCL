#ifndef COMPONENTPROXYMODEL_H
#define COMPONENTPROXYMODEL_H

#include <QSortFilterProxyModel>

class ComponentProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ComponentProxyModel(QObject *parent = nullptr);

public slots:

    void setFilters(QStringList filters);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QStringList m_filters;
};

#endif // COMPONENTPROXYMODEL_H
