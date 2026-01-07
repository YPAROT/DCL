#include "ouinondelegate.h"
#include <QComboBox>
#include <QPainter>
#include <QSqlRelationalTableModel>

OuiNonDelegate::OuiNonDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *OuiNonDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItem("Null", QVariant());
    editor->addItem("Oui", true);
    editor->addItem("Non", false);
    return editor;
}

void OuiNonDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;

    QVariant value = index.data(Qt::EditRole);
    if (!value.isValid()) {
        comboBox->setCurrentIndex(0); // "Null"
    } else if (value.toBool()) {
        comboBox->setCurrentIndex(1); // "Oui"
    } else {
        comboBox->setCurrentIndex(2); // "Non"
    }
}

void OuiNonDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;

    int currentIndex = comboBox->currentIndex();
    QVariant value;
    if (currentIndex == 0) {
        value = QVariant(); // "Null"
    } else if (currentIndex == 1) {
        value = true; // "Oui"
    } else {
        value = false; // "Non"
    }

    model->setData(index, value, Qt::EditRole);
}

void OuiNonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignCenter;

    QVariant value = index.data(Qt::DisplayRole);
    QString text;
    if (!value.isValid()) {
        text = "Null";
    } else if (value.toBool()) {
        text = "Oui";
    } else {
        text = "Non";
    }

    painter->drawText(opt.rect, text,opt.displayAlignment);

    //gestion du background role
    const QSqlRelationalTableModel* model = dynamic_cast<const QSqlRelationalTableModel*>(index.model());
    if (model && model->isDirty(index)) {
        painter->fillRect(opt.rect, Qt::yellow);
    }
}
