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

#include "TableViewPage.h"
#include "DatabaseManager.h"

#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QHeaderView>
#include <QFocusEvent>
#include <QMenu>
#include <QAction>
#include <QSqlField>
#include <QAbstractItemView>


TableViewPage::TableViewPage(QWidget *parent) :
    QTableView(parent),
    m_nStartId(0), m_nPageSize(9), m_nCurPageSize(0), m_nTotal(0),
    m_nCurPage(1), m_nTotalPage(0)
{
    DatabaseManager *dbMgr = new DatabaseManager;
    dbMgr->createConnect();
    dbMgr->createTable();
    dbMgr->setStatus(DatabaseManager::Read);
    QSqlDatabase db = dbMgr->database();

    // initialize view
    m_sqlModel = new SqlTableModel(0, db);
    m_sqlModel->moveToThread(&m_thread);
    m_thread.start();
    m_sqlModel->setTable(g_dbTableName);
//    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_sqlModel->select();
    m_sqlModel->setHeaderData(CHECKED, Qt::Horizontal, tr("√"));
    m_sqlModel->setHeaderData(TITLE, Qt::Horizontal, tr("Title"));
    m_sqlModel->setHeaderData(DATE, Qt::Horizontal, tr("Date"));
    m_sqlModel->setHeaderData(WEATHER, Qt::Horizontal, tr("Weather"));
    m_sqlModel->setHeaderData(AUTHOR, Qt::Horizontal, tr("Author"));
    m_sqlModel->setHeaderData(CONTENT, Qt::Horizontal, tr("Content"));

    qDebug() << "row count:" << m_sqlModel->rowCount();
    m_nTotal = m_sqlModel->rowCount();
    m_nTotalPage = totalPage();
    m_nLastPageSize = lastPageSize();
    this->updateModel();    //更新每页数据
    this->setModel(m_sqlModel);
    this->hideColumn(0);
    this->verticalHeader()->hide();

    this->setItemDelegate(new BackgroundItemDelegate(this));
}

TableViewPage::~TableViewPage()
{
    m_sqlModel->submitAll();
    if(m_thread.isRunning())
    {
        m_thread.quit();
        m_thread.wait();
    }
    m_sqlModel->clear();
    m_sqlModel->deleteLater();
}

/**
 * 当前选中行的所有列的数据，在添加新记录条时，
 * 从当前选中的行数据中，取默认数据到添加记录对话框中
 * @brief TableViewPage::currentRowInfo
 * @return
 */
QList<QVariant> TableViewPage::currentRowInfo() const
{
    QList<QVariant> list;
    QModelIndex idx = this->currentIndex();
    if( !idx.isValid() ) return list;
    int r = idx.row();
    QSqlRecord record = m_sqlModel->record(r);  //取得当前行的数据集
    for( int i = TITLE; i < record.count(); ++i)//从Title位置开始取数据
    {
        QSqlField field = record.field(i);
        QString data = field.value().toString();
        list.append(data);
    }

    return list;
}

void TableViewPage::insert(const QList<QVariant> &valueList)
{
    int rowNum = m_sqlModel->rowCount();
    if( lastPageSize() >= m_nPageSize )
    {
        m_nTotalPage++;
    }
    m_nCurPage = m_nTotalPage;

    m_sqlModel->insertRow(rowNum);
    int col = CHECKED;
    m_sqlModel->setData(m_sqlModel->index(rowNum, col), 0);
    for( int i = 0; i < valueList.count(); ++i)
    {
        col++;
        QString data = valueList.at(i).toString();
        m_sqlModel->setData(m_sqlModel->index(rowNum, col), data);
    }
    m_nTotal++;

    m_sqlModel->submitAll();
    updateModel();
    qDebug() << m_sqlModel->rowCount();
    QModelIndex sel = this->model()->index(m_sqlModel->rowCount(), 1);
    this->setCurrentIndex(sel);
}

void TableViewPage::remove()
{
    QModelIndexList idxList = this->selectionModel()->selectedIndexes();

    QMap<int, int> rowMap;
    foreach (QModelIndex index, idxList)
        rowMap.insert(index.row(), 0);

    int rowDel;
    QMapIterator<int, int> rowMapIterator(rowMap);
    rowMapIterator.toBack();
    while (rowMapIterator.hasPrevious())
    {
        rowMapIterator.previous();
        rowDel = rowMapIterator.key();
        this->model()->removeRow(rowDel);
    }
    if( lastPageSize() <= 1 )
    {
        m_nLastPageSize = m_nPageSize;
        m_nTotalPage--;
        m_nCurPage--;
    }
    m_nTotal--;
    m_sqlModel->submitAll();

    updateModel();
}

void TableViewPage::previousPage()
{
    if(m_nStartId <= 0 || m_nTotal <= 0)
        return;

    m_nCurPage--;
    this->updateModel();
}

void TableViewPage::nextPage()
{
    if( m_nCurPage >= m_nTotalPage || m_nTotal <= 0)
        return;

    m_nCurPage++;
    this->updateModel();
}

/**
 * 跳转到第 index 页
 * @brief TableViewPage::gotoPage
 * @param index
 */
void TableViewPage::gotoPage(int index)
{
    if(  index <= 0 || index > m_nTotalPage || m_nTotalPage <= 0 || index == m_nCurPage)
        return;

    m_nCurPage = index;
    this->updateModel();
}

/**
 * 总页数
 * @brief TableViewPage::totalPage
 * @return
 */
int TableViewPage::totalPage()
{
    m_nTotalPage = m_nTotal / m_nPageSize;
    m_nTotalPage = (m_nTotal % m_nPageSize) > 0 ? (m_nTotalPage + 1) : m_nTotalPage;

    return m_nTotalPage;
}

/**
 * 更新每页数据
 * @brief TableViewPage::updateModel
 */
void TableViewPage::updateModel()
{
    m_sqlModel->submit();
    m_nStartId = (m_nCurPage-1) * m_nPageSize;
    QString strFilter = QString(" 1=1 limit %1,%2").arg(m_nStartId).arg(m_nPageSize);
    m_sqlModel->setFilter(strFilter);
    m_sqlModel->select();
    m_nCurPageSize = m_sqlModel->rowCount();
}

void TableViewPage::highLightView(bool enabled)
{
    if( enabled )
        this->setItemDelegate(new BackgroundItemDelegate(this));
    else
        this->setItemDelegate(new QItemDelegate(this));
}

/**
 * 最后一页记录条数
 * @brief TableViewPage::lastPageSize
 * @return
 */
int TableViewPage::lastPageSize()
{
    m_nLastPageSize = (m_nTotal % m_nPageSize) == 0 ? m_nPageSize : (m_nTotal % m_nPageSize);

    return m_nLastPageSize;
}

void TableViewPage::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* popMenu = new QMenu(this);
    QAction *insertAction = popMenu->addAction( tr("&Insert") );
    connect( insertAction, SIGNAL(triggered()), this, SIGNAL(insertAct()) );

    QAction *removeAction = popMenu->addAction( tr("&Remove") );
    removeAction->setEnabled(false);

    QModelIndex idx = this->indexAt( mapFromGlobal( QPoint( event->globalPos().x(), \
            event->globalPos().y()-this->horizontalHeader()->height())) );
    if( idx.isValid() )
    {
        removeAction->setEnabled(true);
        connect( removeAction, SIGNAL(triggered()), this, SIGNAL(removeAct()) );
    }
    else
    {
        this->clearSelection();
        this->selectionModel()->clearSelection();
    }

    popMenu->exec(event->globalPos());
    event->accept();
}
