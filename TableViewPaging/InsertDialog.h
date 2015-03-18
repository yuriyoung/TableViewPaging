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

#ifndef INSERTDIALOG_H
#define INSERTDIALOG_H

#include <QDialog>

namespace Ui {
class InsertDialog;
}

class InsertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertDialog(QWidget *parent = 0);
    ~InsertDialog();
    void initInfo(const QList<QVariant> &list);

signals:
    void accepted( const QList<QVariant> & );

private slots:
    void submite();

private:
    Ui::InsertDialog *ui;
};

#endif // INSERTDIALOG_H
