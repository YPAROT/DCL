#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include <QDate>
#include "proxydelegate.h"

class DateDelegate : public ProxyDelegate
{
    Q_OBJECT

public:
    explicit DateDelegate(QObject* parent = nullptr);

    // Méthodes à surcharger pour le comportement personnalisé
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // DATEDELEGATE_H

