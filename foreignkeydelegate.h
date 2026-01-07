#ifndef FOREIGNKEYDELEGATE_H
#define FOREIGNKEYDELEGATE_H

#include <QStyledItemDelegate>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>

class ForeignKeyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ForeignKeyDelegate(
        QObject *parent = nullptr,
        const QString &foreignTable = "",
        const QString &displayColumn = "",
        const QString &keyColumn = "",
        QSqlDatabase db = QSqlDatabase()
        );

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QString m_foreignTable;
    QString m_displayColumn;
    QString m_keyColumn;
    QSqlDatabase m_db;
};

#endif // FOREIGNKEYDELEGATE_H
