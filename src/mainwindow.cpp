#include "mainwindow.h"
#include "ui_mainwindow.h"

enum ColumnIndex
{
    Column_Isbn = 0,
    Column_Title = 1,
    Column_Authors = 2,
    Column_Rating = 3,
    Column_Cover = 4
};

bool isIsbn(const QString &src) 
{
    if (src.size() != 13) return 0;
    const char *s = src.toUtf8().data();
    while(*s && *s>='0' && *s<='9') s++;
    return !bool(*s);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->IsbnEdit->setPlaceholderText("Input ISBN to search book");
    ui->TitleLbl->setWordWrap(true);
    ui->CoverLbl->setScaledContents(true);
    ui->AddBtn->setEnabled(false);
    ui->DelBtn->setEnabled(false);

    model = new QSqlTableModel(this);
    model->setTable("book");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(Column_Isbn, Qt::Horizontal, tr("ISBN"));
    model->setHeaderData(Column_Title, Qt::Horizontal, tr("Title"));
    model->setHeaderData(Column_Authors, Qt::Horizontal, tr("Author(s)"));
    model->setHeaderData(Column_Rating, Qt::Horizontal, tr("Rating"));

    ui->BookView->setModel(model);
    ui->BookView->verticalHeader()->hide(); //hide order number
    ui->BookView->setColumnHidden(Column_Isbn, true); //hide ISBN
    ui->BookView->setColumnHidden(Column_Cover, true); //hide coverdata
    ui->BookView->setColumnWidth(Column_Title, 155);
    ui->BookView->setColumnWidth(Column_Authors, 155);
    ui->BookView->setColumnWidth(Column_Rating, 59);
    ui->BookView->setSelectionBehavior(QAbstractItemView::SelectRows); //only select row
    //ui->BookView->horizontalHeader()->setMaximumSectionSize(123);
    //ui->BookView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    selectionModel = ui->BookView->selectionModel();

    netWorker = NetWorker::instance();

    connect(ui->IsbnEdit,&QLineEdit::textChanged,[=](){
        if (isIsbn(ui->IsbnEdit->text())){
            book.setIsbn(ui->IsbnEdit->text());
            request = FetchInfo;
            netWorker->get(QString("https://api.douban.com/v2/book/isbn/:%1").arg(book.getIsbn()));
        }
    });

    connect(netWorker, &NetWorker::finished, [=](QNetworkReply *reply){
        switch (request) {
        case FetchInfo:{
            if(reply->error() == QNetworkReply::NoError){
                QJsonParseError parseJsonErr;
                QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &parseJsonErr);
                if(!(parseJsonErr.error == QJsonParseError::NoError)){
                    qDebug() << QString("Wrong");
                }
                QJsonObject jsonObject = jsonDocument.object();
                book.setTitle(jsonObject["title"].toString());
                QJsonArray autarray = jsonObject.value("author").toArray();
                QStringList authors;
                for(int i = 0; i< autarray.size(); i++){
                    authors.append(autarray.at(i).toString());
                }
                book.setAuthors(authors);
                QJsonValue ratVal = jsonObject.value("rating");
                QJsonObject ratObj = ratVal.toObject();
                book.setRating(ratObj["average"].toString());
                request = FetchCover;
                QJsonValue imgVal = jsonObject.value("images");
                QJsonObject imgObj = imgVal.toObject();
                netWorker->get(imgObj["small"].toString());
                //qDebug() << "FetchInfo";
            }
            else{
                qDebug() << "Error";
            }
            break;
        }
        case FetchCover:{
            book.setCover(reply->readAll());
            RefreshLbl();
            //qDebug() << "FetchCover";
            break;
        }
        }
        reply->deleteLater();
    });

    connect(ui->BookView, &QTableView::clicked, [=](){
        ui->AddBtn->setEnabled(false);
        ui->DelBtn->setEnabled(true);
        int curRow = ui->BookView->currentIndex().row(); //select current row
        book.setIsbn(model->data(model->index(curRow,Column_Isbn)).toString());
        book.setTitle(model->data(model->index(curRow,Column_Title)).toString());
        book.setAuthors(((model->data(model->index(curRow,Column_Authors))).toString()).split(","));
        book.setRating(model->data(model->index(curRow,Column_Rating)).toString());
        book.setCover(model->data(model->index(curRow,Column_Cover)).toByteArray());
        /*QSqlQuery query;
            if(!query.exec(QString("SELECT coverdata from book WHERE isbn = '%1'").arg(book.getIsbn())))
                qDebug() << "Error getting image from table:\n" << query.lastError();
            query.first();
            QByteArray outByteArray = query.value(0).toByteArray();
            book.setCover(outByteArray);*/
        RefreshLbl();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AddBtn_clicked()
{
    int rowNum = model->rowCount();//count rows
    model->insertRow(rowNum);
    model->setData(model->index(rowNum, Column_Isbn), book.getIsbn());
    model->setData(model->index(rowNum, Column_Title), book.getTitle());
    model->setData(model->index(rowNum, Column_Authors), book.getAuthors().join(", "));
    model->setData(model->index(rowNum, Column_Rating), book.getRating());
    model->setData(model->index(rowNum, Column_Cover), book.getCover());
    /*QSqlQuery query;
    query.prepare("INSERT INTO book (isbn, title, authors, rating, coverdata)"
                   "VALUES (:isbn, :title, :authors, :rating, :coverdata)");
    query.bindValue(":isbn", book.getIsbn());
    query.bindValue(":title", book.getTitle());
    query.bindValue(":authors", book.getAuthors().join(", "));
    query.bindValue(":rating", book.getRating());
    query.bindValue(":coverdata", book.getCover());
    if(!query.exec())
        qDebug() << "Error inserting into table:\n" << query.lastError();*/

    model->submitAll(); //refresh qtableview
    ui->AddBtn->setEnabled(false);
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

void MainWindow::RefreshLbl()
{
    ui->TitleLbl->setText(book.getTitle());
    QPixmap cover;
    cover.loadFromData(book.getCover());
    ui->CoverLbl->setPixmap(cover);
    ui->AddBtn->setEnabled(!isAdded(book.getIsbn()));
    ui->DelBtn->setEnabled(isAdded(book.getIsbn()));
}

bool MainWindow::isAdded(const QString &isbn)
{
    QSqlQuery query;
    query.exec(QString("SELECT * from book WHERE isbn = '%1'").arg(isbn));
    if(!query.next()){
        //qDebug() << "The book doesn't exist." << endl;
        return 0;
    }
    else{
        //qDebug() << "The book exists." << endl;
        return 1;
    }
    /*
    //The method below will update the tableview
    model->setFilter(QString("isbn= '%1'").arg(isbn));
    if (!model->select()){
        qDebug() << "The book doesn't exist." << endl;
        return 0;
    }
    else{
        qDebug() << "The book exists." << endl;
        return 1;
    }
    */
}
