#ifndef CHANNELSELECTOR_H
#define CHANNELSELECTOR_H

#include <QComboBox>
#include "../core/channelmanager.h"

/**
 * @brief The ChannelSelector class displays channel list from JSON
 */
class ChannelSelector : public QComboBox
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param channelManager Channel manager instance
     * @param parent Parent widget
     */
    explicit ChannelSelector(ChannelManager *channelManager, QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ChannelSelector();

    /**
     * @brief Update the channel list
     */
    void updateChannelList();

    /**
     * @brief Get the currently selected channel
     * @return Currently selected channel data
     */
    ChannelData currentChannel() const;

    /**
     * @brief Set the current channel by index
     * @param index Channel index
     */
    void setCurrentChannelIndex(int index);

signals:
    /**
     * @brief Signal emitted when a channel is selected
     * @param channel Selected channel data
     */
    void channelSelected(const ChannelData &channel);

private slots:
    /**
     * @brief Handle channel selection change
     * @param index New channel index
     */
    void onCurrentIndexChanged(int index);

    /**
     * @brief Handle channel list change
     */
    void onChannelListChanged();

    /**
     * @brief Handle current channel change
     * @param channel New current channel
     */
    void onCurrentChannelChanged(const ChannelData &channel);

private:
    ChannelManager *m_channelManager;
};

#endif // CHANNELSELECTOR_H