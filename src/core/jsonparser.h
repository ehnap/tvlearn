#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QString>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "../data/channeldata.h"

/**
 * @brief The JSONParser class handles parsing channel data from JSON files
 */
class JSONParser
{
public:
    /**
     * @brief Constructor
     */
    JSONParser();

    /**
     * @brief Parse channels from a JSON file
     * @param filePath Path to the JSON file
     * @return List of channel data
     * @throws QString error message if parsing fails
     */
    QList<ChannelData> parseFile(const QString &filePath);

    /**
     * @brief Parse channels from a JSON string
     * @param jsonString JSON string
     * @return List of channel data
     * @throws QString error message if parsing fails
     */
    QList<ChannelData> parseString(const QString &jsonString);

    /**
     * @brief Save channels to a JSON file
     * @param channels List of channel data
     * @param filePath Path to the JSON file
     * @return True if successful, false otherwise
     */
    bool saveToFile(const QList<ChannelData> &channels, const QString &filePath);

private:
    /**
     * @brief Parse channels from a JSON document
     * @param doc JSON document
     * @return List of channel data
     * @throws QString error message if parsing fails
     */
    QList<ChannelData> parseDocument(const QJsonDocument &doc);

    /**
     * @brief Validate a JSON object as a channel
     * @param obj JSON object
     * @return True if valid, false otherwise
     */
    bool isValidChannel(const QJsonObject &obj);
};

#endif // JSONPARSER_H