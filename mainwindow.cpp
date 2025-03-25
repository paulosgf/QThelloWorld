#include "mainwindow.h"
#include <QPropertyAnimation>
#include <QIcon>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyle(0.8); // Define a transparência (0.0 = totalmente transparente, 1.0 = opaco)
    setupConnections();
}

void MainWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // Barra de usuários
    m_userBar = new QWidget(this);
    QHBoxLayout *userBarLayout = new QHBoxLayout(m_userBar);
    userBarLayout->setSpacing(20);
    userBarLayout->setAlignment(Qt::AlignCenter);
    createUserBar();

    // Botão de autenticação
    m_authButton = new QPushButton("Autenticar", this);

    // Label de status
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->hide();

    layout->addWidget(m_userBar);
    layout->addWidget(m_authButton);
    layout->addWidget(m_statusLabel);
}

void MainWindow::applyStyle(float opacity)
{
    // Define a transparência da janela principal
    this->setWindowOpacity(opacity);

    // Estilo geral
    QString style = QString(R"(
        QWidget {
            background-color: rgba(45, 45, 45, %1);
            color: #ffffff;
            font-size: 16px;
        }

        QComboBox {
            min-width: 300px;
            min-height: 50px;
            padding: 10px;
            border: 2px solid rgba(77, 77, 77, %1);
            border-radius: 5px;
            selection-background-color: rgba(64, 64, 64, %1);
        }

        QComboBox QAbstractItemView {
            background-color: rgba(64, 64, 64, %1);
            selection-background-color: rgba(80, 80, 80, %1);
        }

        QPushButton {
            min-width: 200px;
            min-height: 50px;
            background-color: rgba(96, 96, 96, %1);
            border: 2px solid rgba(77, 77, 77, %1);
            border-radius: 5px;
            padding: 10px;
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
    )").arg(opacity); // Aplica a transparência ao fundo dos widgets

    this->setStyleSheet(style);
}

void MainWindow::createUserBar()
{
    QHBoxLayout *userBarLayout = qobject_cast<QHBoxLayout*>(m_userBar->layout());
    if (!userBarLayout) return;

    // Limpa a barra de usuários existente
    QLayoutItem *item;
    while ((item = userBarLayout->takeAt(0))) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // Obtém o número de usuários
    int userCount = m_usersModel.rowCount(QModelIndex()); // Usa QModelIndex() como índice raiz

    // Adiciona botões para cada usuário
    for (int i = 0; i < userCount; ++i) {
        QString username = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::NameRole).toString();
        QString realName = m_usersModel.data(m_usersModel.index(i, 0), QLightDM::UsersModel::RealNameRole).toString();

        // Cria um widget personalizado para o usuário
        QWidget *userWidget = new QWidget(m_userBar);
        QVBoxLayout *userLayout = new QVBoxLayout(userWidget);
        userLayout->setAlignment(Qt::AlignCenter);
        userLayout->setSpacing(5);

        // Ícone do usuário
        QLabel *iconLabel = new QLabel(userWidget);
        QPixmap iconPixmap("icons/avatar.png"); // Carrega o ícone
        if (iconPixmap.isNull()) {
            qWarning() << "Ícone não encontrado! Verifique o caminho:" << "icons/avatar.png";
        } else {
            iconLabel->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Redimensiona o ícone
        }
        iconLabel->setAlignment(Qt::AlignCenter);

        // Nome do usuário
        QLabel *nameLabel = new QLabel(userWidget);
        nameLabel->setText(realName.isEmpty() ? username : realName);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("color: #ffffff; font-size: 14px;");

        // Adiciona ícone e nome ao layout
        userLayout->addWidget(iconLabel);
        userLayout->addWidget(nameLabel);

        // Botão transparente sobre o widget do usuário
        QPushButton *userButton = new QPushButton(userWidget);
        userButton->setProperty("username", username); // Armazena o nome de usuário
        userButton->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                border: none;
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.1);
                border-radius: 10px;
            }
        )");
        userButton->setFixedSize(userWidget->sizeHint());

        // Conecta o botão à função de autenticação
        connect(userButton, &QPushButton::clicked, [this, username]() {
            authenticateUser(username);
        });

        // Adiciona o widget do usuário à barra
        userBarLayout->addWidget(userWidget);
    }
}

void MainWindow::setupConnections()
{
    connect(&m_greeter, &QLightDM::Greeter::authenticationComplete, this, &MainWindow::handleAuthenticationResult);
}

void MainWindow::authenticateUser(const QString &username)
{
    m_greeter.authenticate(username);
}

void MainWindow::handleAuthenticationResult()
{
    QPropertyAnimation *anim = new QPropertyAnimation(m_statusLabel, "geometry");
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutQuad);

    if(m_greeter.isAuthenticated()) {
        m_statusLabel->setStyleSheet("color: #44ff44;");
        m_statusLabel->setText("Autenticação bem-sucedida!");

        // Iniciar sessão
        QModelIndex sessionIndex = m_sessionsModel.index(0, 0);
        if(sessionIndex.isValid()) {
            QString sessionKey = m_sessionsModel.data(sessionIndex, QLightDM::SessionsModel::KeyRole).toString();
            m_greeter.startSessionSync(sessionKey);
        }
    } else {
        m_statusLabel->setStyleSheet("color: #ff4444;");
        m_statusLabel->setText("Falha na autenticação!");
    }

    // Animação de fade-in
    m_statusLabel->show();
    anim->setStartValue(QRect(m_statusLabel->x(), m_statusLabel->y() + 20, m_statusLabel->width(), m_statusLabel->height()));
    anim->setEndValue(m_statusLabel->geometry());
    anim->start();
}
