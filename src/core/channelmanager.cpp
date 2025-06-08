#include "channelmanager.h"
#include <QDebug>

ChannelManager::ChannelManager(QObject *parent)
    : QObject(parent), m_currentIndex(-1)
{
}

ChannelManager::~ChannelManager()
{
}

bool ChannelManager::loadFromFile(const QString &filePath)
{
    try
    {
        m_channels = m_jsonParser.parseFile(filePath);
        m_currentIndex = m_channels.isEmpty() ? -1 : 0;

        emit channelsLoaded();
        emit channelListChanged();

        if (m_currentIndex >= 0)
        {
            emit currentChannelChanged(m_channels[m_currentIndex]);
        }

        return true;
    }
    catch (const QString &error)
    {
        qWarning() << "Error loading channels:" << error;
        return false;
    }
}

bool ChannelManager::saveToFile(const QString &filePath)
{
    return m_jsonParser.saveToFile(m_channels, filePath);
}

QList<ChannelData> ChannelManager::channels() const
{
    return m_channels;
}

ChannelData ChannelManager::currentChannel() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_channels.size())
    {
        return m_channels[m_currentIndex];
    }

    return ChannelData();
}

int ChannelManager::currentIndex() const
{
    return m_currentIndex;
}

bool ChannelManager::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_channels.size())
    {
        return false;
    }

    if (m_currentIndex != index)
    {
        m_currentIndex = index;
        emit currentChannelChanged(m_channels[m_currentIndex]);
    }

    return true;
}

void ChannelManager::addChannel(const ChannelData &channel)
{
    m_channels.append(channel);

    if (m_currentIndex < 0)
    {
        m_currentIndex = 0;
        emit currentChannelChanged(m_channels[m_currentIndex]);
    }

    emit channelListChanged();
}

bool ChannelManager::removeChannel(int index)
{
    if (index < 0 || index >= m_channels.size())
    {
        return false;
    }

    m_channels.removeAt(index);

    if (m_channels.isEmpty())
    {
        m_currentIndex = -1;
    }
    else if (m_currentIndex >= m_channels.size())
    {
        m_currentIndex = m_channels.size() - 1;
        emit currentChannelChanged(m_channels[m_currentIndex]);
    }
    else if (m_currentIndex == index)
    {
        emit currentChannelChanged(m_channels[m_currentIndex]);
    }

    emit channelListChanged();
    return true;
}

bool ChannelManager::updateChannel(int index, const ChannelData &channel)
{
    if (index < 0 || index >= m_channels.size())
    {
        return false;
    }

    m_channels[index] = channel;

    if (m_currentIndex == index)
    {
        emit currentChannelChanged(m_channels[m_currentIndex]);
    }

    emit channelListChanged();
    return true;
}

int ChannelManager::count() const
{
    return m_channels.size();
}