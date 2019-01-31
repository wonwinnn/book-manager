#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->IsbnEdit->setPlaceholderText("Input ISBN");
    ui->TitleEdit->setPlaceholderText("Input title");


    model = new QSqlTableModel(this);
    model->setTable("book");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(0, Qt::Horizontal, tr("ISBN"));
    model->setHeaderData(1, Qt::Horizontal, tr("Title"));

    ui->BookView->setModel(model);
    ui->BookView->verticalHeader()->hide(); //hide order number
    //ui->BookView->setColumnHidden(0, true); //hide ISBN
    ui->BookView->horizontalHeader()->setMinimumSectionSize(134); //BookView width=268, 268/2=1234
    ui->BookView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->BookView->setSelectionBehavior(QAbstractItemView::SelectRows); //only select row

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AddBtn_clicked()
{
    QString Isbn = ui->IsbnEdit->text();
    QString Title = ui->TitleEdit->text();
    int rowNum = model->rowCount();//count rows
    model->insertRow(rowNum);
    model->setData(model->index(rowNum, 0), Isbn);
    model->setData(model->index(rowNum, 1), Title);
    model->submitAll();
}

void MainWindow::on_DelBtn_clicked()
{
    int curRow = ui->BookView->currentIndex().row(); //select current row
    model->removeRow(curRow);
    if (QMessageBox::Yes == QMessageBox::warning(this,
                                  tr("Delete"),
                                  tr("Are you sure?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No)){
        model->submitAll();
    } else {
        model->revertAll();
    }
}

void MainWindow::on_SearchBtn_clicked()
{
   ui->SearchBtn->setEnabled(false);
   QString Isbn = ui->IsbnEdit->text();
   QUrl url(QString("https://api.douban.com/v2/book/isbn/:%1").arg(Isbn));
   QNetworkRequest request;
   request.setUrl(url);
   manager.get(request);

}

void MainWindow::replyFinished(QNetworkReply *reply)
{
   ui->SearchBtn->setEnabled(true);

   if(reply->error() == QNetworkReply::NoError)
   {
       QJsonParseError parseJsonErr;
       QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &parseJsonErr);
       if(!(parseJsonErr.error == QJsonParseError::NoError))
       {
           qDebug() << QString("Wrong");
       }
       QJsonObject jsonObject = jsonDocument.object();
       QString title = jsonObject["title"].toString();
       ui->TitleEdit->setText(title);
   }
   else
   {
       qDebug() << "Error";
   }
}

