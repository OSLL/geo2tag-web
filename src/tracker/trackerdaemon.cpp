#include "trackerdaemon.h"
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QTimer>

#include "ApplyMarkQuery.h"
#include "SubscribeChannelQuery.h"
#include "LoginQuery.h"


#define DEFAULT_LATITUDE 60.17
#define DEFAULT_LONGITUDE 24.95
#define DAEMON_PORT 34243
#define ERRORLOG_LENGTH 30

trackerDaemon::trackerDaemon() : QObject(NULL)
{
//TODO Create socket, connect signals
//
  m_server = new QTcpServer(this);
  if (!m_server->listen(QHostAddress::LocalHost,DAEMON_PORT)){
//     r("Critical error - can not start server!!!!!");
  }
  connect(m_server, SIGNAL(newConnection()), this, SLOT(uiConnected()));
  startGps(); // TODO investigate, how we can to stopGPS in stop()
  QTimer::singleShot(0, this, SLOT(setupBearer()));
  connect(&m_applyMarkQuery, SIGNAL(responseReceived(QString,QString)), this, SLOT(onApplyMarkResponse(QString,QString)));
  connect(&m_loginQuery, SIGNAL(responseReceived(QString,QString)), this, SLOT(onLoginResponse(QString,QString,QString)));
  connect(&m_subscribeQuery, SIGNAL(responseReceived(QString,QString)), this, SLOT(onSubscribeResponse(QString,QString)));
  connect(&m_applyChannelQuery, SIGNAL(responseReceived(QString,QString)), this, SLOT(onApplyChannelResponse(QString,QString)));
  initSettings();


}

void trackerDaemon::cleanLocalSettigns()
{
  QSettings settings("osll","tracker");
  settings.clear();

}

void trackerDaemon::initSettings()
{
  QSettings settings("osll","tracker");

  if( settings.value("magic").toString() == APP_MAGIC )
  {
    qDebug() << "magic = " << settings.value("magic").toString();
    emit readSettings();
  }
  else
  {
    emit createSettings();
  }
}

void trackerDaemon::readSettings()
{
  QSettings settings("osll","tracker");
  m_settings.channel = settings.value("channel").toString();
  m_settings.key = settings.value("key").toString();
  m_settings.user = settings.value("user").toString();
  m_settings.passw = settings.value("passwd").toString();
  m_settings.auth_token = settings.value("auth_token").toString();
  m_settings.initialized = true;
}

void trackerDaemon::createSettings()
{

    //TODO Add settings initialization

    QSettings settings("osll","tracker");
    settings.setValue("channel",m_settings.channel);
    settings.setValue("key",m_settings.key);
    settings.setValue("user",m_settings.user);
    settings.setValue("passwd",m_settings.passw);
    settings.setValue("auth_token",m_settings.auth_token);
    settings.setValue("magic",APP_MAGIC);
    m_settings.initialized = true;
}

void trackerDaemon::startGps()
{
    if (!m_positionSource) {
        m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
	m_positionSource -> setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
        QObject::connect(m_positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
        this, SLOT(positionUpdated(QGeoPositionInfo)));
    }
    m_positionSource->startUpdates();
}

void trackerDaemon::positionUpdated(QGeoPositionInfo gpsPos)
{
    m_positionInfo = gpsPos;
}

//TODO learn what it is
void trackerDaemon::setupBearer()
{  /*
    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
        & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || !canStartIAP) {
        return;
    }
    m_session = new QNetworkSession(cfg);
    m_session->open();
    m_session->waitForOpened();
    */
}

void trackerDaemon::timerEvent(QTimerEvent *te)
{
  killTimer(te->timerId());


  if( m_settings.initialized )
  {
    qDebug() << "   ... dropping mark" << QDateTime().currentDateTime();
    bool result = setMark();
  }

  startTimer(UPDATE_INTERVAL);
}

bool trackerDaemon::setMark()
{
    qreal latitude = DEFAULT_LATITUDE;
    qreal longitude = DEFAULT_LONGITUDE;
    
    if (m_positionInfo.coordinate().isValid()) {
        latitude = m_positionInfo.coordinate().latitude();
        longitude = m_positionInfo.coordinate().longitude();
        
        m_applyMarkQuery.setQuery(m_settings.auth_token,
                                                   m_settings.channel,
                                                   QString("title"),
                                                   QString("url"),
                                                   QString("description"),
                                                   latitude,
                                                   longitude,
                                                   QLocale("english").toString(QDateTime::currentDateTime(),"dd MMM yyyy hh:mm:ss"));
  //TODO place it into start()          //connect(m_applyMarkQuery, SIGNAL(responseReceived(QString,QString)), this, SLOT(onApplyMarkResponse(QString,QString)));
       m_applyMarkQuery.doRequest();
    }
    else {
        setStatus(QString("Error"),QString("GPS error"));
    }

    

    return true;
}

void trackerDaemon::onApplyMarkResponse(QString status,QString status_description)
{
    setStatus(status,status_description);
}

void trackerDaemon::onLoginResponse(QString status,QString auth_token,QString status_description){
    setStatus(status,status_description);
    if (status == QString("Ok"))
    {
        m_settings.auth_token=auth_token;
    }
}

void trackerDaemon::onSubscribeChannelResponse(QString status,QString status_description){
    setStatus(status,status_description);
}


void trackerDaemon::start(){// start adding marks by timer
    if (m_settings.auth_token!=QString("")){

        m_timerID=startTimer(100); // first update should be fast*/
    }else{
        // Write UI that we didnt authentificated
        setStatus(QString("Error"),QString("bad login or password"));
    }
}

void trackerDaemon::stop(){// stop adding marks by timer;
    if (m_timerID){
        killTimer(m_timerID);
        m_timerID=0;
    }
}


void trackerDaemon::login(QString login,QString password){
    m_settings.user=login;
    m_settings.passw=password;
    m_loginQuery.setQuery(login,password);
    m_loginQuery.doRequest();
}



void trackerDaemon::uiConnected(){
  if (!m_uiSocket){
      m_receiver=new RequestReceiver(m_uiSocket,this);

      connect(m_receiver,SIGNAL(start()),this,SLOT(start()));
      connect(m_receiver,SIGNAL(stop()),this,SLOT(stop()));
      connect(m_receiver,SIGNAL(login(QString,QString)),this,SLOT(login(QString,QString)));
      connect(m_receiver,SIGNAL(setChannel(QString,QString)),this,SLOT(setChannel(QString,QString)));
      connect(m_receiver,SIGNAL(addChannel(QString,QString)),this,SLOT(addChannel(QString,QString)));
      connect(m_receiver,SIGNAL(status()),this,SLOT(status()));



     // m_uiSocket=->nextPendingConnection();
    //  connect(m_uiSocket, SIGNAL(readyRead()), this, SLOT(processSocketData()));
  }
}

void trackerDaemon::setStatus(QString status,QString status_description){
    m_status=status;
    m_statusDescription=status_description;
    m_lastAttempt=QDateTime::currentDateTime();
}

void trackerDaemon::status(){ 
    QDataStream out(m_uiSocket);
    out << m_lastAttempt;
    out << m_status;
    if (m_status!="Ok"){
        out << m_statusDescription; 
    }
}

void trackerDaemon::setChannel(QString channel,QString channelKey){
    m_settings.channel=channel;
    m_settings.key=channelKey;
    m_subscribeQuery.setQuery(m_settings.auth_token,m_settings.channel);
    m_subscribeQuery.doRequest();
}

void trackerDaemon::addChannel(QString channel,QString channelKey){
    m_applyChannelQuery.setQuery(m_settings.auth_token,channel,QString(""),QString(""),1000);
}

/*void trackerDaemon::processSocketData(){
    QDataStream in(m_uiSocket);
    QString command("");
    in >> command;
    switch(command){
        case "start": {
            start();
            break;
        }
        case "stop":{
            stop();
            break;
        }
        case "login":{
            QString name(""),password("");
            in >> name;
            in >> password;
            if(name==QString("") || password==QString("")) return;
            login(name,password);
            break;
        }
        case "set channel":{
            QString channel(""),key("");
            in >> channel;
            in >> key;
            if (channel==QString("") || key==QString("")) return;
            setChannel(channel,key);
            break;
        }
        case "add channel":{
            QString channel(""),key("");
            in >> channel;
            in >> key;
            if (channel==QString("") || key==QString("")) return;
            addChannel(channel,key);
            break;
        }
        case "status":{
            status();
            break;
        }
    }
}*/
