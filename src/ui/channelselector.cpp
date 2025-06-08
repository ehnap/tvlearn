#include "channelselector.h"
#include <QDebug>

ChannelSelector::ChannelSelector(ChannelManager *channelManager, QWidget *parent)
    : QComboBox(parent), m_channelManager(channelManager)
{
    // Set properties
    setToolTip(tr("Select Channel"));
    setMinimumWidth(200);

    // Connect signals
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChannelSelector::onCurrentIndexChanged);

    connect(m_channelManager, &ChannelManager::channelListChanged,
            this, &ChannelSelector::onChannelListChanged);

    connect(m_channelManager, &ChannelManager::currentChannelChanged,
            this, &ChannelSelector::onCurrentChannelChanged);

    // Initialize channel list
    updateChannelList();
}

ChannelSelector::~ChannelSelector()
{
}

void ChannelSelector::updateChannelList()
{
    // Clear current items
    blockSignals(true);
    clear();

    // Add channels
    QList<ChannelData> channels = m_channelManager->channels();
    for (const ChannelData &channel : channels)
    {
        addItem(channel.name());
    }

    // Set current index
    int currentIndex = m_channelManager->currentIndex();
    if (currentIndex >= 0 && currentIndex < count())
    {
        setCurrentIndex(currentIndex);
    }

    blockSignals(false);
}

ChannelData ChannelSelector::currentChannel() const
{
    return m_channelManager->currentChannel();
}

void ChannelSelector::setCurrentChannelIndex(int index)
{
    if (index >= 0 && index < count())
    {
        setCurrentIndex(index);
        m_channelManager->setCurrentIndex(index);
    }
}

void ChannelSelector::onCurrentIndexChanged(int index)
{
    if (index >= 0 && index < count())
    {
        m_channelManager->setCurrentIndex(index);
        emit channelSelected(m_channelManager->currentChannel());
    }
}

void ChannelSelector::onChannelListChanged()
{
    updateChannelList();
}

void ChannelSelector::onCurrentChannelChanged(const ChannelData &channel)
{
    int index = m_channelManager->currentIndex();
    if (index >= 0 && index < count() && index != currentIndex())
    {
        blockSignals(true);
        setCurrentIndex(index);
        blockSignals(false);
    }
}