#include <QGuiApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <cmath>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QNetworkRequest>
#include <sstream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QQuickStyle>
#include <QFontDatabase>
#include "huerequestmanager.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<HueRequestManager>("com.sanctity", 1, 0, "HueRequestManager");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    HueRequestManager hrm;

    QGuiApplication app(argc, argv);

//    QQuickStyle::setStyle("Material");

    QFontDatabase::addApplicationFont(":/materialdesignicons-webfont.ttf");

    QQmlApplicationEngine engine;

    hrm.getScenes();
    hrm.getLights();

    QStringList sceneNames, lightNames, groupNames;

    for(QString key : hrm.scenes.keys())
    {
        sceneNames.append(key);
    }

    for(QString key : hrm.lights.keys())
    {
        lightNames.append(key);
    }

    for(QString key : hrm.groups.keys())
    {
        groupNames.append(key);
    }

    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("scenes_model", QVariant::fromValue(sceneNames));
    ctxt->setContextProperty("lights_model", QVariant::fromValue(lightNames));
    ctxt->setContextProperty("groups_model", QVariant::fromValue(groupNames));

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

//    HueRequestManager tryThis = engine.rootObjects().first()->findChild<HueRequestManager>();

    return app.exec();
}
