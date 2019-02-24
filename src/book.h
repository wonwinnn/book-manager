#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QPixmap>

class Book
{
public:
    Book();
    ~Book();

    QString getIsbn();
    void setIsbn(const QString &isbn);

    QString getTitle();
    void setTitle(const QString &title);

    QStringList getAuthors();
    void setAuthors(const QStringList &authors);

    QString getRating();
    void setRating(const QString &rating);

    QByteArray getCover();
    void setCover(const QByteArray &coverdata);

    //void Added(bool &added);
    //bool isAdded();

private:
    class Private;
    Private *d;
};

class Book::Private
{
public:
    QString m_isbn;
    QString m_title;
    QStringList m_authors;
    QString m_rating;
    QByteArray m_coverdata;
    //bool m_isAdded;
};

#endif // BOOK_H
