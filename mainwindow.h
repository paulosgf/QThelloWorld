#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>  // <<< trocado de QWidget para QMainWindow
#include <QLightDM/Greeter>
#include <QLightDM/UsersModel>
#include <QLightDM/SessionsModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow  // <<< Aqui está o ponto crítico
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void applyStyle(float opacity = 0.9);

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
