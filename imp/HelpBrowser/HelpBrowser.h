// ======================================================================
//  HelpBrowser.h
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


/*
 *
 *  Created on: 02.03.2017
 *      Author: Aleksey Akatyev
 *
 *  Вывод окна помощи
 *
 */

#pragma once

#define SIZE_HELP_WINDOW_X 0.66
#define SIZE_HELP_WINDOW_Y 0.66

#include <QtWidgets>

// ======================================================================
class HelpBrowser : public QWidget {
    Q_OBJECT

public:
    HelpBrowser(QObject* pparent = 0, const QString& strPath = "", const QString& strFileName = "");

private slots:
    void deleteHelpBrowser(void);

};

