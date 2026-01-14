#ifndef FOREIGNKEYDELEGATE_H
#define FOREIGNKEYDELEGATE_H

#include <QSqlDatabase>
#include "proxydelegate.h"

class ForeignKeyDelegate : public ProxyDelegate
{
    Q_OBJECT

public:
    explicit ForeignKeyDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;


};

#endif // FOREIGNKEYDELEGATE_H
