#include "mainwindow.h"
#include <QDebug>
#include <QIcon>
#include <QPropertyAnimation>
#include <QSizePolicy>
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , // <<< Corrigido aqui!
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configuração inicial
    setupDynamicUI();

    // Conexões
    connect(ui->showPasswordCheck, &QCheckBox::stateChanged, [this](int state) {
        ui->passwordField->setEchoMode(state == Qt::Checked ? QLineEdit::Normal
                                                            : QLineEdit::Password);
    });

    connect(ui->authButton, &QPushButton::clicked, this, &MainWindow::authenticateUser);
    connect(ui->passwordField, &QLineEdit::returnPressed, this, &MainWindow::authenticateUser);

    // Tenta conectar
    if (!m_greeter.connectToDaemonSync()) {
        qCritical() << "Falha ao iniciar conexão com LightDM";
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);
    }
    connect(&m_greeter,
            &QLightDM::Greeter::authenticationComplete,
            this,
            &MainWindow::handleAuthenticationResult);

    // Esconder componentes de autenticação inicialmente
    ui->authContainer->hide();
    ui->statusLabel->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QHBoxLayout *userBarLayout = qobject_cast<QHBoxLayout *>(ui->userBarContainer->layout());
    if (!userBarLayout)
        return;

    // Limpar layout existente
    QLayoutItem *item;
    while ((item = userBarLayout->takeAt(0))) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    int userCount = m_usersModel.rowCount(QModelIndex());

    for (int i = 0; i < userCount; ++i) {
        QString username
            = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::NameRole).toString();
        QString realName = m_usersModel
                               .data(m_usersModel.index(i, 0), QLightDM::UsersModel::RealNameRole)
                               .toString();

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
            for (QWidget *w : ui->userBarContainer->findChildren<QWidget *>()) {
                w->setStyleSheet(w->styleSheet().replace("border-color: #00ff00;", ""));
            }

            // Set new selection
            userContainer->setStyleSheet("background: rgba(80, 80, 80, 0.9);"
                                         "border: 2px solid #00ff00;"
                                         "border-radius: 10px;");

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
    if (!m_greeter.property("connected").toBool()) {
        qWarning() << "Greeter não conectado - Estado atual:" << m_greeter.property("connected");
        return;
    }

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
    // Verificar sucesso usando a API disponível
    if (m_greeter.authenticationUser() == m_selectedUser &&
        m_greeter.isAuthenticated()) {
        // Autenticação bem-sucedida
    } else {
        // Obter mensagem de erro alternativa
        QString error = m_greeter.property("authenticationError").toString();
        ui->statusLabel->setText(error.isEmpty() ? "Erro desconhecido" : error);
        ui->statusLabel->show();
    }
}
