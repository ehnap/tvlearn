#ifndef CHANNELDATA_H
#define CHANNELDATA_H

#include <QString>
#include <QJsonObject>

/**
 * @brief The ChannelData class represents a channel entry with name and URL
 */
class ChannelData
{
public:
    /**
     * @brief Default constructor
     */
    ChannelData();

    /**
     * @brief Constructor with name and URL
     * @param name The channel name
     * @param url The channel URL
     */
    ChannelData(const QString &name, const QString &url);

    /**
     * @brief Get the channel name
     * @return The channel name
     */
    QString name() const;

    /**
     * @brief Set the channel name
     * @param name The new channel name
     */
    void setName(const QString &name);

    /**
     * @brief Get the channel URL
     * @return The channel URL
     */
    QString url() const;

    /**
     * @brief Set the channel URL
     * @param url The new channel URL
     */
    void setUrl(const QString &url);

    /**
     * @brief Convert the channel data to a JSON object
     * @return JSON object representation of the channel
     */
    QJsonObject toJson() const;

    /**
     * @brief Create a ChannelData object from a JSON object
     * @param json The JSON object
     * @return ChannelData object
     */
    static ChannelData fromJson(const QJsonObject &json);

private:
    QString m_name;
    QString m_url;
};

#endif // CHANNELDATA_H