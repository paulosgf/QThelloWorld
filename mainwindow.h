#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLightDM/Greeter>
#include <QLightDM/SessionsModel>
#include <QLightDM/UsersModel>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool connectToGreeter();

private slots:
    void handleAuthentication();
    void handleAuthenticationResult();

private:
    QLightDM::Greeter m_greeter;
    QLightDM::UsersModel m_usersModel;
    QLightDM::SessionsModel m_sessionsModel;
    QComboBox *m_userCombo;
    QPushButton *m_authButton;
};

#endif // MAINWINDOW_H
