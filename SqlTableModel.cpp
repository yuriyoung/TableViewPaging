#include "SqlTableModel.h"
#include "GlobalConstances.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

SqlTableModel::SqlTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db)
{
    m_id = 0;
    m_nStartId = -1;
    m_nCurPageCount = 0;
    m_nAPageCount = 6;
    m_nTotal = 0;
}

QVariant SqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation == Qt::Vertical && role == Qt::DisplayRole )
        return section + m_id;

    return QSqlTableModel::headerData(section, orientation, role);
}

int SqlTableModel::rowCount(const QModelIndex &parent)
{
    return QSqlTableModel::rowCount(parent);
}

void SqlTableModel::previousPage()
{
    if(m_nStartId <= 0 || m_nTotal <= 0)
        return;

    m_nCurPageCount = m_nAPageCount;
    m_nStartId -= m_nAPageCount;

    this->updateModel();
}

void SqlTableModel::nextPage()
{
    if( m_nCurPageCount < m_nAPageCount || m_nTotal <= 0)
        return;

    m_nStartId += m_nAPageCount;
    m_nCurPageCount = m_nTotal - m_nStartId > m_nAPageCount ? m_nAPageCount : m_nTotal - m_nStartId;

    this->updateModel();
}

void SqlTableModel::gotoPage(int index)
{
    if( index < m_nAPageCount || m_nTotal <= 0)
        return;

    m_nCurPageCount = m_nTotal % m_nAPageCount;
    m_nStartId = m_nTotal - index;

    this->updateModel();
}

void SqlTableModel::updateModel()
{
    m_nTotal = rowCount();
    m_nCurPageCount = m_nTotal > m_nAPageCount ? m_nAPageCount : m_nTotal;

    QString strFilter = QString(" 1=1 limit %1,%2").arg(m_nStartId).arg(m_nAPageCount);
    this->setId(m_nStartId+1);
    this->setFilter(strFilter);
    this->select();
}
