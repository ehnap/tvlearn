#include "mediaplayer.h"
#include <QDebug>
#include <QUrl>

MediaPlayer::MediaPlayer(Settings *settings, QObject *parent)
    : QObject(parent), m_mpvCore(nullptr), m_playbackController(nullptr), m_settings(settings), m_currentMedia(""), m_isNetworkStream(false)
{
}

MediaPlayer::~MediaPlayer()
{
    delete m_playbackController;
    delete m_mpvCore;
}

bool MediaPlayer::initialize()
{
    // Create and initialize MPV core
    m_mpvCore = new MPVCore(this);
    if (!m_mpvCore->initialize())
    {
        qWarning() << "Failed to initialize MPV core";
        return false;
    }

    // Create playback controller
    m_playbackController = new PlaybackController(m_mpvCore, this);

    // Connect signals
    connect(m_mpvCore, &MPVCore::error, this, &MediaPlayer::onMpvError);
    connect(m_settings, &Settings::settingsChanged, this, &MediaPlayer::onSettingsChanged);
    connect(m_settings, &Settings::mpvSettingsChanged, this, &MediaPlayer::onMpvSettingsChanged);

    // Apply settings
    applySettings();

    return true;
}

MPVCore *MediaPlayer::mpvCore() const
{
    return m_mpvCore;
}

PlaybackController *MediaPlayer::playbackController() const
{
    return m_playbackController;
}

void MediaPlayer::loadMedia(const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }

    m_currentMedia = path;
    m_isNetworkStream = isNetworkUrl(path);

    // Configure cache for network streams
    if (m_isNetworkStream)
    {
        m_mpvCore->setProperty("cache", true);
        m_mpvCore->setProperty("cache-secs", m_settings->mpvValue("cache-secs", 10).toInt());
    }
    else
    {
        m_mpvCore->setProperty("cache", false);
    }

    // Load the file
    m_mpvCore->loadFile(path);

    emit mediaLoaded(path);
}

void MediaPlayer::loadChannel(const ChannelData &channel)
{
    loadMedia(channel.url());
}

bool MediaPlayer::isNetworkStream() const
{
    return m_isNetworkStream;
}

QString MediaPlayer::currentMedia() const
{
    return m_currentMedia;
}

void MediaPlayer::applySettings()
{
    // Apply volume
    int volume = m_settings->value("volume", 100).toInt();
    m_playbackController->setVolume(volume);

    // Apply MPV settings
    QMap<QString, QVariant> mpvSettings = m_settings->allMpvSettings();
    for (auto it = mpvSettings.constBegin(); it != mpvSettings.constEnd(); ++it)
    {
        m_mpvCore->setProperty(it.key(), it.value());
    }

    // Set up hardware acceleration
    QString hwdec = m_settings->mpvValue("hwdec", "auto").toString();
    m_mpvCore->setupHardwareAcceleration(hwdec);
}

void MediaPlayer::onMpvError(const QString &message)
{
    qWarning() << "MPV error:" << message;
    emit error(message);
}

void MediaPlayer::onSettingsChanged()
{
    // Update volume
    int volume = m_settings->value("volume", 100).toInt();
    m_playbackController->setVolume(volume);
}

void MediaPlayer::onMpvSettingsChanged()
{
    // Apply MPV settings
    QMap<QString, QVariant> mpvSettings = m_settings->allMpvSettings();
    for (auto it = mpvSettings.constBegin(); it != mpvSettings.constEnd(); ++it)
    {
        m_mpvCore->setProperty(it.key(), it.value());
    }

    // Set up hardware acceleration
    QString hwdec = m_settings->mpvValue("hwdec", "auto").toString();
    m_mpvCore->setupHardwareAcceleration(hwdec);
}

bool MediaPlayer::isNetworkUrl(const QString &path) const
{
    QUrl url(path);
    return url.scheme() == "http" || url.scheme() == "https" ||
           url.scheme() == "rtmp" || url.scheme() == "rtsp" ||
           url.scheme() == "mms" || url.scheme() == "rtp";
}