#include "mainwindow.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_userCombo(new QComboBox(this))
    , m_authButton(new QPushButton("Autenticar", this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_userCombo);
    layout->addWidget(m_authButton);

    m_userCombo->setModel(&m_usersModel);

    connect(m_authButton, &QPushButton::clicked, this, &MainWindow::handleAuthentication);
    connect(&m_greeter, &QLightDM::Greeter::authenticationComplete, this, &MainWindow::handleAuthenticationResult);
}

bool MainWindow::connectToGreeter()
{
    return m_greeter.connectSync();
}

void MainWindow::handleAuthentication()
{
    const QString username = m_userCombo->currentData(QLightDM::UsersModel::NameRole).toString();
    m_greeter.authenticate(username);
}

void MainWindow::handleAuthenticationResult()
{
    if (m_greeter.isAuthenticated()) {
        const QModelIndex sessionIndex = m_sessionsModel.index(0, 0);
        if (sessionIndex.isValid()) {
            const QString sessionKey = m_sessionsModel.data(sessionIndex, QLightDM::SessionsModel::KeyRole).toString();
            m_greeter.startSessionSync(sessionKey);
        } else {
            qCritical() << "Nenhuma sessão disponível!";
        }
    } else {
        qDebug() << "Autenticação falhou!";
    }
}
