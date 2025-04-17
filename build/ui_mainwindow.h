/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *userBarContainer;
    QWidget *authContainer;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *passwordField;
    QCheckBox *showPasswordCheck;
    QPushButton *authButton;
    QLabel *statusLabel;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(856, 648);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        userBarContainer = new QWidget(centralwidget);
        userBarContainer->setObjectName(QString::fromUtf8("userBarContainer"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(userBarContainer->sizePolicy().hasHeightForWidth());
        userBarContainer->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(userBarContainer);

        authContainer = new QWidget(centralwidget);
        authContainer->setObjectName(QString::fromUtf8("authContainer"));
        verticalLayout_2 = new QVBoxLayout(authContainer);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        passwordField = new QLineEdit(authContainer);
        passwordField->setObjectName(QString::fromUtf8("passwordField"));
        passwordField->setMinimumSize(QSize(0, 30));
        passwordField->setMaximumSize(QSize(16777215, 40));
        passwordField->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(passwordField);

        showPasswordCheck = new QCheckBox(authContainer);
        showPasswordCheck->setObjectName(QString::fromUtf8("showPasswordCheck"));

        verticalLayout_2->addWidget(showPasswordCheck);

        authButton = new QPushButton(authContainer);
        authButton->setObjectName(QString::fromUtf8("authButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(authButton->sizePolicy().hasHeightForWidth());
        authButton->setSizePolicy(sizePolicy1);
        authButton->setMinimumSize(QSize(120, 35));
        authButton->setMaximumSize(QSize(200, 40));

        verticalLayout_2->addWidget(authButton);


        verticalLayout->addWidget(authContainer);

        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(statusLabel);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "LightDM Greeter", nullptr));
        passwordField->setPlaceholderText(QCoreApplication::translate("MainWindow", "Digite sua senha", nullptr));
        showPasswordCheck->setText(QCoreApplication::translate("MainWindow", "Mostrar senha", nullptr));
        authButton->setText(QCoreApplication::translate("MainWindow", "Autenticar", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
