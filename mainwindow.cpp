#include "mainwindow.h"
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyle();
    setupConnections();
}

void MainWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // ComboBox de usuários
    m_userCombo = new QComboBox(this);
    m_userCombo->setModel(&m_usersModel);

    // Botão de autenticação
    m_authButton = new QPushButton("Autenticar", this);

    // Label de status
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->hide();

    layout->addWidget(m_userCombo);
    layout->addWidget(m_authButton);
    layout->addWidget(m_statusLabel);
}

void MainWindow::applyStyle()
{
    // Estilo geral
    this->setStyleSheet(R"(
        QWidget {
            background-color: #2d2d2d;
            color: #ffffff;
            font-size: 16px;
        }

        QComboBox {
            min-width: 300px;
            min-height: 50px;
            padding: 10px;
            border: 2px solid #4d4d4d;
            border-radius: 5px;
            selection-background-color: #404040;
        }

        QComboBox QAbstractItemView {
            background-color: #404040;
            selection-background-color: #505050;
        }

        QPushButton {
            min-width: 200px;
            min-height: 50px;
            background-color: #606060;
            border: 2px solid #4d4d4d;
            border-radius: 5px;
            padding: 10px;
        }

        QPushButton:hover {
            background-color: #707070;
        }

        QPushButton:pressed {
            background-color: #505050;
        }

        QLabel#statusLabel {
            color: #ff4444;
            font-size: 14px;
        }
    )");

    // Estilo específico para o label de status
    m_statusLabel->setObjectName("statusLabel");
}

void MainWindow::setupConnections()
{
    connect(m_authButton, &QPushButton::clicked, this, &MainWindow::authenticateUser);
    connect(&m_greeter, &QLightDM::Greeter::authenticationComplete, this, &MainWindow::handleAuthenticationResult);
}

void MainWindow::authenticateUser()
{
    QString username = m_userCombo->currentData(QLightDM::UsersModel::NameRole).toString();
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
