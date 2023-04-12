/********************************************************************************
** Form generated from reading UI file 'adas_1080p.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef ADAS_UI_H
#define ADAS_UI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QLabel *label_rear_img;
    QLabel *label_front_img;
    QLabel *label_right_txt;
    QLabel *label_front_txt;
    QLabel *label_left_txt;
    QLabel *label_rear_txt;
    QLabel *label_right_img;
    QLabel *label_left_img;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(1920, 1080);
        Form->setMaximumSize(QSize(1920, 1080));
        Form->setWindowTitle(QString::fromUtf8(""));
        Form->setAutoFillBackground(false);
        Form->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 56, 70);"));
        horizontalLayoutWidget = new QWidget(Form);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 1921, 1091));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        label_rear_img = new QLabel(horizontalLayoutWidget);
        label_rear_img->setObjectName(QString::fromUtf8("label_rear_img"));
        label_rear_img->setMaximumSize(QSize(950, 500));

        gridLayout->addWidget(label_rear_img, 1, 1, 1, 1);

        label_front_img = new QLabel(horizontalLayoutWidget);
        label_front_img->setObjectName(QString::fromUtf8("label_front_img"));
        label_front_img->setMaximumSize(QSize(950, 500));

        gridLayout->addWidget(label_front_img, 1, 0, 1, 1);

        label_right_txt = new QLabel(horizontalLayoutWidget);
        label_right_txt->setObjectName(QString::fromUtf8("label_right_txt"));
        label_right_txt->setMaximumSize(QSize(950, 50));
        label_right_txt->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));\n"
"font: 75 16pt \"URW Gothic\";\n"
"color: white;"));
        label_right_txt->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        label_right_txt->setMargin(10);

        gridLayout->addWidget(label_right_txt, 3, 1, 1, 1);

        label_front_txt = new QLabel(horizontalLayoutWidget);
        label_front_txt->setObjectName(QString::fromUtf8("label_front_txt"));
        label_front_txt->setMaximumSize(QSize(950, 50));
        label_front_txt->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));\n"
"font: 75 16pt \"URW Gothic\";\n"
"color: white;"));
        label_front_txt->setTextFormat(Qt::AutoText);
        label_front_txt->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        label_front_txt->setMargin(10);

        gridLayout->addWidget(label_front_txt, 0, 0, 1, 1);

        label_left_txt = new QLabel(horizontalLayoutWidget);
        label_left_txt->setObjectName(QString::fromUtf8("label_left_txt"));
        label_left_txt->setMaximumSize(QSize(950, 50));
        label_left_txt->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));\n"
"font: 75 16pt \"URW Gothic\";\n"
"color: white;"));
        label_left_txt->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        label_left_txt->setMargin(10);
        label_left_txt->setIndent(-1);

        gridLayout->addWidget(label_left_txt, 3, 0, 1, 1);

        label_rear_txt = new QLabel(horizontalLayoutWidget);
        label_rear_txt->setObjectName(QString::fromUtf8("label_rear_txt"));
        label_rear_txt->setMaximumSize(QSize(950, 50));
        label_rear_txt->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));\n"
"font: 75 16pt \"URW Gothic\";\n"
"color: white;"));
        label_rear_txt->setScaledContents(false);
        label_rear_txt->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        label_rear_txt->setMargin(10);

        gridLayout->addWidget(label_rear_txt, 0, 1, 1, 1);

        label_right_img = new QLabel(horizontalLayoutWidget);
        label_right_img->setObjectName(QString::fromUtf8("label_right_img"));
        label_right_img->setMaximumSize(QSize(950, 500));

        gridLayout->addWidget(label_right_img, 4, 1, 1, 1);

        label_left_img = new QLabel(horizontalLayoutWidget);
        label_left_img->setObjectName(QString::fromUtf8("label_left_img"));
        label_left_img->setMaximumSize(QSize(950, 500));

        gridLayout->addWidget(label_left_img, 4, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        label_rear_img->setText(QCoreApplication::translate("Form", "TextLabel", nullptr));
        label_front_img->setText(QCoreApplication::translate("Form", "TextLabel", nullptr));
        label_right_txt->setText(QCoreApplication::translate("Form", "RIGHT", nullptr));
        label_front_txt->setText(QCoreApplication::translate("Form", "FRONT", nullptr));
        label_left_txt->setText(QCoreApplication::translate("Form", "LEFT", nullptr));
        label_rear_txt->setText(QCoreApplication::translate("Form", "REAR", nullptr));
        label_right_img->setText(QCoreApplication::translate("Form", "TextLabel", nullptr));
        label_left_img->setText(QCoreApplication::translate("Form", "TextLabel", nullptr));
        (void)Form;
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // ADAS_UI_H
