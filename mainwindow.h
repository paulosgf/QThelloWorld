#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLightDM/Greeter>
#include <QLightDM/UsersModel>
#include <QLightDM/SessionsModel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void applyStyle(float opacity = 0.9); // Chave de configuração de transparência

private slots:
    void authenticateUser(const QString &username);
    void handleAuthenticationResult();

private:
    void setupUI();
    void setupConnections();
    void createUserBar();

    QLightDM::Greeter m_greeter;
    QLightDM::UsersModel m_usersModel;
    QLightDM::SessionsModel m_sessionsModel;

    QWidget *m_userBar;
    QPushButton *m_authButton;
    QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H
