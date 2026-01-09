#ifndef PROXYDELEGATE_H
#define PROXYDELEGATE_H

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>

class ProxyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ProxyDelegate(QObject *parent = nullptr);

    void setProxy(QSortFilterProxyModel *proxy);

protected:
    QSortFilterProxyModel *m_proxy;

};

#endif // PROXYDELEGATE_H
