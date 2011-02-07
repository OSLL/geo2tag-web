#include <QEventLoop>
#include <QDebug>
#include "DataThread.h"
#include <QList>

DataThread::DataThread(LightMap* map,QObject *parent) :
    QThread(parent),  m_settings("osll","tracker"), m_isConnected(false),m_requestIsSended(false),m_map(map)
{
        connect(this,SIGNAL(getFeed()),this,SLOT(onGetFeed()));
}



void DataThread::run()
{
    qDebug() << "thread started";
    QEventLoop eventLoop;
    QString login = m_settings.value("user").toString();
    QString password = m_settings.value("password").toString();
    if(login.isEmpty())
        login = "Mark";
    if(password.isEmpty())
        password = "test";

    m_loginQuery = new LoginQuery(login, password, parent());
    connect(m_loginQuery, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_loginQuery, SIGNAL(errorOccured(QString)), SLOT(onError(QString)));
    m_loginQuery->doRequest();
    for(;;)
    {
        if(m_isConnected)
        {
            qDebug() << "connected: auth_token=" << m_loginQuery->getUser()->getToken();
            emit(getFeed());
            //ToDo query RSS

        }
        eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents, 1000);
        QThread::msleep(5000);
    }
}


void DataThread::onConnected()
{
    m_isConnected = true;
    m_user=m_loginQuery->getUser();
    m_rssQuery=new RSSFeedQuery(m_user,60.,30.,3000000.,parent());
    qDebug() << "rssFeedQuery setted" << m_rssQuery;
    connect(m_rssQuery, SIGNAL(rssFeedReceived()), this,SLOT(onMarksGotten()));
    connect(m_rssQuery, SIGNAL(errorOccured(QString)), SLOT(onError(QString)));
}

void DataThread::onMarksGotten()
{
    m_marks=m_rssQuery->getRSSFeed();
    m_map->setMarks(m_marks);
    QList<QSharedPointer<DataMark> > marks = m_marks.values(); 
    qDebug() << "RssFeed gotten!!!!! " <<  m_marks.count() << "marks recieved";
    for (int i=0;i<marks.size();i++)
    {
            qDebug() << "mark at " << marks.at(i)->getLatitude()  << " " << marks.at(i)->getLongitude();
    }
    m_requestIsSended=false;
    //and now redraw canvas
}

void DataThread::onGetFeed()
{
    qDebug() << "onGetFeed";
    if (!m_requestIsSended)
    {
            qDebug() << " m_rssQuery->doRequest()";
            m_rssQuery->doRequest();
            m_requestIsSended=true;
    }
}




void DataThread::onError(QString message)
{
    qDebug() << "Error occured!" << message ;
}
