#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPropertyAnimation>
#include <QIcon>
#include <QDebug>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), // <<< Corrigido aqui!
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configuração inicial
    applyStyle(0.8);
    setupDynamicUI();

    // Conexões
    connect(ui->showPasswordCheck, &QCheckBox::stateChanged, [this](int state){
        ui->passwordField->setEchoMode(state == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    connect(ui->authButton, &QPushButton::clicked, this, &MainWindow::authenticateUser);
    connect(ui->passwordField, &QLineEdit::returnPressed, this, &MainWindow::authenticateUser);
    connect(&m_greeter, &QLightDM::Greeter::authenticationComplete, this, &MainWindow::handleAuthenticationResult);

    // Esconder componentes de autenticação inicialmente
    ui->authContainer->hide();
    ui->statusLabel->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
void MainWindow::setupResponsiveLayout()
{
    // Se ainda quiser configurar espaçamento manualmente:
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (layout) {
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(15);
    }
}
*/
void MainWindow::applyStyle(float opacity)
{
    this->setWindowOpacity(opacity);

    QString style = QString(R"(
        QWidget {
            background-color: rgba(45, 45, 45, %1);
            color: #ffffff;
            font-size: 16px;
        }

        QLineEdit {
            min-width: 250px;
            min-height: 40px;
            padding: 10px;
            border: 2px solid rgba(77, 77, 77, %1);
            border-radius: 5px;
            background-color: rgba(64, 64, 64, %1);
            color: #ffffff;
        }

        QPushButton {
            min-width: 200px;
            min-height: 50px;
            background-color: rgba(96, 96, 96, %1);
            border: 2px solid rgba(77, 77, 77, %1);
            border-radius: 5px;
            padding: 10px;
            color: #ffffff;
        }

        QPushButton:hover {
            background-color: rgba(112, 112, 112, %1);
        }

        QPushButton:pressed {
            background-color: rgba(80, 80, 80, %1);
        }

        QCheckBox {
            color: #ffffff;
            spacing: 5px;
            margin-left: 10px;
        }

        QLabel#statusLabel {
            color: #ff4444;
            font-size: 14px;
            margin-top: 15px;
        }
    )").arg(opacity);

    this->setStyleSheet(style);
}

void MainWindow::setupDynamicUI()
{
    QWidget *userBarContainer = ui->userBarContainer;

    if (!userBarContainer->layout()) {
        QHBoxLayout *userBarLayout = new QHBoxLayout(userBarContainer);
        userBarLayout->setContentsMargins(0, 0, 0, 0);
        userBarLayout->setSpacing(20);
    }

    createUserBar();
}

void MainWindow::createUserBar()
{
    QHBoxLayout *userBarLayout = qobject_cast<QHBoxLayout*>(ui->userBarContainer->layout());
    if (!userBarLayout) return;

    // Limpar layout existente
    QLayoutItem *item;
    while ((item = userBarLayout->takeAt(0))) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int userCount = m_usersModel.rowCount(QModelIndex());

    for (int i = 0; i < userCount; ++i) {
        QString username = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::NameRole).toString();
        QString realName = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::RealNameRole).toString();

        QWidget *userContainer = new QWidget(ui->userBarContainer);
        QVBoxLayout *containerLayout = new QVBoxLayout(userContainer);
        containerLayout->setAlignment(Qt::AlignCenter);
        containerLayout->setContentsMargins(10, 10, 10, 10);

        QPushButton *userButton = new QPushButton(userContainer);
        userButton->setIcon(QIcon("icons/avatar.png"));
        userButton->setIconSize(QSize(64, 64));
        userButton->setText(realName.isEmpty() ? username : realName);
        userButton->setProperty("username", username);

        userButton->setStyleSheet(R"(
            QPushButton {
                border: none;
                background: transparent;
                color: white;
                font-size: 14px;
                padding: 5px;
            }
            QPushButton:hover {
                color: #00ff00;
            }
        )");

        connect(userButton, &QPushButton::clicked, [this, username, userContainer]() {
            // Reset selection
            for(QWidget *w : ui->userBarContainer->findChildren<QWidget*>()) {
                w->setStyleSheet(w->styleSheet().replace("border-color: #00ff00;", ""));
            }

            // Set new selection
            userContainer->setStyleSheet(
                "background: rgba(80, 80, 80, 0.9);"
                "border: 2px solid #00ff00;"
                "border-radius: 10px;"
                );

            m_selectedUser = username;
            ui->authContainer->show();
            ui->passwordField->setFocus();
        });

        containerLayout->addWidget(userButton);
        userBarLayout->addWidget(userContainer);
    }
}

void MainWindow::authenticateUser()
{
    if (m_selectedUser.isEmpty()) {
        ui->statusLabel->setText("Nenhum usuário selecionado");
        ui->statusLabel->show();
        return;
    }

    if (ui->passwordField->text().isEmpty()) {
        ui->statusLabel->setText("Digite sua senha");
        ui->statusLabel->show();
        return;
    }

    m_greeter.authenticate(m_selectedUser);
    m_greeter.respond(ui->passwordField->text());
}

void MainWindow::handleAuthenticationResult()
{
    if (m_greeter.isAuthenticated()) {
        QModelIndex sessionIndex = m_sessionsModel.index(0, 0);
        if (sessionIndex.isValid()) {
            QString sessionKey = m_sessionsModel.data(sessionIndex, QLightDM::SessionsModel::KeyRole).toString();
            m_greeter.startSessionSync(sessionKey);
        }
    } else {
        ui->statusLabel->setText("Autenticação falhou");
        ui->statusLabel->show();
        ui->passwordField->clear();
    }
}
