#include "foreignkeydelegate.h"
#include <QComboBox>
#include <QPainter>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDebug>
#include <QSqlRelationalTableModel>

ForeignKeyDelegate::ForeignKeyDelegate(
    QObject *parent) : ProxyDelegate(parent)
{
}

QWidget *ForeignKeyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{   
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable(false); // Désactive l'édition directe

    // Remplit le QComboBox avec les données de la table étrangère
    //récupération du modèle
    const QSqlRelationalTableModel* model = m_proxy ? dynamic_cast<const QSqlRelationalTableModel*>(m_proxy->sourceModel()) : dynamic_cast<const QSqlRelationalTableModel*>(index.model());
    //récupération de la table étrnagère
    QSqlTableModel *foreign_model = model ? model->relationModel(index.column()) : nullptr;
    //si elle n'existe pas on retourne un éditeur gérant le proxy simplement
    if(!foreign_model) return ProxyDelegate::createEditor(parent,option,index);
    editor->setModel(foreign_model);
    //récupération de la colonne à afficher grâce au modele
    const QString display_column_name = model->relation(index.column()).displayColumn();
    //Vérification du codage par le driver
    const QSqlDriver *driver = foreign_model->database().driver(); //pour bien faire se fait sur la table foreign mais elle utilise normalement le même driver que la table qui la référence
    const QString display_column_encoded = driver->isIdentifierEscaped(display_column_name,QSqlDriver::FieldName) ? driver->stripDelimiters(display_column_name,QSqlDriver::FieldName):display_column_name;

    editor->setModelColumn(foreign_model->fieldIndex(display_column_encoded));

    return editor;
}

void ForeignKeyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(!index.isValid()) return;

    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return ProxyDelegate::setEditorData(editor,index);

    //gestion du proxy
    QModelIndex source_index = m_proxy ? m_proxy->mapToSource(index) : index;

    QVariant value = source_index.data(Qt::DisplayRole);//Le display donne accès au nom dans la table lié et non à l'index comme l'editrole

    //récupération du type (copié de la façon de faire dans QsqlRelationalDelegate. Faire plus simple?)
    const QByteArray editor_property_name = editor->metaObject()->userProperty().name();
    if (!editor_property_name.isEmpty()) {
        if (!value.isValid())
            value = QVariant(editor->property(editor_property_name.data()).userType());
        editor->setProperty(editor_property_name.data(), value);
        return;
    }

}

void ForeignKeyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(!index.isValid()) return;

    //Récupération du modele
    QSqlRelationalTableModel* source_model = m_proxy ? dynamic_cast<QSqlRelationalTableModel*>(m_proxy->sourceModel()) : dynamic_cast<QSqlRelationalTableModel*>(model);
    //récupération de la table étrangère
    QSqlTableModel *foreign_model = source_model ? source_model->relationModel(index.column()) : nullptr;
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!source_model || !foreign_model || !comboBox) {
        ProxyDelegate::setModelData(editor, model, index);
        return;
    }
    //Récupération du driver
    const QSqlDriver *const driver = foreign_model->database().driver();

    //On récupère le bon index si il y a un proxy
    QModelIndex source_index = m_proxy ? m_proxy->mapToSource(index) : index;
    if(!source_index.isValid()) return;

    int currentItem = comboBox->currentIndex();

    //recherche de l'index et du nom dans la table étrangère
    const QString display_column_name = source_model->relation(source_index.column()).displayColumn();
    const QString index_column_name = source_model->relation(source_index.column()).indexColumn();

    const QString display_column_encoded = driver->isIdentifierEscaped(display_column_name,QSqlDriver::FieldName) ? driver->stripDelimiters(display_column_name,QSqlDriver::FieldName):display_column_name;
    const QString index_column_encoded = driver->isIdentifierEscaped(index_column_name,QSqlDriver::FieldName) ? driver->stripDelimiters(index_column_name,QSqlDriver::FieldName):index_column_name;

    //Display
    source_model->setData(source_index,foreign_model->data(foreign_model->index(currentItem,foreign_model->fieldIndex(display_column_encoded))),Qt::DisplayRole);
    //Edit
    source_model->setData(source_index,foreign_model->data(foreign_model->index(currentItem,foreign_model->fieldIndex(index_column_encoded))),Qt::EditRole);

}

void ForeignKeyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //On récupère le bon index si il y a un proxy
    QModelIndex source_index = m_proxy ? m_proxy->mapToSource(index) : index;
    QVariant displayText = source_index.data(Qt::DisplayRole);

    // Gestion des options de dessin
    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;


    //gestion du background role
    const QSqlRelationalTableModel* model = dynamic_cast<const QSqlRelationalTableModel*>(source_index.model());
    if (model && model->isDirty(source_index)) {
        painter->fillRect(opt.rect, Qt::yellow);
    }

    // Dessine le texte
    painter->drawText(opt.rect, displayText.toString(),opt.displayAlignment);
}


