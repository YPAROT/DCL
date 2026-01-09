#include "filtertableview.h"
#include "ui_filtertableview.h"
#include <QScrollBar>

FilterTableView::FilterTableView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FilterTableView)
{
    ui->setupUi(this);

    //Paramètre par défaut des tables

    //On cache les barre de scrolling de la table de filtre
    ui->filterTable->horizontalScrollBar()->hide();
    ui->filterTable->verticalScrollBar()->hide();
    //Barres de scrolling sur sqltable
    ui->sqlTable->horizontalScrollBar()->setVisible(true);
    ui->sqlTable->verticalScrollBar()->setVisible(true);
    //Les headers horizontaux sont reportés sur les filtres
    ui->sqlTable->horizontalHeader()->hide();
    connect(ui->sqlTable,&CustomTableView::modelChanged,this,&FilterTableView::sqlModelChanged);
    //les headers verticaux sont cachés
    ui->sqlTable->verticalHeader()->hide();
    ui->filterTable->verticalHeader()->hide();
    //dimension max de la taille des filtres
    ui->filterTable->setMaximumHeight(60); //(30 pour Header + 30 pour filtre)
    //Possibilité de trier par colonne
    ui->sqlTable->setSortingEnabled(true);
    ui->filterTable->setSortingEnabled(false);

    //actions sur la vue

}

FilterTableView::~FilterTableView()
{
    delete ui;
}

QTableView *FilterTableView::getTableView() const
{
    return ui->sqlTable;
}

void FilterTableView::sqlModelChanged(QAbstractItemModel* model)
{
    if(model ==nullptr)
    {
        return;
    }

    //mise en place du bon nombre de colonnes
    int column_count = model->columnCount();
    ui->filterTable->setColumnCount(column_count);

    //Mise en place des headers
    ui->filterTable->setHorizontalHeader(ui->sqlTable->horizontalHeader());
    ui->filterTable->horizontalHeader()->setVisible(true); //Les headers copiés sont setvisible(false)

    //ajout des filtres
    ui->filterTable->setRowCount(1);

    //Supression des anciens filtres
    m_filters_edit.clear();

    for (int i = 0; i < column_count; ++i)
    {
        QLineEdit *lineEditor = new QLineEdit(this);
        lineEditor->setPlaceholderText(QString("filtre"));
        m_filters_edit.append(lineEditor);
        //connect(lineEditor, SIGNAL(textChanged(QString)), this, SLOT(lineEditorJustChanged()));
        ui->filterTable->setCellWidget(0, i, lineEditor);
    }

    //réajustement de la hauteur du header
    int height = ui->filterTable->horizontalHeader()->height()
                 + ui->filterTable->rowHeight(0) + 4;

    ui->filterTable->setMaximumHeight(height);

}
