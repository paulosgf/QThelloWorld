#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 600);
    window.show();

    window.setWindowOpacity(0.6);

    QString style = QString(R"(
        QWidget {
            background-color: rgba(45, 45, 45, %1);
            color: #ffffff;
            font-size: 16px;
        }

        QLineEdit {
            min-width: 250px;
            min-height: 40px;
            padding: 10px;
            border: 2px solid rgba(77, 77, 77, %1);
            border-radius: 5px;
            background-color: rgba(64, 64, 64, %1);
            color: #ffffff;
        }

        QPushButton {
            min-width: 200px;
            min-height: 50px;
            background-color: rgba(96, 96, 96, %1);
            border: 2px solid rgba(77, 77, 77, %1);
            border-radius: 5px;
            padding: 10px;
            color: #ffffff;
        }

        QPushButton:hover {
            background-color: rgba(112, 112, 112, %1);
        }

        QPushButton:pressed {
            background-color: rgba(80, 80, 80, %1);
        }

        QCheckBox {
            color: #ffffff;
            spacing: 5px;
            margin-left: 10px;
        }

        QLabel#statusLabel {
            color: #ff4444;
            font-size: 14px;
            margin-top: 15px;
        }
    )")
                        .arg(0.6); // <- valor da opacidade

    window.setStyleSheet(style);

    return app.exec();
}
