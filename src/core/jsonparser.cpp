#include "jsonparser.h"
#include <QFile>
#include <QJsonParseError>
#include <QTextStream>

JSONParser::JSONParser()
{
}

QList<ChannelData> JSONParser::parseFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw QString("Could not open file: %1").arg(filePath);
    }

    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();

    return parseString(jsonString);
}

QList<ChannelData> JSONParser::parseString(const QString &jsonString)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError)
    {
        throw QString("JSON parse error: %1").arg(error.errorString());
    }

    return parseDocument(doc);
}

QList<ChannelData> JSONParser::parseDocument(const QJsonDocument &doc)
{
    QList<ChannelData> channels;

    if (!doc.isArray())
    {
        throw QString("JSON document is not an array");
    }

    QJsonArray array = doc.array();
    for (int i = 0; i < array.size(); ++i)
    {
        if (!array[i].isObject())
        {
            continue;
        }

        QJsonObject obj = array[i].toObject();
        if (isValidChannel(obj))
        {
            channels.append(ChannelData::fromJson(obj));
        }
    }

    return channels;
}

bool JSONParser::isValidChannel(const QJsonObject &obj)
{
    return obj.contains("name") && obj.contains("url") &&
           obj["name"].isString() && obj["url"].isString();
}

bool JSONParser::saveToFile(const QList<ChannelData> &channels, const QString &filePath)
{
    QJsonArray array;
    for (const ChannelData &channel : channels)
    {
        array.append(channel.toJson());
    }

    QJsonDocument doc(array);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}