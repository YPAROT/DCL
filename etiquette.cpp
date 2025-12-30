#include "etiquette.h"
#include <QDebug>
#include <QPageSize>
#include <QPageLayout>
#include <QRegularExpression>

Etiquette::Etiquette(QWidget *parent)
    : QWidget(parent)
{
    my_Lignes.clear();
    my_ParametersPolice.clear();
    setMinimumHeight(600);
    setMinimumWidth(1000);
    my_Cadre_width = 400;

    my_printer = new QPrinter(QPrinter::ScreenResolution);
    my_printer->setOutputFormat(QPrinter::PdfFormat);
    my_printer->setPageSize(QPageSize::A4); //Passage de QT5 à QT6
}

void Etiquette::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    QPen pen;
    QFont font;
    int nb_lignes;
    int FontSizePxly;

    //Centre étiquette
    int max_x = width();
    int max_y = height();

    //qDebug()<<max_x;

    int center_x = max_x / 2;
    int center_y = max_y / 2;

    //Nombre de ligne
    nb_lignes = my_Lignes.size();

    //Origine du texte
    int sizeText_y = 0; //offset du aux angles arrondis du cadre
    for (int i = 0; i < nb_lignes; i++) {
        font.setPointSize(my_ParametersPolice.at(i));
        QFontMetrics TempFontSize(font);
        FontSizePxly = TempFontSize.height();
        sizeText_y += FontSizePxly + 2; //2= interligne
    }

    int text_x = center_x - my_Cadre_width / 2 + 10;
    int text_y = center_y - sizeText_y / 2;
    for (int i = 0; i < nb_lignes; i++) {
        font.setPointSize(my_ParametersPolice.at(i));
        painter.setFont(font);
        QFontMetrics TempFontSize(font);
        FontSizePxly = TempFontSize.height();
        text_y += FontSizePxly + 2; //2 d'interligne

        painter.drawText(text_x, text_y, my_Lignes.at(i));
    }

    //Dessin du bord de l'étiquette
    pen.setWidth(4);
    painter.setPen(pen);
    int taille_cadre_y = sizeText_y + 20;
    painter.drawRoundedRect(center_x - my_Cadre_width / 2,
                            center_y - taille_cadre_y / 2,
                            my_Cadre_width,
                            taille_cadre_y,
                            5,
                            5);

    //Taille rélle étiquette une fois imprimée?

    //passage de Qt5 à Qt6
    // QRect taille_page_px = my_printer->pageRect();
    // QRectF taille_page_mm = my_printer->pageRect(QPrinter::Millimeter);
    QPageLayout myPageLayout= my_printer->pageLayout();
    QRectF taille_page_mm = myPageLayout.paintRect(QPageLayout::Millimeter);
    QRectF taille_page_px_f = my_printer->pageRect(QPrinter::DevicePixel);
    QRect taille_page_px = taille_page_px_f.toRect();

    //    qDebug()<<taille_page_px;
    //    qDebug()<<taille_page_mm;

    double taille_reelle_x = static_cast<double>(my_Cadre_width) / taille_page_px.width()
                             * taille_page_mm.width();
    //    qDebug()<<taille_reelle_x;

    double taille_reelle_y = static_cast<double>(taille_cadre_y) / taille_page_px.height()
                             * taille_page_mm.height();
    //    qDebug()<<taille_reelle_y;

    QString info_etiquette = "Largeur: " + QString::number(taille_reelle_x)
                             + " mm / Hauteur: " + QString::number(taille_reelle_y) + " mm";
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(5, 20, info_etiquette);

    //ajout cadre visualisation
    pen.setWidth(2);
    painter.drawRect(0, 0, max_x, max_y);
}

void Etiquette::NewParameters(QStringList &Name, QStringList &Value, QVector<int> &Police)
{
    QString TempStr;
    QFont TempFont;
    //int FontSizePxlx;

    QStringList iName = Name;
    QStringList iValue = Value;
    QVector<int> iPolice = Police;

    QStringList SplitList;
    int nb_param;

    //on utilise des append => clear des vecteurs
    my_Lignes.clear();
    my_ParametersPolice.clear();

    //Si les tailles ne correspondent pas on ajuste (par rapport au nom du paramètre)
    //Nombre de paramètres
    nb_param = iName.size();

    //on vérifie que les autres champs ont au moins le même nb
    if (iValue.size() < nb_param) {
        int NbToAdd = nb_param - iValue.size();
        for (int i = 0; i < NbToAdd; i++) {
            iValue << "Erreur valeur manquante";
        }
    }

    if (iPolice.size() < nb_param) {
        int NbToAdd = nb_param - iPolice.size();
        for (int i = 0; i < NbToAdd; i++) {
            iPolice << 10; //on met 10 en valeur par défaut
        }
    }

    //On retravaille la mise en page. Il y a des césures à prévoir potentiellement
    for (int i = 0; i < nb_param; i++) {
        if (!(iName.at(i) == "")) {
            TempStr = iName.at(i) + ": " + iValue.at(i);
        } else {
            TempStr = "";
        }

        TempFont.setPointSize(iPolice.at(i));
        QFontMetrics TempFontSize(TempFont);
        //FontSizePxlx=TempFontSize.averageCharWidth();

        //if(TempStr.size()*FontSizePxlx>my_Cadre_width-40) //on laisse un espace de 10pxl de chaque côté + marge de calcul
        if (TempFontSize.horizontalAdvance(TempStr) > my_Cadre_width - 20) //10pxl de chaque côté du cadre
        {
            //Split en fonction des espaces
            SplitList = TempStr.split(QRegularExpression("\\s+")); //blancs enlevés  //migration Qt5 vers Qt6
            //on ajoute tant que on ne dépasse pas puis on passe à la ligne etc
            //le premier mot doit y aller
            TempStr = SplitList.takeFirst();
            while (SplitList.size() > 0) {
                //if((TempStr.size()+SplitList.at(0).size())*FontSizePxlx<=my_Cadre_width-20)
                if (TempFontSize.horizontalAdvance(TempStr + " " + SplitList.at(0))
                    <= my_Cadre_width - 20) //10pxl de chaque côté du cadre
                {
                    TempStr += " " + SplitList.takeFirst();
                } else {
                    //ajout dans la liste
                    my_Lignes << TempStr;
                    my_ParametersPolice << iPolice.at(i);
                    //reprise de la concaténation
                    TempStr.clear();
                }
            }

        } else {
            my_Lignes << TempStr;
            my_ParametersPolice << iPolice.at(i);
        }
    }

    update();
}

void Etiquette::PrintSelectionPDF()
{
    //Selection du fichier de sortie
    QString FileOutput = QFileDialog::getSaveFileName(0, "test", QString(), "*.pdf");

    my_printer->setOutputFileName(FileOutput);

    QPainter painter(my_printer);

    //    qDebug() << "Page px :" << my_printer->pageRect() << my_printer->paperRect();
    //    qDebug() << "Page mm :" << my_printer->pageRect(QPrinter::Millimeter) << my_printer->paperRect(QPrinter::Millimeter);

    //Code du paint event modifié

    QPen pen;
    QFont font;
    int nb_lignes;
    int FontSizePxly;

    //Centre étiquette
    int max_x = my_printer->width();
    int max_y = my_printer->height();

    //qDebug()<<max_x;

    int center_x = max_x / 2;
    int center_y = max_y / 2;

    //Nombre de ligne
    nb_lignes = my_Lignes.size();

    //Origine du texte
    int sizeText_y = 0; //offset du aux angles arrondis du cadre
    for (int i = 0; i < nb_lignes; i++) {
        font.setPointSize(my_ParametersPolice.at(i));
        QFontMetrics TempFontSize(font);
        FontSizePxly = TempFontSize.height();
        sizeText_y += FontSizePxly + 2; //2= interligne
    }

    int text_x = center_x - my_Cadre_width / 2 + 10;
    int text_y = center_y - sizeText_y / 2;
    for (int i = 0; i < nb_lignes; i++) {
        font.setPointSize(my_ParametersPolice.at(i));
        painter.setFont(font);
        QFontMetrics TempFontSize(font);
        FontSizePxly = TempFontSize.height();
        text_y += FontSizePxly + 2; //2 d'interligne

        painter.drawText(text_x, text_y, my_Lignes.at(i));
    }
    //Dessin du bord de l'étiquette
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawRoundedRect(center_x - my_Cadre_width / 2,
                            center_y - (sizeText_y + 20) / 2,
                            my_Cadre_width,
                            sizeText_y + 20,
                            5,
                            5);
}
