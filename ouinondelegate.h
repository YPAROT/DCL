#ifndef OUINONDELEGATE_H
#define OUINONDELEGATE_H

#include "proxydelegate.h"
#include <QComboBox>

class OuiNonDelegate : public ProxyDelegate
{
    Q_OBJECT

public:
    explicit OuiNonDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#endif // OUINONDELEGATE_H
