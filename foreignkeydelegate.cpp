#include "foreignkeydelegate.h"
#include <QComboBox>
#include <QPainter>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRelationalTableModel>

ForeignKeyDelegate::ForeignKeyDelegate(
    QObject *parent,
    const QString &foreignTable,
    const QString &displayColumn,
    const QString &keyColumn,
    QSqlDatabase db
    ) : QStyledItemDelegate(parent),
    m_foreignTable(foreignTable),
    m_displayColumn(displayColumn),
    m_keyColumn(keyColumn),
    m_db(db)
{
}

QWidget *ForeignKeyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable(false); // Désactive l'édition directe

    // Remplit le QComboBox avec les données de la table étrangère
    QSqlQuery query(m_db);
    query.exec(QString("SELECT %1, %2 FROM %3").arg(m_keyColumn, m_displayColumn, m_foreignTable));
    while (query.next()) {
        editor->addItem(query.value(1).toString(), query.value(0));
    }

    return editor;
}

void ForeignKeyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;

    // Récupère la valeur actuelle de l'index
    QVariant currentId = index.data(Qt::EditRole);
    // Récupère le nom actuel
    QSqlQuery query(m_db);
    query.prepare(QString("SELECT %1 FROM %2 WHERE %3 = ?").arg(m_displayColumn, m_foreignTable, m_keyColumn));
    query.addBindValue(currentId);
    query.exec();
    if (query.next()) {
        comboBox->setCurrentText(query.value(0).toString());
    }
}

void ForeignKeyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;

    // Récupère l'ID sélectionné
    int foreignKey = comboBox->currentData().toInt();
    // Met à jour le modèle avec la nouvelle valeur
    model->setData(index, foreignKey, Qt::EditRole);
}

void ForeignKeyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Récupère la valeur actuelle de l'index
    QVariant currentId = index.data(Qt::EditRole);
    // Récupère le nom à afficher
    QSqlQuery query(m_db);
    query.prepare(QString("SELECT %1 FROM %2 WHERE %3 = ?").arg(m_displayColumn, m_foreignTable, m_keyColumn));
    query.addBindValue(currentId);
    query.exec();
    QString displayText;
    if (query.next()) {
        displayText = query.value(0).toString();
    }

    // Gestion des options de dessin
    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;


    //gestion du background role
    const QSqlRelationalTableModel* model = dynamic_cast<const QSqlRelationalTableModel*>(index.model());
    if (model && model->isDirty(index)) {
        painter->fillRect(opt.rect, Qt::yellow);
    }

    // Dessine le texte
    painter->drawText(opt.rect, displayText,opt.displayAlignment);
}
