
/*
 * about.cpp
 *
 *  Created on: 01.03.2017
 *      Author: Aleksey Akatyev
 *
 * Индицирует информацию об изготовителе
 *
 */


#include "about.h"

AboutDialog::AboutDialog(QWidget* parent, int V_MAJOR, int V_MINOR, int V_PATCH): QDialog(parent)
{
    about = new QLabel("Измеритель микроперемещений.");
    QChar copyrightChar(0xA9, 0x00);
    QChar leftDoubleQuotesChar(0xAB, 0x00);
    QChar rightDoubleQuotesChar(0xBB, 0x00);
    copyright = new QLabel(QString("%1 2022 ООО ИМЦ %2Микро%3").arg(copyrightChar).arg(leftDoubleQuotesChar).arg(rightDoubleQuotesChar));
    developed = new QLabel(QString("Разработано ООО %1ВИПП Техника%2 для ООО %1Микромех%2").arg(leftDoubleQuotesChar).arg(rightDoubleQuotesChar));
    support = new QLabel("Техническая поддержка: imcmikro@mail.ru");
    webAdress = new QLabel("www.imcmikro.ru");
    thanks = new QLabel(QString("Выражаем благодарность ООО ВИПП %1Техника%2 за поддержку\nпри разработке продукта.").arg(leftDoubleQuotesChar).arg(rightDoubleQuotesChar));
    version = new QLabel(QString("Версия %1.%2.%3").arg(V_MAJOR).arg(V_MINOR).arg(V_PATCH));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(version);
    mainLayout->addWidget(about);
    mainLayout->addWidget(copyright);
    mainLayout->addWidget(thanks);
    mainLayout->addWidget(support);
    mainLayout->addWidget(webAdress);
    setLayout(mainLayout);
    setWindowTitle("О программе");
}
