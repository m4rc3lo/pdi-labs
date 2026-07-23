/********************************************************************************
** Form generated from reading UI file 'PointList.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POINTLIST_H
#define UI_POINTLIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_PointList
{
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QTableView *tableView;

    void setupUi(QDialog *PointList)
    {
        if (PointList->objectName().isEmpty())
            PointList->setObjectName(QStringLiteral("PointList"));
        PointList->resize(630, 459);
        buttonBox = new QDialogButtonBox(PointList);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(280, 420, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEdit = new QTextEdit(PointList);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(330, 10, 291, 221));
        tableView = new QTableView(PointList);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setGeometry(QRect(10, 10, 311, 431));

        retranslateUi(PointList);
        QObject::connect(buttonBox, SIGNAL(accepted()), PointList, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PointList, SLOT(reject()));

        QMetaObject::connectSlotsByName(PointList);
    } // setupUi

    void retranslateUi(QDialog *PointList)
    {
        PointList->setWindowTitle(QApplication::translate("PointList", "PointList", 0));
    } // retranslateUi

};

namespace Ui {
    class PointList: public Ui_PointList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POINTLIST_H
