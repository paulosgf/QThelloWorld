#include "mainwindow.h"
#include <QDebug>
#include <QIcon>
#include <QPropertyAnimation>
#include <QSizePolicy>
#include "ui_mainwindow.h"
#include <QTimer>
#define LIGHTDM_DEBUG 1


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Debug
    qputenv("LIGHTDM_TEST_MODE", "1");

    // Conexão dos sinais
    connect(&m_greeter, &QLightDM::Greeter::showPrompt,
            this, &MainWindow::onShowPrompt);
    connect(&m_greeter, &QLightDM::Greeter::showMessage,
            this, &MainWindow::onShowMessage);
    connect(&m_greeter, &QLightDM::Greeter::authenticationComplete,
            this, &MainWindow::onAuthenticationComplete);
    // Conexão corrigida para o checkbox
    connect(ui->showPasswordCheck, &QCheckBox::toggled, [this](bool checked) {
        ui->passwordField->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);

        // Atualização visual imediata
        ui->passwordField->repaint();
        QApplication::processEvents();
    });

    connectToLightDM();
    setupDynamicUI();

    // Esconder componentes de autenticação inicialmente
    ui->authContainer->hide();
    ui->statusLabel->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onShowMessage(QString text, QLightDM::Greeter::MessageType type)
{
    qDebug() << "Mensagem do LightDM:" << text << "Tipo:" << type;

    // Armazena apenas mensagens de erro
    if (type == QLightDM::Greeter::MessageTypeError) {
        m_lastError = text;
        ui->statusLabel->setText(text);
        ui->statusLabel->show();
    }
}

void MainWindow::connectToLightDM()
{
    // Implementação equivalente a lightdm_greeter_connect_to_daemon_sync()
    QEventLoop loop;
    QTimer::singleShot(3000, &loop, &QEventLoop::quit); // Timeout de 3s

    // Tenta conexão periodicamente
    QTimer connectTimer;
    connect(&connectTimer, &QTimer::timeout, [this](){
        if (m_greeter.connectSync()) {
            m_connected = true;
            qDebug() << "Conectado ao LightDM!";
            QCoreApplication::instance()->quit();
        }
    });

    connectTimer.start(100);
    loop.exec();

    if (!m_connected) {
        qFatal("Falha na conexão com o LightDM daemon");
    }
}

void MainWindow::onShowPrompt(QString text, QLightDM::Greeter::PromptType type)
{
    // Implementação de show_prompt_cb
    if (type == QLightDM::Greeter::PromptTypeSecret) {
        m_greeter.respond(ui->passwordField->text());
    }
}

void MainWindow::onAuthenticationComplete()
{
    if (m_greeter.isAuthenticated()) {
        qDebug() << "Tentando iniciar sessão...";

        // Verifica sessões disponíveis
        if (m_sessionsModel.rowCount(QModelIndex()) == 0) {
            qCritical() << "Nenhuma sessão disponível!";
            return;
        }

        // Obtém a sessão padrão de forma segura
        QModelIndex sessionIndex = m_sessionsModel.index(0, 0);
        QString sessionKey = m_sessionsModel.data(
                                                sessionIndex,
                                                QLightDM::SessionsModel::KeyRole
                                                ).toString();

        qDebug() << "Sessão selecionada:" << sessionKey;
        // Força a inicialização do ambiente
        qputenv("XDG_RUNTIME_DIR", "/run/user/0");  // Correção para o erro do log
        qputenv("DBUS_SESSION_BUS_ADDRESS", qgetenv("DBUS_SESSION_BUS_ADDRESS"));

        // Inicia a sessão de forma assíncrona
        QTimer::singleShot(0, [this, sessionKey]() {
            if (m_greeter.startSessionSync(sessionKey)) {
                qDebug() << "Sessão iniciada com sucesso!";
                QCoreApplication::exit(0);  // Fecha o greeter
            } else {
                qCritical() << "Falha ao iniciar sessão. Código do erro:"
                            << m_greeter.property("authenticationErrorCode").toInt();
                            ui->statusLabel->setText("Erro ao iniciar ambiente gráfico");
            }
        });
    } else {
        QString error = m_lastError.isEmpty() ? "Autenticação falhou" : m_lastError;
        ui->statusLabel->setText(error);
        m_lastError.clear();
    }

    ui->passwordField->clear();
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
    if (!m_connected || m_authenticating) return;

    // Implementação de lightdm_greeter_authenticate()
    m_authenticating = true;
    m_greeter.authenticate(m_selectedUser);
}



