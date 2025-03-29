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
#include <QLineEdit>
#include <QStackedWidget>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void applyStyle(float opacity = 0.9);

private slots:
    void authenticateUser();
    void handleAuthenticationResult();
    void showPasswordField();

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
    QLineEdit *m_passwordField;
    QStackedWidget *m_authStack;
    QString m_selectedUser;
};

#endif // MAINWINDOW_H
