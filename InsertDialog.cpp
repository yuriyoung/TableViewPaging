/**************************************************************************
** Copyright (C) 2013 Southgis Co.,Ltd. Written by Young Yori, All rights reserved.
**--------------------------------------------------------------------------------
**   Filename:LinearList.h
**   Author: Young Yori
**   Data: Administrator 2013/12/25 2013
**   Description:
**          This file is part of the Data Development Department Project and shall
**      only be used, modified, and distributed under the terms of the Data
**      Development Department this Source.
**
**      Included in the build system of the FreeType library.
**--------------------------------------------------------------------------------
**                      www.newdebug.com, 12319597@qq.com
**************************************************************************/

#include "InsertDialog.h"
#include "ui_InsertDialog.h"

InsertDialog::InsertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(accepted()),
            this, SLOT(submite()) );
}

InsertDialog::~InsertDialog()
{
    delete ui;
}

void InsertDialog::initInfo(const QList<QVariant> &list)
{
    ui->titleEdt->setText(list.at(0).toString());
//    QDateTime date = QDateTime::fromString(list.at(1).toString());
//    ui->dateEdit->setDateTime(date);
    ui->dateEdit->setDateTime(QDateTime::currentDateTime());
    ui->weatherEdt->setText(list.at(2).toString());
    ui->authorEdt->setText(list.at(3).toString());
    ui->contentEdt->setPlainText(list.at(4).toString());
}

void InsertDialog::submite()
{
    QList<QVariant> list;
    QString title = ui->titleEdt->text();
    QString date = ui->dateEdit->text();
    QString weather = ui->weatherEdt->text();
    QString author = ui->authorEdt->text();
    QString content = ui->contentEdt->toPlainText();
    list << title << date << weather << author << content;

    emit accepted(list);
}
