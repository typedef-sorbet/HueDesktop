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

    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("scenes_model", QVariant::fromValue(QStringList(hrm.scenes.keys())));
    ctxt->setContextProperty("lights_model", QVariant::fromValue(QStringList(hrm.lights.keys())));
    ctxt->setContextProperty("groups_model", QVariant::fromValue(QStringList(hrm.groups.keys())));
    ctxt->setContextProperty("manager", &hrm);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
