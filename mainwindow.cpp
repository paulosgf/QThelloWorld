#include "mainwindow.h"
#include <QDebug>
#include <QIcon>
#include <QPropertyAnimation>
#include <QSizePolicy>
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDBusConnection>
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
        qDebug() << "Autenticação bem-sucedida. Tentando iniciar sessão...";

        // Verifica sessões disponíveis
        if (m_sessionsModel.rowCount(QModelIndex()) == 0) {
            qCritical() << "Nenhuma sessão disponível!";
            ui->statusLabel->setText("Erro: Nenhuma sessão disponível");
            ui->statusLabel->show();
            return;
        }

        // Obtém a sessão padrão ou a sessão xubuntu (conforme visto nos logs)
        QString sessionKey = "xubuntu"; // Default para xubuntu baseado nos logs

        // Itera através das sessões disponíveis para encontrar a sessão xubuntu
        for (int i = 0; i < m_sessionsModel.rowCount(QModelIndex()); i++) {
            QModelIndex idx = m_sessionsModel.index(i, 0);
            QString key = m_sessionsModel.data(idx, QLightDM::SessionsModel::KeyRole).toString();
            qDebug() << "Sessão disponível:" << key;

            if (key == "xubuntu") {
                sessionKey = key;
                break;
            }
        }

        qDebug() << "Iniciando sessão:" << sessionKey;

        // Obtém o UID do usuário autenticado
        uid_t uid = 1000; // Default para usuário regular
        QDBusInterface accountsInterface("org.freedesktop.Accounts",
                                        "/org/freedesktop/Accounts",
                                        "org.freedesktop.Accounts",
                                        QDBusConnection::systemBus());

        if (accountsInterface.isValid()) {
            QDBusReply<QDBusObjectPath> userPath = accountsInterface.call("FindUserByName", m_selectedUser);
            if (userPath.isValid()) {
                QDBusInterface userInterface("org.freedesktop.Accounts",
                                           userPath.value().path(),
                                           "org.freedesktop.Accounts.User",
                                           QDBusConnection::systemBus());

                if (userInterface.isValid()) {
                    QVariant uidVariant = userInterface.property("Uid");
                    if (uidVariant.isValid()) {
                        uid = uidVariant.toUInt();
                        qDebug() << "UID do usuário" << m_selectedUser << ":" << uid;
                    }
                }
            }
        }

        // Configurar integração com systemd-logind
        QDBusInterface logindInterface("org.freedesktop.login1",
                                      "/org/freedesktop/login1",
                                      "org.freedesktop.login1.Manager",
                                      QDBusConnection::systemBus());

        if (logindInterface.isValid()) {
            qDebug() << "Usando systemd-logind para gerenciamento de sessão";

            // Configurar variáveis necessárias para systemd-logind
            QString runtimeDir = QString("/run/user/%1").arg(uid);
            qputenv("XDG_RUNTIME_DIR", runtimeDir.toUtf8());
            qputenv("XDG_SESSION_TYPE", "x11");
            qputenv("XDG_SESSION_CLASS", "user");
            qputenv("XDG_SESSION_DESKTOP", sessionKey.toUtf8());
            qputenv("XDG_SEAT", "seat0");
            qputenv("DBUS_SESSION_BUS_ADDRESS", QString("unix:path=%1/bus").arg(runtimeDir).toUtf8());
        } else {
            qWarning() << "systemd-logind não disponível!";
            // Configuração mínima caso o logind não esteja disponível
            qputenv("XDG_RUNTIME_DIR", QString("/run/user/%1").arg(uid).toUtf8());
            qputenv("DBUS_SESSION_BUS_ADDRESS", QString("unix:path=/run/user/%1/bus").arg(uid).toUtf8());
        }

        // Garante que estamos desconectando do servidor X atual
        // para permitir que a sessão do usuário assuma o controle
        QDBusConnection::sessionBus().disconnectFromBus(QDBusConnection::sessionBus().name());

        // Limpa a tela antes de iniciar a sessão
        this->hide();
        QApplication::processEvents();

        // Inicia a sessão de maneira robusta
        if (m_greeter.startSessionSync(sessionKey)) {
            qDebug() << "Comando de iniciar sessão enviado com sucesso!";
            // O greeter será encerrado pelo LightDM após startSessionSync ser bem-sucedido
            QCoreApplication::quit();
        } else {
            qCritical() << "Falha ao iniciar sessão:" << sessionKey;

            // Verifica se há informações adicionais de erro
            QVariant error = m_greeter.property("authenticationError");
            if (error.isValid()) {
                qCritical() << "Código de erro:" << error.toInt();
            }

            ui->statusLabel->setText("Falha ao iniciar sessão");
            ui->statusLabel->show();
            this->show(); // Mostrar novamente já que houve erro
        }
    } else {
        QString error = m_lastError.isEmpty() ? "Autenticação falhou" : m_lastError;
        ui->statusLabel->setText(error);
        ui->statusLabel->show();
        m_lastError.clear();

        // Limpa a senha e redefine status
        ui->passwordField->clear();
        ui->passwordField->setFocus();
    }

    m_authenticating = false;
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
    if (!m_connected || m_authenticating) {
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

    m_authenticating = true;
    ui->statusLabel->hide();

    qDebug() << "Iniciando autenticação para usuário:" << m_selectedUser;
    m_greeter.authenticate(m_selectedUser);
   m_greeter.authenticate(m_selectedUser);
}



