#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_AddBtn_clicked();
    void on_DelBtn_clicked();
    void on_SearchBtn_clicked();
    void replyFinished(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    QNetworkAccessManager manager;
};

#endif // MAINWINDOW_H
