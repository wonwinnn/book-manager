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
#include "networker.h"
#include "book.h"

namespace Ui {
class MainWindow;
}

enum RemoteRequest {
    FetchInfo,
    FetchCover
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void RefreshLbl();
    bool isAdded(const QString &isbn);

private slots:
    void on_AddBtn_clicked();
    void on_DelBtn_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    QItemSelectionModel *selectionModel;
    RemoteRequest request;
    NetWorker *netWorker;
    Book book;
};

#endif // MAINWINDOW_H
