#include "mainwindow.h"

#include <QApplication>
#include <QLightDM/Greeter>
#include <QLightDM/SessionsModel>
#include <QLightDM/UsersModel>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QLightDM::Greeter greeter;
    QLightDM::UsersModel usersModel;
    QLightDM::SessionsModel sessionsModel;

    QWidget window;
    QVBoxLayout layout(&window);

    QComboBox userCombo;
    userCombo.setModel(&usersModel);

    QPushButton button("Autenticar");
    layout.addWidget(&userCombo);
    layout.addWidget(&button);

    // Conectar botão à autenticação
    QObject::connect(&button, &QPushButton::clicked, [&]() {
        QString username = userCombo.currentData(QLightDM::UsersModel::NameRole).toString();
        greeter.authenticate(username);
    });

    // Tratar resultado da autenticação
    QObject::connect(&greeter, &QLightDM::Greeter::authenticationComplete, [&]() {
        if (greeter.isAuthenticated()) {
            // Obter a sessão padrão ou a primeira disponível
            QModelIndex sessionIndex = sessionsModel.index(0, 0);
            if (sessionIndex.isValid()) {
                QString sessionKey = sessionsModel.data(sessionIndex, QLightDM::SessionsModel::KeyRole).toString();
                greeter.startSessionSync(sessionKey); // Usar sessionKey como QString
            } else {
                qCritical() << "Nenhuma sessão disponível!";
            }
        } else {
            qDebug() << "Autenticação falhou!";
        }
    });

    if (!greeter.connectSync()) {
        qCritical() << "Falha ao conectar ao LightDM";
        return 1;
    }

    window.show();
    return app.exec();
}
