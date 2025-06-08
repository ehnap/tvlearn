#include "channeldata.h"
#include <QJsonObject>

ChannelData::ChannelData()
    : m_name(""), m_url("")
{
}

ChannelData::ChannelData(const QString &name, const QString &url)
    : m_name(name), m_url(url)
{
}

QString ChannelData::name() const
{
    return m_name;
}

void ChannelData::setName(const QString &name)
{
    m_name = name;
}

QString ChannelData::url() const
{
    return m_url;
}

void ChannelData::setUrl(const QString &url)
{
    m_url = url;
}

QJsonObject ChannelData::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["url"] = m_url;
    return json;
}

ChannelData ChannelData::fromJson(const QJsonObject &json)
{
    QString name = json["name"].toString();
    QString url = json["url"].toString();
    return ChannelData(name, url);
}