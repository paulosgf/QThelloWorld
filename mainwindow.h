#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLightDM/Greeter>
#include <QLightDM/SessionsModel>
#include <QLightDM/UsersModel>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void applyStyle();

private slots:
    void authenticateUser();
    void handleAuthenticationResult();

private:
    void setupUI();
    void setupConnections();

    QLightDM::Greeter m_greeter;
    QLightDM::UsersModel m_usersModel;
    QLightDM::SessionsModel m_sessionsModel;

    QComboBox *m_userCombo;
    QPushButton *m_authButton;
    QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H
