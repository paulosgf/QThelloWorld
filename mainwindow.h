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
#include <QCheckBox>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void applyStyle(float opacity = 0.9);

private slots: // Remova a declaração do showPasswordField daqui
    void authenticateUser();
    void handleAuthenticationResult();

private:
    // ... mantenha apenas as declarações existentes
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
    QString m_selectedUser;
};

#endif // MAINWINDOW_H
