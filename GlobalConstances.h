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

#ifndef GLOBALCONSTANCES_H
#define GLOBALCONSTANCES_H

#include <QString>
static const QString g_dbFileName = "data.dat";
static const QString g_dbTableName = "daily";

static const QString g_titleLabel = "Title";
static const QString g_dateLabel = "Date";
static const QString g_authorLabel = "Author";
static const QString g_weatherLabel = "Weather";
static const QString g_contentLabel = "Content";

#define CHECKED         1
#define TITLE           2
#define DATE            3
#define WEATHER         4
#define AUTHOR          5
#define CONTENT         6

#endif // GLOBALCONSTANCES_H

