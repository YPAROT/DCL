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
    ui->sqlTable->horizontalHeader()->setStretchLastSection(true); // On remplit toute la fenêtre
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
    //On effece la liste maintenant que la mémoire est libérée
    m_filters_edit.clear();

    //mise en place du bon nombre de colonnes
    int column_count = model->columnCount();
    ui->filterTable->setColumnCount(column_count);

    //Mise en place des headers
    ui->filterTable->setHorizontalHeader(ui->sqlTable->horizontalHeader());
    ui->filterTable->horizontalHeader()->setVisible(true); //Les headers copiés sont setvisible(false)

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

    qDebug()<<"Objet "<<objectName()<<" possède "<<m_filters_edit.count()<<" filtres\n\n";

    //réajustement de la hauteur du header
    int height = ui->filterTable->horizontalHeader()->height()
                 + ui->filterTable->rowHeight(0) + 4;

    // qDebug()<<"Objet "<<objectName()<<":\nTaille header: "<<ui->filterTable->horizontalHeader()->height()<<"\nTaille première ligne: "<<ui->filterTable->rowHeight(0)<<"\n";
    // qDebug()<<"Taille d'un line edit: "<<ui->filterTable->cellWidget(0,0)->height()<<"\n\n";
    ui->filterTable->setMaximumHeight(height);
}
