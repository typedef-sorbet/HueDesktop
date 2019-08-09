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
#include <QEventLoop>
#include <QQuickView>
#include <QQmlContext>
#include <stdlib.h>

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
    // Constants
    static const bool LIGHTS_OFF = false;
    static const bool LIGHTS_ON = true;
    bool current_power_state;
    std::tuple<float, float, float, int> *buffered_request;
    QMap<QString, QString> scenes;
    QMap<QString, QString> lights;
    QMap<QString, QString> groups;
    explicit HueRequestManager(QObject *parent = 0) : QObject(parent) {
        mgr = new QNetworkAccessManager(this);
        current_power_state = true;
        buffered_request = nullptr;
        scenes = QMap<QString, QString>();
        lights = QMap<QString, QString>();
        groups = QMap<QString, QString>();
    }
    HueRequestManager(HueRequestManager& other)
    {
        mgr = other.mgr;
        mgr->setParent(this);
        current_power_state = other.current_power_state;
        buffered_request = other.buffered_request;
        scenes = other.scenes;
        lights = other.lights;
        groups = other.groups;
    }

    Q_INVOKABLE void changeLights(float r, float g, float b, int bri, int lightOrGroup, QString which)
    {
        qDebug() << "Recieved request to change lights to color (" << r << "," << g << "," << b << "," << bri << ").";

        if(current_power_state == LIGHTS_OFF)
        {
            qDebug() << "Buffering the request.";

            // TODO is this free necessary?
            free(buffered_request);
            buffered_request = new std::tuple<float, float, float, int>(r, g, b, bri);

            return;
        }

        // TODO scale for multiple lights? I don't exactly intend for this to go public, so I wouldn't need to do this for a while, if at all...
        std::vector<double> *xy = this->getRGBtoXY(r, g, b);
        double x = xy->at(0), y = xy->at(1);

        // TODO do some string formatting here.
        std::ostringstream string_builder;
        QString which_object = (lightOrGroup == 0 ? this->lights : this->groups)[which];
        string_builder << "http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/" << (lightOrGroup == 0 ? "lights" : "groups") << "/" << which_object.toStdString() << "/" << (lightOrGroup == 0 ? "state" : "action");
        qDebug() << string_builder.str().c_str();
        QUrl url_1(string_builder.str().c_str());

        QNetworkRequest req_1;

        string_builder = std::ostringstream();
        string_builder << "{\"xy\": [" << x << ", " << y << "], \"bri\":" << bri << "}";

        qDebug() << string_builder.str().c_str();

        req_1.setUrl(url_1);
        req_1.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

        QByteArray data(string_builder.str().c_str());

        mgr->put(req_1, data);
    }

    Q_INVOKABLE void switchLights(bool onOrOff)
    {
        qDebug() << "Recieved request to turn the lights" << (onOrOff ? "on" : "off");

        QUrl url("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/groups/1/action");

        QNetworkRequest req;

        std::ostringstream string_builder;

        string_builder << "{\"on\": " << (onOrOff ? "true" : "false");

        if(onOrOff && buffered_request)
        {
            qDebug() << "Packaging \"on\" request with previously buffered color change request.";
            std::vector<double> *xy = this->getRGBtoXY(std::get<0>(*buffered_request), std::get<1>(*buffered_request), std::get<2>(*buffered_request));
            string_builder << ", \"xy\": [" << xy->at(0) << ", " << xy->at(1) << "], \"bri\": " << std::get<3>(*buffered_request);
            free(buffered_request);
            buffered_request = nullptr;
        }

        string_builder << "}";

        req.setUrl(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

        QByteArray data(string_builder.str().c_str());

        mgr->put(req, data);

        current_power_state = onOrOff;
    }

    Q_INVOKABLE void getScenes()
    {
        QUrl get_scenes_url("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/scenes");

        QNetworkRequest req(get_scenes_url);

        QNetworkReply *reply;
        QEventLoop loop;

        reply = mgr->get(req);

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

        loop.exec();

        // at this point, we should have gotten the scenes from the Bridge

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();

        QStringList names;

        for(QString key : root.keys())
        {
            QString name = root.value(key).toObject().value("name").toString();

            // insert backwards so we can refer via user-spec'd name
            this->scenes.insert(name, key);

            names.append(name);
        }

//        for(QString key : this->scenes.keys())
//        {
//            qDebug() << key << ": " << this->scenes[key];
//        }
    }

    Q_INVOKABLE void getLights()
    {
        // gets individual lights and groups from the root API json response

        QUrl get_info_url("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl");

        QNetworkRequest req(get_info_url);

        QNetworkReply *reply;
        QEventLoop loop;

        reply = mgr->get(req);

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

        loop.exec();

        // at this point, we should have gotten the json from the Bridge

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();

        QJsonObject groups_obj = root.value("groups").toObject(), lights_obj = root.value("lights").toObject();

        for(QString group_key : groups_obj.keys())
        {
            QString name = groups_obj.value(group_key).toObject().value("name").toString();
            this->groups.insert(name, group_key);
            qDebug() << name << ": " << group_key;
        }

        for(QString light_key : lights_obj.keys())
        {
            QString name = lights_obj.value(light_key).toObject().value("name").toString();
            this->lights.insert(name, light_key);
            qDebug() << name << ": " << light_key;
        }

    }

    Q_INVOKABLE void setScene(QString sceneName)
    {
        QUrl url("http://192.168.0.45/api/bqdaMSCscyVhBgZhkrF5ptFm2-NhJSAAg3rVmskl/groups/1/action");

        QNetworkRequest req;

        req.setUrl(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

        std::ostringstream string_builder;

        string_builder << "{\"scene\": \"" << this->scenes[sceneName].toStdString() << "\"}";

        QByteArray data(string_builder.str().c_str());

        qDebug() << data;

        mgr->put(req, data);
    }
};

#endif // HUEREQUESTMANAGER_H
