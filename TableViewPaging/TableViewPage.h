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

#ifndef TABLEVIEWPAGE_H
#define TABLEVIEWPAGE_H
#include "GlobalConstances.h"

#include <QSqlTableModel>
#include <QTableView>
#include <QEvent>
#include <QThread>
#include <QSqlRecord>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QHeaderView>
#include <QComboBox>

#include <QDebug>

class QSqlDatabase;

class SqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
        explicit SqlTableModel(QWidget *parent = 0, QSqlDatabase db = QSqlDatabase()):
        QSqlTableModel(parent, db)
    {
    }

    Qt::ItemFlags flags( const QModelIndex &index ) const
    {
        if(!index.isValid())
            return 0;

        if ( index.column() == CHECKED )
            return (QSqlTableModel::flags(index) &~Qt::ItemIsEditable);

        return  QSqlTableModel::flags(index);
    }

    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole )
    {
        if(!index.isValid())
            return false;

        if( index.column() != CHECKED )
        {
            QSqlTableModel::setData(index, value, Qt::EditRole);
            return submit();//实时提交有些慢，退出时再提交
        }

        return QSqlTableModel::setData(index, value, Qt::EditRole);
    }

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const
    {
        if(!index.isValid())
            return QVariant();

        if (role == Qt::DisplayRole)
        {
            if ( index.column() == CHECKED )
            {
                int checked = this->record(index.row()).value(CHECKED).toInt();
                return  checked;// == 1 ? Qt::Checked : Qt::Unchecked;
            }
        }

        return QSqlTableModel::data(index, role);
    }
};

class TableViewPage : public QTableView
{
    Q_OBJECT
public:
    explicit TableViewPage(QWidget *parent = 0);
    ~TableViewPage();

    // 为了获取被隐藏的列数据，直接从当前tableview的model是无法
    // 获取被隐藏列的数据的，只能通过数据库的model取得数据
    QSqlTableModel* sqlModel() { return m_sqlModel; }

    QList<QVariant> currentRowInfo() const;
    int currentPage() const { return m_nCurPage; }
    int totalSize() const { return m_nTotal; }
    int lastPageSize() const { return m_nLastPageSize; }

signals:
    void insertAct();
    void removeAct();

public slots:
    void insert(const QList<QVariant> &valueList);
    void remove();

    void previousPage();
    void nextPage();
    void gotoPage(int index);
    int  totalPage();
    void updateModel();

    void highLightView(bool);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    int lastPageSize();

private:
    SqlTableModel*      m_sqlModel;
    QThread             m_thread;

    int                 m_nStartId;
    int                 m_nPageSize;
    int                 m_nCurPageSize;
    int                 m_nLastPageSize;
    int                 m_nTotal;

    int                 m_nCurPage;
    int                 m_nTotalPage;
};


class BackgroundItemDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    explicit BackgroundItemDelegate(QWidget* parent):QItemDelegate(parent){}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index ) const
    {
        // 得到index的文本，以便后面绘制
        QString text = (index.data(Qt::DisplayRole)).toString();
        QStyleOptionViewItemV4 opt = setOptions(index, option);
        // 文本显示对齐方式
//        opt.displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;;

        qobject_cast<TableViewPage*>(parent())->horizontalHeader()->resizeSection(CHECKED, 20);

        painter->save();
        QPen pen;

        if( index.column() == CHECKED )
        {
            painter->restore();
            //获取值
            bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
            //按钮的风格选项
            QStyleOptionButton *checkOpt = new QStyleOptionButton();
            checkOpt->state |= QStyle::State_Enabled;
            //根据值判断是否选中
            if(checked)
            {
                checkOpt->state |= QStyle::State_On;
            }
            else
            {
                checkOpt->state |= QStyle::State_Off;
            }
            //返回QCheckBox几何形状
            checkOpt->rect = CheckBoxRect(option);
            //绘制QCheckBox
            QApplication::style()->drawControl(QStyle::CE_CheckBox, checkOpt, painter);
        }
        // 绘制文本颜色
        else if(index.column() == TITLE)
        {
            // 第 1 列 绘制背景
            painter->fillRect(opt.rect, QBrush(QColor(22, 83, 134)));

            // 设置文本颜色画笔， 下同
            pen.setColor(QColor(200,106,123));
        }
        else if(index.column() == DATE)
        {
            pen.setColor(QColor(200, 100, 140));
        }
        else if(index.column() == WEATHER)
        {
            QString weather = qobject_cast<TableViewPage*>(parent())->
                    sqlModel()->index(index.row(), WEATHER).data().toString();
            if( weather == "晴" )
            {
                pen.setColor(QColor(255,0,0));
//                painter->fillRect(opt.rect, QBrush(QColor(120, 113, 34)));
            }
            else if(weather == "雨天")
            {
                pen.setColor(QColor(105,120,210));
            }
            else if(weather == "阴天")
            {
                pen.setColor(QColor(100,100,100));
            }
            else
                pen.setColor(QColor(140,200,25));
        }
        else if(index.column() == AUTHOR)
        {
            pen.setColor(QColor(40,240,25));
            QStyleOptionComboBox *comboboxOpt = new QStyleOptionComboBox;
            comboboxOpt->rect = option.rect;
            comboboxOpt->state = option.state;
            comboboxOpt->state |= QStyle::State_Enabled;
            comboboxOpt->editable = false;
            comboboxOpt->currentText = "text";
//            comboboxOpt->rect = ComboBoxRect(option);
            QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, comboboxOpt, painter);
        }
        else if(index.column() == CONTENT)
        {
            // 偶数行绘制背景
            if( index.row()%2 == 0)
                painter->fillRect(opt.rect, QBrush(QColor(52, 113, 34)));

            pen.setColor(QColor(140,200,125));
        }

        if( index.column() != CHECKED )
        {
            painter->setPen(Qt::NoPen);
            painter->setRenderHint(QPainter::Antialiasing, true);
            // 选中时的背景颜色，貌似这里没起作用？
            if( opt.state & QStyle::State_Selected )
            {
                painter->setBrush(option.palette.highlightedText());
                painter->fillRect(opt.rect, QBrush(QColor(22, 183, 134)));
//                QItemDelegate::paint(painter, option, index);
            }
            // 开始绘制背景颜色和文本
            painter->setPen(pen);
            drawBackground(painter, opt, index);
            painter->drawText(opt.rect, opt.displayAlignment, text);
            painter->restore();
        }
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index)
    {
        if( index.column() != CHECKED )
            return false;

        if( (event->type() == QEvent::MouseButtonRelease) ||
                (event->type() == QEvent::MouseButtonDblClick) )
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
            if( mouse_event->button() == Qt::LeftButton &&
                    CheckBoxRect(option).contains(mouse_event->pos()) )
            {
                int checked = model->data(index, Qt::DisplayRole).toInt();
                return model->setData(index, checked == 1 ? 0 : 1, Qt::CheckStateRole);
            }
        }

        return false;
    }

    QRect CheckBoxRect(const QStyleOptionViewItemV4 &viewItemStyleOptions)const
    {
        //绘制按钮所需要的参数
        QStyleOptionButton checkBoxStyleOption;
        //按照给定的风格参数 返回元素子区域
        QRect checkBoxRect = QApplication::style()->subElementRect( QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
        //返回QCheckBox坐标
        QPoint checkBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
                             viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);
        //返回QCheckBox几何形状
        return QRect(checkBoxPoint, checkBoxRect.size());
    }

    QRect ComboBoxRect(const QStyleOptionViewItemV4 &viewItemStyleOptions)const
    {
        //绘制按钮所需要的参数
        QStyleOptionComplex comboBoxStyleOption;
        //按照给定的风格参数 返回元素子区域
        QRect comboBoxRect = QApplication::style()->subControlRect(QStyle::CC_ComboBox, &comboBoxStyleOption, QStyle::SC_ComboBoxEditField);
        //返回QCheckBox坐标
        QPoint comboBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width()  + comboBoxRect.width() ,
                             viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - comboBoxRect.height() / 2);
        //返回QCheckBox几何形状
        return QRect(comboBoxPoint, comboBoxRect.size());
    }
};

#endif // TABLEVIEWPAGE_H
