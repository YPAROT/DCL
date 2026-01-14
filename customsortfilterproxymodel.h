#ifndef CUSTOMSORTFILTERPROXYMODEL_H
#define CUSTOMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CustomSortFilterProxyModel(QObject *parent = nullptr);

public slots:

    virtual void setFilters(QStringList filters);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QStringList m_filters;
};

#endif // CUSTOMSORTFILTERPROXYMODEL_H
