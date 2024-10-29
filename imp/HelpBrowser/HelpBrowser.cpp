// ======================================================================
//  HelpBrowser.cpp
// ======================================================================
//                   This file is a part of the book 
//             "Qt 5.3 Professional programming with C++"
// ======================================================================
//  Copyright (c) 2014 by Max Schlee
//
//  Email : Max.Schlee@neonway.com
//  Blog  : http://www.maxschlee.com
//
//  Social Networks
//  ---------------
//  FaceBook : http://www.facebook.com/mschlee
//  Twitter  : http://twitter.com/Max_Schlee
//  2Look.me : http://2look.me/NW100003
//  Xing     : http://www.xing.com/profile/Max_Schlee
//  vk.com   : https://vk.com/max.schlee
// ======================================================================


//
/*
 *
 *  Created on: 02.03.2017
 *      Author: Aleksey Akatyev
 *
 *  Вывод окна помощи
 *
 */

#include <QApplication>
#include "HelpBrowser.h"



HelpBrowser::HelpBrowser(QObject* pparent, const QString& strPath, const QString& strFileName)
{
    QPushButton*  pcmdBack    = new QPushButton("<<");
    QPushButton*  pcmdHome    = new QPushButton("Главная");
    QPushButton*  pcmdForward = new QPushButton(">>");
    QTextBrowser* ptxtBrowser = new QTextBrowser;


    QPoint center = QDesktopWidget().availableGeometry().center(); //получаем координаты центра экрана
    int iWidth = QDesktopWidget().availableGeometry().width();
    int iHeight = QDesktopWidget().availableGeometry().height();
    center.setX(center.x() - (SIZE_HELP_WINDOW_X * iWidth / 2));
    center.setY(center.y() - (SIZE_HELP_WINDOW_Y * iHeight / 2));
    this->resize(SIZE_HELP_WINDOW_X * iWidth, SIZE_HELP_WINDOW_Y * iHeight);
    this->move(center);
    this->setWindowTitle("Руководство по работе с программой Измерение микроперемещений");


    connect(pcmdBack, SIGNAL(clicked()),
            ptxtBrowser, SLOT(backward())
           );
    connect(pcmdHome, SIGNAL(clicked()),
            ptxtBrowser, SLOT(home())
           );
    connect(pcmdForward, SIGNAL(clicked()),
            ptxtBrowser, SLOT(forward())
           );
    connect(ptxtBrowser, SIGNAL(backwardAvailable(bool)),
            pcmdBack, SLOT(setEnabled(bool))
           );
    connect(ptxtBrowser, SIGNAL(forwardAvailable(bool)),
            pcmdForward, SLOT(setEnabled(bool))
           );

    ptxtBrowser->setSearchPaths(QStringList() << strPath);
    ptxtBrowser->setSource(QString(strFileName));

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    QHBoxLayout* phbxLayout = new QHBoxLayout;
    phbxLayout->addWidget(pcmdBack);
    phbxLayout->addWidget(pcmdHome);
    phbxLayout->addWidget(pcmdForward);
    pvbxLayout->addLayout(phbxLayout);
    pvbxLayout->addWidget(ptxtBrowser);
    setLayout(pvbxLayout);

    connect(pparent, SIGNAL(sigCloseGeneralWindow()), this, SLOT(deleteHelpBrowser()));
}


void HelpBrowser::deleteHelpBrowser(void)
{
    this->deleteLater();
}
