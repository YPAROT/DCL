#include "datedelegate.h"
#include <QDateEdit>
#include <QCalendarWidget>
#include <QPainter>
#include <QSqlRelationalTableModel>

DateDelegate::DateDelegate(QObject* parent)
    : ProxyDelegate(parent)
{
}

QWidget* DateDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Utiliser QDateEdit pour afficher un calendrier déroulant
    QDateEdit* editor = new QDateEdit(parent);
    editor->setCalendarPopup(true); // Active le popup du calendrier
    editor->setDisplayFormat("dd/MM/yyyy"); // Format d'affichage de la date
    return editor;
}

void DateDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    //On récupère le bon index si il y a un proxy
    QModelIndex source_index = m_proxy ? m_proxy->mapToSource(index) : index;

    QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
    QString dateString = source_index.data(Qt::EditRole).toString();
    dateEdit->setDate(QDate::fromString(dateString,"dd/MM/yyyy"));
}

void DateDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    //On récupère le bon index si il y a un proxy
    QModelIndex source_index = m_proxy ? m_proxy->mapToSource(index) : index;

    QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
    QDate date = dateEdit->date();
    model->setData(source_index, date.toString("dd/MM/yyyy"), Qt::EditRole);
}

void DateDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //On récupère le bon index si il y a un proxy
    QModelIndex source_index = m_proxy ? m_proxy->mapToSource(index) : index;

    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignCenter; //revoir l'alignement

    // Afficher la date formatée
    QString date = source_index.data(Qt::DisplayRole).toString();

    //gestion du background role
    const QSqlRelationalTableModel* model = dynamic_cast<const QSqlRelationalTableModel*>(source_index.model());
    if (model && model->isDirty(source_index)) {
        painter->fillRect(opt.rect, Qt::yellow);
    }

    painter->drawText(opt.rect, date,opt.displayAlignment);
}
