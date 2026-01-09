#include "proxydelegate.h"

ProxyDelegate::ProxyDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
        m_proxy = nullptr;
}

void ProxyDelegate::setProxy(QSortFilterProxyModel *proxy)
{
    m_proxy = proxy;
}
