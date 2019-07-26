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

// RGB to Hue XY code adapted from tim on StackOverflow (accepted answer at https://stackoverflow.com/questions/22564187/rgb-to-philips-hue-hsb)
std::vector<double> *getRGBtoXY(float cred, float cgreen, float cblue) {
    // For the hue bulb the corners of the triangle are:
    // -Red: 0.675, 0.322
    // -Green: 0.4091, 0.518
    // -Blue: 0.167, 0.04
    double normalizedToOne[3];
    normalizedToOne[0] = (cred / 255);
    normalizedToOne[1] = (cgreen / 255);
    normalizedToOne[2] = (cblue / 255);
    float red, green, blue;

    // Make red more vivid
    if (normalizedToOne[0] > 0.04045) {
        red = (float) pow(
                (normalizedToOne[0] + 0.055) / (1.0 + 0.055), 2.4);
    } else {
        red = (float) (normalizedToOne[0] / 12.92);
    }

    // Make green more vivid
    if (normalizedToOne[1] > 0.04045) {
        green = (float) pow((normalizedToOne[1] + 0.055)
                / (1.0 + 0.055), 2.4);
    } else {
        green = (float) (normalizedToOne[1] / 12.92);
    }

    // Make blue more vivid
    if (normalizedToOne[2] > 0.04045) {
        blue = (float) pow((normalizedToOne[2] + 0.055)
                / (1.0 + 0.055), 2.4);
    } else {
        blue = (float) (normalizedToOne[2] / 12.92);
    }

    float X = (float) (red * 0.649926 + green * 0.103455 + blue * 0.197109);
    float Y = (float) (red * 0.234327 + green * 0.743075 + blue * 0.022598);
    float Z = (float) (red * 0.0000000 + green * 0.053077 + blue * 1.035763);

    float x = X / (X + Y + Z);
    float y = Y / (X + Y + Z);

    std::vector<double> *xyAsList = new std::vector<double>;
    xyAsList->push_back(x);
    xyAsList->push_back(y);
    return xyAsList;
}

void changeLights(std::vector<double> xy)
{
    double x = xy[0];
    double y = xy[1];

    static QNetworkAccessManager mgr;

    QUrl url_1("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/lights/1/state");
    QUrl url_2("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/lights/2/state");

    static QNetworkRequest req_1;
    static QNetworkRequest req_2;

    std::ostringstream data_stream;
    data_stream << "{\"xy\": [" << x << ", " << y << "]}";

    req_1.setUrl(url_1);
    req_1.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    req_2.setUrl(url_2);
    req_2.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    mgr.put(req_1, data_stream.str().c_str());
    mgr.put(req_2, data_stream.str().c_str());

    qDebug("done");
}

int main(int argc, char *argv[])
{
    std::vector<double> *xy = getRGBtoXY(255, 200, 0);
    changeLights(*xy);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
