#include "book.h"

Book::Book() : d(new Book::Private())
{
}

Book::~Book()
{
    delete d;
    d = 0;
}

QString Book::getIsbn() const
{
    return d->m_isbn;
}

void Book::setIsbn(const QString &isbn)
{
    d->m_isbn = isbn;
}

QString Book::getTitle() const
{
    return d->m_title;
}

void Book::setTitle(const QString &title)
{
    d->m_title = title;
}

QStringList Book::getAuthors() const
{
    return d->m_authors;
}

void Book::setAuthors(const QStringList &authors)
{
    d->m_authors = authors;
}

QString Book::getRating() const
{
    return d->m_rating;
}

void Book::setRating(const QString &rating)
{
    d->m_rating = rating;
}

QByteArray Book::getCover() const
{
    return d->m_coverdata;
}

void Book::setCover(const QByteArray &coverdata)
{
    d->m_coverdata = coverdata;
}
/*
bool Book::isAdded()
{
    return d->m_isAdded;
}

void Book::Added(const bool &added)
{
    d->m_isAdded = added;
}
*/
