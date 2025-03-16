#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

class WebSocketServer : public QObject {
    Q_OBJECT

public:
    WebSocketServer(quint16 port, QObject *parent = nullptr)
        : QObject(parent), m_server(new QWebSocketServer("BiometricServer", QWebSocketServer::NonSecureMode, this)) {
        connect(m_server, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
        if (m_server->listen(QHostAddress::Any, port)) {
            qDebug() << "Servidor WebSocket iniciado na porta" << port;
        }
    }

private slots:
    void onNewConnection() {
        QWebSocket *socket = m_server->nextPendingConnection();
        connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onMessageReceived);
    }

    void onMessageReceived(const QString &message) {
        qDebug() << "Mensagem recebida:" << message;
        if (message == "startAuthentication") {
            // Lógica para autenticação biométrica
            QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
            socket->sendTextMessage("authenticationComplete:true");
        }
    }

private:
    QWebSocketServer *m_server;
};
#endif // MAINWINDOW_H
