#include "filtertableview.h"
#include "ui_filtertableview.h"
#include <QScrollBar>
#include <QSqlRelationalTableModel>
#include "componenttablemodel.h"
#include "customrelationalmodel.h"

FilterTableView::FilterTableView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FilterTableView)
{
    ui->setupUi(this);

    //Paramètre par défaut des tables
    //--> Scrolling commun
    //Barres de scrolling sur sqltable
    ui->sqlTable->horizontalScrollBar()->setVisible(true);
    ui->sqlTable->verticalScrollBar()->setVisible(true);
    //On cache les barre de scrolling de la table de filtre
    ui->filterTable->horizontalScrollBar()->hide();
    ui->filterTable->verticalScrollBar()->hide();
    //Synchronisation des scrollbar horizontales (de sqlTable vers filterTable)
    connect(ui->sqlTable->horizontalScrollBar(), &QScrollBar::valueChanged, ui->filterTable->horizontalScrollBar(),&QScrollBar::setValue );

    //--> Header horizontaux communs
    //Les headers horizontaux sont reportés sur les filtres
    ui->filterTable->horizontalHeader()->setVisible(true);
    ui->filterTable->horizontalHeader()->setStretchLastSection(true);// On remplit toute la fenêtre
    ui->sqlTable->horizontalHeader()->hide();
    ui->sqlTable->horizontalHeader()->setStretchLastSection(true); // On remplit toute la fenêtre
    connect(ui->sqlTable,&CustomTableView::modelChanged,this,&FilterTableView::sqlModelChanged);
    connect(ui->filterTable->horizontalHeader(),&QHeaderView::sortIndicatorChanged,ui->sqlTable->horizontalHeader(),&QHeaderView::setSortIndicator);
    connect(ui->filterTable->horizontalHeader(),&QHeaderView::sectionResized,this,&FilterTableView::filterTableHeaderSectionResized);

    //les headers verticaux sont cachés
    ui->sqlTable->verticalHeader()->hide();
    ui->filterTable->verticalHeader()->hide();
    //dimension max de la taille des filtres
    ui->filterTable->setMaximumHeight(70); //(30 pour Header + 40 pour filtre)
    //Possibilité de trier par colonne
    ui->sqlTable->setSortingEnabled(true);
    ui->filterTable->setSortingEnabled(false);

    //Ajout d'un menu contextuel
    if(ui->sqlTable->editTriggers() != QAbstractItemView::NoEditTriggers)
    {
        ui->sqlTable->setContextMenuPolicy(Qt::ActionsContextMenu);

        QAction* m_insert_row_action = new QAction("Insérer une ligne",ui->sqlTable);
        ui->sqlTable->addAction(m_insert_row_action);

        QAction* m_copy_row_action = new QAction("Copier une ligne",ui->sqlTable);
        ui->sqlTable->addAction(m_copy_row_action);

        QAction* m_paste_row_action = new QAction("Coller une ligne",ui->sqlTable);
        ui->sqlTable->addAction(m_paste_row_action);

        QAction* m_delete_row_action = new QAction("Effacer une ligne insérée",ui->sqlTable);
        ui->sqlTable->addAction(m_delete_row_action);

        QAction* m_commit_action = new QAction("Commit",ui->sqlTable);
        ui->sqlTable->addAction(m_commit_action);

        QAction* m_revert_rows_action = new QAction("Annuler les changements sur les lignes sélectionnées",ui->sqlTable);
        ui->sqlTable->addAction(m_revert_rows_action);

        QAction* m_refresh_action = new QAction("Recharger la base de donnée",ui->sqlTable);
        ui->sqlTable->addAction(m_refresh_action);

        //Connections aux slots
        connect(m_insert_row_action,&QAction::triggered,this,&FilterTableView::insertRow);
        connect(m_copy_row_action,&QAction::triggered,this,&FilterTableView::copyRow);
        connect(m_paste_row_action,&QAction::triggered,this,&FilterTableView::pasteRow);
        connect(m_delete_row_action,&QAction::triggered,this,&FilterTableView::deleteRow);
        connect(m_commit_action,&QAction::triggered,this,&FilterTableView::commit);
        connect(m_revert_rows_action,&QAction::triggered,this,&FilterTableView::revertRows);
        connect(m_refresh_action,&QAction::triggered,this,&FilterTableView::refresh);

    }
    else
    {
        ui->sqlTable->setContextMenuPolicy(Qt::NoContextMenu);
    }

    //connections des signaux
    m_filter_refresh_timer = new QTimer(this);
    connect(m_filter_refresh_timer,&QTimer::timeout,this,&FilterTableView::filterRefreshTickTrigger);

}

FilterTableView::~FilterTableView()
{
    delete ui;
}

QTableView *FilterTableView::getTableView() const
{
    return ui->sqlTable;
}

void FilterTableView::oneFilterJustChanged()
{
    m_filter_refresh_timer->start(500); //refresh de 500ms
}

void FilterTableView::filterRefreshTickTrigger()
{
    m_filter_refresh_timer->stop(); //arrêt du timer
    //récupération des filtres
    QStringList filters;
    for(QLineEdit* filter_edit : std::as_const(m_filters_edit))
    {
        filters<<filter_edit->text();
    }
    emit filtersChanged(filters);
}

void FilterTableView::sqlModelChanged(QAbstractItemModel* model)
{
    if(model ==nullptr)
    {
        return;
    }

    //Supression des anciens filtres
    if(!m_filters_edit.isEmpty())
    {
        for(QLineEdit* lineEditor : std::as_const(m_filters_edit))
        {
            //déconnexion des signaux
            disconnect(lineEditor,&QLineEdit::textChanged, this, &FilterTableView::oneFilterJustChanged);
        }
        //On enlève les lineedit du QWidget
        ui->filterTable->clearContents();
        //On libère la mémoire
        qDeleteAll(m_filters_edit);
    }
    //On efface la liste maintenant que la mémoire est libérée
    m_filters_edit.clear();

    //resize des éléments et donc des headers
    ui->sqlTable->resizeColumnsToContents();
    ui->sqlTable->resizeRowsToContents();
    ui->sqlTable->resizeColumnsToContents();

    //mise en place du bon nombre de colonnes
    int column_count = model->columnCount();
    ui->filterTable->setColumnCount(column_count);

    //Mise en place des headers
    //On empêche la mise à jour de se refaire dans l'autre sens pour éviter les boucles infinies
    ui->sqlTable->blockSignals(true);
    QStringList header_labels;
    for (int i = 0; i < column_count; ++i)
    {
        ui->filterTable->setColumnWidth(i, ui->sqlTable->columnWidth(i));
        header_labels<<model->headerData(i,Qt::Horizontal).toString();

    }
    ui->filterTable->setHorizontalHeaderLabels(header_labels);
    ui->sqlTable->blockSignals(false);

    //ajout des filtres
    ui->filterTable->setRowCount(1);

    for (int i = 0; i < column_count; ++i)
    {
        QLineEdit *lineEditor = new QLineEdit(this);
        lineEditor->setPlaceholderText(QString("filtre"));
        m_filters_edit.append(lineEditor);
        connect(lineEditor, &QLineEdit::textChanged, this, &FilterTableView::oneFilterJustChanged);
        ui->filterTable->setCellWidget(0, i, lineEditor);
    }


}

void FilterTableView::filterTableHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{
    ui->sqlTable->setColumnWidth(logicalIndex,newSize);
}

void FilterTableView::insertRow()
{
    QSqlRelationalTableModel* model = static_cast<QSqlRelationalTableModel*>(ui->sqlTable->model());
    model->insertRows(model->rowCount(),1);
}

void FilterTableView::copyRow()
{

}

void FilterTableView::pasteRow()
{

}

void FilterTableView::deleteRow()
{

}

void FilterTableView::commit()
{

}

void FilterTableView::revertRows()
{
    QSqlRelationalTableModel* model = static_cast<QSqlRelationalTableModel*>(ui->sqlTable->model());
    QModelIndexList selected_rows = ui->sqlTable->selectionModel()->selectedRows();
    for(QModelIndex index : std::as_const(selected_rows))
    {
        if(index.isValid())
        {
            model->revertRow(index.row());
        }
    }
}

void FilterTableView::refresh()
{
    //attention au proxy!!!!
    qDebug() << "Type du modèle : " << ui->sqlTable->model()->metaObject()->className();
    //La fonction select n'étant pas déclarée virtual dans les classes mères, il faut pointer le bon objet

    ComponentTableModel* model1 = dynamic_cast<ComponentTableModel*>(ui->sqlTable->model());
    if(model1 != nullptr)
    {
        model1->select();
        return;
    }

    CustomRelationalModel* model2 = dynamic_cast<CustomRelationalModel*>(ui->sqlTable->model());
    if(model2 != nullptr)
    {
        qDebug()<<"Table correspondante: "<<model2->tableName();
        model2->select();
        return;
    }

    qWarning()<<"Refresh impossible car le modèle ne correspond pas aux casts possibles";
}
