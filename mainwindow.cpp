#include "mainwindow.h"
#include <QPropertyAnimation>
#include <QIcon>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyle(0.8);
    setupConnections();
}

void MainWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Barra de usuários
    m_userBar = new QWidget(this);
    QHBoxLayout *userBarLayout = new QHBoxLayout(m_userBar);
    userBarLayout->setSpacing(20);
    userBarLayout->setAlignment(Qt::AlignCenter);
    createUserBar();

    // Stack para alternar entre seleção de usuário e senha
    m_authStack = new QStackedWidget(this);

    // Widget inicial (seleção de usuário)
    QWidget *userSelectWidget = new QWidget();
    QVBoxLayout *userSelectLayout = new QVBoxLayout(userSelectWidget);

    m_authButton = new QPushButton("Autenticar", this);
    userSelectLayout->addWidget(m_authButton);

    // Widget para autenticação por senha
    QWidget *passwordWidget = new QWidget();
    QVBoxLayout *passwordLayout = new QVBoxLayout(passwordWidget);

    m_passwordField = new QLineEdit(this);
    m_passwordField->setPlaceholderText("Digite sua senha");
    m_passwordField->setEchoMode(QLineEdit::Password);

    QPushButton *submitButton = new QPushButton("Autenticar", this);

    passwordLayout->addWidget(m_passwordField);
    passwordLayout->addWidget(submitButton);

    // Adiciona widgets ao stack
    m_authStack->addWidget(userSelectWidget);
    m_authStack->addWidget(passwordWidget);

    // Label de status
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->hide();

    mainLayout->addWidget(m_userBar);
    mainLayout->addWidget(m_authStack);
    mainLayout->addWidget(m_statusLabel);
}

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
            min-width: 300px;
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

        QLabel#statusLabel {
            color: #ff4444;
            font-size: 14px;
        }
    )").arg(opacity);

    this->setStyleSheet(style);
}

void MainWindow::createUserBar()
{
    QHBoxLayout *userBarLayout = qobject_cast<QHBoxLayout*>(m_userBar->layout());
    if (!userBarLayout) return;

    QLayoutItem *item;
    while ((item = userBarLayout->takeAt(0))) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int userCount = m_usersModel.rowCount(QModelIndex());

    for (int i = 0; i < userCount; ++i) {
        QString username = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::NameRole).toString();
        QString realName = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::RealNameRole).toString();

        QPushButton *userButton = new QPushButton(m_userBar);
        userButton->setIcon(QIcon("icons/avatar.png"));
        userButton->setText(realName.isEmpty() ? username : realName);
        userButton->setProperty("username", username);

        connect(userButton, &QPushButton::clicked, [this, username]() {
            m_selectedUser = username;
            showPasswordField();
        });

        userBarLayout->addWidget(userButton);
    }
}

void MainWindow::setupConnections()
{
    connect(m_authButton, &QPushButton::clicked, this, &MainWindow::showPasswordField);
    connect(m_passwordField, &QLineEdit::returnPressed, this, &MainWindow::authenticateUser);
    connect(&m_greeter, &QLightDM::Greeter::authenticationComplete, this, &MainWindow::handleAuthenticationResult);
}

void MainWindow::showPasswordField()
{
    if (m_selectedUser.isEmpty()) {
        m_statusLabel->setText("Selecione um usuário primeiro");
        m_statusLabel->show();
        return;
    }
    m_authStack->setCurrentIndex(1); // Mostra campo de senha
    m_passwordField->setFocus();
}

void MainWindow::authenticateUser()
{
    if (m_selectedUser.isEmpty()) {
        m_statusLabel->setText("Selecione um usuário primeiro");
        m_statusLabel->show();
        return;
    }

    if (m_passwordField->text().isEmpty()) {
        m_statusLabel->setText("Digite sua senha");
        m_statusLabel->show();
        return;
    }

    // Autenticação em duas etapas
    m_greeter.authenticate(m_selectedUser);
    m_greeter.respond(m_passwordField->text());
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
        m_statusLabel->setText("Autenticação falhou");
        m_statusLabel->show();
        m_passwordField->clear();
    }
}
