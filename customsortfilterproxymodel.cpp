#include "customsortfilterproxymodel.h"

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void CustomSortFilterProxyModel::setFilters(QStringList filters)
{
    beginFilterChange();
    m_filters = filters;
    endFilterChange();
}

bool CustomSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filters.isEmpty()) return true;  // Aucun filtre = tout accepter

    for (int i = 0; i < m_filters.size(); ++i) {
        if (m_filters[i].isEmpty()) continue;  // Ignorer les filtres vides

        QModelIndex index = sourceModel()->index(source_row, i, source_parent);
        QString data = sourceModel()->data(index).toString();

        if (!data.contains(m_filters[i], Qt::CaseInsensitive)) {
            return false;  // La ligne ne correspond pas à ce filtre
        }
    }
    return true;  // La ligne correspond à tous les filtres
}
