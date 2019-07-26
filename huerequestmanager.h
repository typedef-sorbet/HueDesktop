#ifndef HUEREQUESTMANAGER_H
#define HUEREQUESTMANAGER_H

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
#include <QThread>

class HueRequestManager : public QObject {
    Q_OBJECT
protected:
    QNetworkAccessManager *mgr;

    // RGB to Hue XY code adapted from tim on StackOverflow (accepted answer at https://stackoverflow.com/questions/22564187/rgb-to-philips-hue-hsb)
    std::vector<double> *getRGBtoXY(float cred, float cgreen, float cblue)
    {
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
public:
    explicit HueRequestManager(QObject *parent = 0) : QObject(parent) {
        mgr = new QNetworkAccessManager(this);
    }

    Q_INVOKABLE void changeLights(float r, float g, float b)
    {
        // TODO scale for multiple lights? I don't exactly intend for this to go public, so I wouldn't need to do this for a while, if at all...
        std::vector<double> *xy = this->getRGBtoXY(r, g, b);
        double x = xy->at(0), y = xy->at(1);

        QUrl url_1("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/groups/1/action");

        QNetworkRequest req_1, req_2;

        std::ostringstream data_stream;
        data_stream << "{\"xy\": [" << x << ", " << y << "]}";

        req_1.setUrl(url_1);
        req_1.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

        QByteArray data(data_stream.str().c_str());

        mgr->put(req_1, data);
    }
};

#endif // HUEREQUESTMANAGER_H
