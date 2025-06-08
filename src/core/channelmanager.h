#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include <QObject>
#include <QList>
#include "../data/channeldata.h"
#include "jsonparser.h"

/**
 * @brief The ChannelManager class manages channel data and selection
 */
class ChannelManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit ChannelManager(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ChannelManager();

    /**
     * @brief Load channels from a file
     * @param filePath Path to the channels file
     * @return True if successful, false otherwise
     */
    bool loadFromFile(const QString &filePath);

    /**
     * @brief Save channels to a file
     * @param filePath Path to the channels file
     * @return True if successful, false otherwise
     */
    bool saveToFile(const QString &filePath);

    /**
     * @brief Get all channels
     * @return List of all channels
     */
    QList<ChannelData> channels() const;

    /**
     * @brief Get the current channel
     * @return Current channel data
     */
    ChannelData currentChannel() const;

    /**
     * @brief Get the current channel index
     * @return Current channel index
     */
    int currentIndex() const;

    /**
     * @brief Set the current channel by index
     * @param index Channel index
     * @return True if successful, false if index is out of range
     */
    bool setCurrentIndex(int index);

    /**
     * @brief Add a new channel
     * @param channel Channel data
     */
    void addChannel(const ChannelData &channel);

    /**
     * @brief Remove a channel by index
     * @param index Channel index
     * @return True if successful, false if index is out of range
     */
    bool removeChannel(int index);

    /**
     * @brief Update a channel by index
     * @param index Channel index
     * @param channel New channel data
     * @return True if successful, false if index is out of range
     */
    bool updateChannel(int index, const ChannelData &channel);

    /**
     * @brief Get the number of channels
     * @return Number of channels
     */
    int count() const;

signals:
    /**
     * @brief Signal emitted when channels are loaded
     */
    void channelsLoaded();

    /**
     * @brief Signal emitted when the current channel changes
     * @param channel New current channel
     */
    void currentChannelChanged(const ChannelData &channel);

    /**
     * @brief Signal emitted when the channel list changes
     */
    void channelListChanged();

private:
    QList<ChannelData> m_channels;
    int m_currentIndex;
    JSONParser m_jsonParser;
};

#endif // CHANNELMANAGER_H