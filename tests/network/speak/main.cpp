#include <QCoreApplication>
#include <QTimer>

#include <cclient.h>
#include <cclientplayer.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CClient *client = new CClient(&app);

    QObject::connect(client, &CClient::connected, [=](){
        client->signup("", "", "Takashiro", "zhaoyun");
    });

    QObject::connect(client, &CClient::loggedIn, [=](){
        foreach (const CClientPlayer *player, client->players()) {
            QObject::connect(player, &CClientPlayer::speak, [=](const QString &message){
                qDebug() << QString("%1(%2) said: %3").arg(player->screenName()).arg(player->id()).arg(message);
            });
        }
    });

    QObject::connect(client, &CClient::playerAdded, [=](const CClientPlayer *player){
        qDebug() << QString("%1(%2) came").arg(player->screenName()).arg(player->id());
        QObject::connect(player, &CClientPlayer::speak, [=](const QString &message){
            qDebug() << QString("%1(%2) said: %3").arg(player->screenName()).arg(player->id()).arg(message);
        });

        //Say hello to him.
        CClientPlayer *self = client->self();
        if (self && self != player)
            client->speakToServer(QString("welcome, %2(%1)!").arg(player->id()).arg(player->screenName()));
    });

    QObject::connect(client, &CClient::playerRemoved, [](const CClientPlayer *player){
        qDebug() << QString("%1(%2) left").arg(player->screenName()).arg(player->id());
    });

    client->connectToHost(QHostAddress::LocalHostIPv6, 5927);

    QTimer::singleShot(5000, [=](){
        qApp->quit();
    });

    return app.exec();
}