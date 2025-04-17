#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QLightDM/Greeter>
#include <QLightDM/SessionsModel>
#include <QLightDM/UsersModel>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString m_lastError; // Novo membro para armazenar erros
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAuthenticationComplete();
    void onShowPrompt(QString text, QLightDM::Greeter::PromptType type);
    void onShowMessage(QString text, QLightDM::Greeter::MessageType type);
    void authenticateUser();

private:
    void setupDynamicUI();
    void createUserBar();
    void connectToLightDM();
    bool m_connected = false;
    bool m_authenticating = false;

    Ui::MainWindow *ui;
    QLightDM::Greeter m_greeter;
    QLightDM::UsersModel m_usersModel;
    QLightDM::SessionsModel m_sessionsModel;
    QString m_selectedUser;
};

#endif // MAINWINDOW_H
