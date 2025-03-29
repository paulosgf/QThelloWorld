#include "mainwindow.h"
#include <QPropertyAnimation>
#include <QIcon>
#include <QDebug>
#include <QMessageBox>
#include <QCheckBox>
#include <QHBoxLayout>

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

    // Stack para autenticação
    m_authStack = new QStackedWidget(this);

    // Widget de seleção de usuário
    QWidget *userSelectWidget = new QWidget();
    QVBoxLayout *userSelectLayout = new QVBoxLayout(userSelectWidget);
    m_authButton = new QPushButton("Autenticar", this);
    userSelectLayout->addWidget(m_authButton);

    // Widget de senha
    QWidget *passwordWidget = new QWidget();
    QVBoxLayout *passwordLayout = new QVBoxLayout(passwordWidget);

    // Campo de senha com opção de visualização
    QWidget *passwordContainer = new QWidget();
    QHBoxLayout *passwordHBox = new QHBoxLayout(passwordContainer);
    passwordHBox->setContentsMargins(0, 0, 0, 0);

    m_passwordField = new QLineEdit(this);
    m_passwordField->setPlaceholderText("Digite sua senha");
    m_passwordField->setEchoMode(QLineEdit::Password);

    QCheckBox *showPasswordCheck = new QCheckBox("Mostrar senha", this);

    passwordHBox->addWidget(m_passwordField);
    passwordHBox->addWidget(showPasswordCheck);

    QPushButton *submitButton = new QPushButton("Autenticar", this);

    passwordLayout->addWidget(passwordContainer);
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

    // Conexão do checkbox de mostrar senha
    connect(showPasswordCheck, &QCheckBox::stateChanged, this, [this](int state){
        m_passwordField->setEchoMode(state == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
    });
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

        QCheckBox::indicator {
            width: 20px;
            height: 20px;
        }

        QLabel#statusLabel {
            color: #ff4444;
            font-size: 14px;
            margin-top: 15px;
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

        QWidget *userContainer = new QWidget(m_userBar);
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

        QVBoxLayout *buttonLayout = new QVBoxLayout(userButton);
        buttonLayout->setAlignment(Qt::AlignCenter);
        buttonLayout->setSpacing(5);

        containerLayout->addWidget(userButton);
        userBarLayout->addWidget(userContainer);

        connect(userButton, &QPushButton::clicked, [this, username, userContainer]() {
            for(QWidget *w : m_userBar->findChildren<QWidget*>()) {
                w->setStyleSheet(w->styleSheet().replace("border-color: #00ff00;", ""));
            }
            userContainer->setStyleSheet(
                "background: rgba(80, 80, 80, 0.9);"
                "border: 2px solid #00ff00;"
                "border-radius: 10px;"
                );
            m_selectedUser = username;
        });
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
    m_authStack->setCurrentIndex(1);
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
