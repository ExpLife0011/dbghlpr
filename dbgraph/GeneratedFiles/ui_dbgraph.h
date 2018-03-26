/********************************************************************************
** Form generated from reading UI file 'dbgraph.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBGRAPH_H
#define UI_DBGRAPH_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dbgraphClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTabWidget *dissassembly_tab_widget;
    QWidget *mnemonic_tab;
    QWidget *graph_tab;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *dbgraphClass)
    {
        if (dbgraphClass->objectName().isEmpty())
            dbgraphClass->setObjectName(QStringLiteral("dbgraphClass"));
        dbgraphClass->resize(1312, 1055);
        centralWidget = new QWidget(dbgraphClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        dissassembly_tab_widget = new QTabWidget(centralWidget);
        dissassembly_tab_widget->setObjectName(QStringLiteral("dissassembly_tab_widget"));
        dissassembly_tab_widget->setTabBarAutoHide(true);
        mnemonic_tab = new QWidget();
        mnemonic_tab->setObjectName(QStringLiteral("mnemonic_tab"));
        dissassembly_tab_widget->addTab(mnemonic_tab, QString());
        graph_tab = new QWidget();
        graph_tab->setObjectName(QStringLiteral("graph_tab"));
        dissassembly_tab_widget->addTab(graph_tab, QString());

        gridLayout->addWidget(dissassembly_tab_widget, 0, 0, 1, 1);

        dbgraphClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(dbgraphClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        dbgraphClass->setStatusBar(statusBar);

        retranslateUi(dbgraphClass);

        dissassembly_tab_widget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(dbgraphClass);
    } // setupUi

    void retranslateUi(QMainWindow *dbgraphClass)
    {
        dbgraphClass->setWindowTitle(QApplication::translate("dbgraphClass", "dbgraph", Q_NULLPTR));
        dissassembly_tab_widget->setTabText(dissassembly_tab_widget->indexOf(mnemonic_tab), QApplication::translate("dbgraphClass", "mnemonic", Q_NULLPTR));
        dissassembly_tab_widget->setTabText(dissassembly_tab_widget->indexOf(graph_tab), QApplication::translate("dbgraphClass", "graph", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class dbgraphClass: public Ui_dbgraphClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBGRAPH_H
