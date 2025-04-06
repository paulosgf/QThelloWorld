#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void authenticateUser();
    void handleAuthenticationResult();

private:
    void setupDynamicUI();
    void createUserBar();

    Ui::MainWindow *ui;
    QLightDM::Greeter m_greeter;
    QLightDM::UsersModel m_usersModel;
    QLightDM::SessionsModel m_sessionsModel;
    QString m_selectedUser;
};

#endif // MAINWINDOW_H
