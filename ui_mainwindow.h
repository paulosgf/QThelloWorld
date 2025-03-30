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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *userBarContainer;
    QWidget *authContainer;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *passwordField;
    QCheckBox *showPasswordCheck;
    QPushButton *authButton;
    QLabel *statusLabel;

    void setupUi(QWidget *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        verticalLayout = new QVBoxLayout(MainWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        userBarContainer = new QWidget(MainWindow);
        userBarContainer->setObjectName(QString::fromUtf8("userBarContainer"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(userBarContainer->sizePolicy().hasHeightForWidth());
        userBarContainer->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(userBarContainer);

        authContainer = new QWidget(MainWindow);
        authContainer->setObjectName(QString::fromUtf8("authContainer"));
        verticalLayout_2 = new QVBoxLayout(authContainer);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        passwordField = new QLineEdit(authContainer);
        passwordField->setObjectName(QString::fromUtf8("passwordField"));
        passwordField->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(passwordField);

        showPasswordCheck = new QCheckBox(authContainer);
        showPasswordCheck->setObjectName(QString::fromUtf8("showPasswordCheck"));

        verticalLayout_2->addWidget(showPasswordCheck);

        authButton = new QPushButton(authContainer);
        authButton->setObjectName(QString::fromUtf8("authButton"));

        verticalLayout_2->addWidget(authButton);


        verticalLayout->addWidget(authContainer);

        statusLabel = new QLabel(MainWindow);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(statusLabel);


        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QWidget *MainWindow)
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
