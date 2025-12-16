#ifndef ETIQUETTE_H
#define ETIQUETTE_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QFont>
#include <QFontMetrics>
#include <QFileDialog>
#include <QtPrintSupport/QPrinter>

class Etiquette : public QWidget
{
    Q_OBJECT
public:
    explicit Etiquette(QWidget *parent = 0);
    void paintEvent(QPaintEvent* e);

signals:

public slots:
    void NewParameters(QStringList &Name,QStringList &Value,QVector<int> &Police);
    void PrintSelectionPDF(void);

public:
    QStringList my_Lignes;
    QVector<int> my_ParametersPolice;
    QPrinter *my_printer;
    int my_Cadre_width;


};

#endif // ETIQUETTE_H
