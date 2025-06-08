#include "playbackcontroller.h"
#include <QDebug>

PlaybackController::PlaybackController(MPVCore *mpvCore, QObject *parent)
    : QObject(parent), m_mpvCore(mpvCore), m_isPlaying(false), m_duration(0.0), m_position(0.0), m_volume(100), m_isMuted(false), m_lastVolume(100)
{
    // Connect MPV property change signals
    connect(m_mpvCore, &MPVCore::propertyChanged, this, &PlaybackController::onPropertyChanged);
    connect(m_mpvCore, &MPVCore::playbackFinished, this, &PlaybackController::onPlaybackFinished);

    // Initialize properties from MPV with safe defaults
    QVariant pauseVar = m_mpvCore->getProperty("pause");
    m_isPlaying = pauseVar.isValid() ? !pauseVar.toBool() : false;

    // These properties may not be available until media is loaded
    QVariant durationVar = m_mpvCore->getProperty("duration");
    m_duration = durationVar.isValid() ? durationVar.toDouble() : 0.0;

    QVariant positionVar = m_mpvCore->getProperty("time-pos");
    m_position = positionVar.isValid() ? positionVar.toDouble() : 0.0;

    QVariant volumeVar = m_mpvCore->getProperty("volume");
    m_volume = volumeVar.isValid() ? volumeVar.toInt() : 100;

    QVariant muteVar = m_mpvCore->getProperty("mute");
    m_isMuted = muteVar.isValid() ? muteVar.toBool() : false;

    qDebug() << "PlaybackController initialized with defaults:"
             << "playing=" << m_isPlaying
             << "duration=" << m_duration
             << "position=" << m_position;
}

bool PlaybackController::isPlaying() const
{
    return m_isPlaying;
}

double PlaybackController::duration() const
{
    return m_duration;
}

double PlaybackController::position() const
{
    return m_position;
}

void PlaybackController::setPosition(double position)
{
    if (position != m_position)
    {
        m_mpvCore->seek(position);
        // The position will be updated through the property change signal
    }
}

int PlaybackController::volume() const
{
    return m_volume;
}

void PlaybackController::setVolume(int volume)
{
    if (volume != m_volume)
    {
        m_volume = qBound(0, volume, 100);
        m_mpvCore->setVolume(m_volume);
        emit volumeChanged(m_volume);

        // If we're setting volume to non-zero, we're implicitly unmuting
        if (m_volume > 0 && m_isMuted)
        {
            setMute(false);
        }
    }
}

void PlaybackController::play()
{
    m_mpvCore->play();
}

void PlaybackController::pause()
{
    m_mpvCore->pause();
}

void PlaybackController::togglePlayPause()
{
    m_mpvCore->togglePause();
}

void PlaybackController::stop()
{
    m_mpvCore->stop();
}

void PlaybackController::seekForward(double seconds)
{
    double newPosition = m_position + seconds;
    if (m_duration > 0)
    {
        newPosition = qMin(newPosition, m_duration);
    }
    setPosition(newPosition);
}

void PlaybackController::seekBackward(double seconds)
{
    double newPosition = m_position - seconds;
    newPosition = qMax(0.0, newPosition);
    setPosition(newPosition);
}

void PlaybackController::setMute(bool mute)
{
    if (mute != m_isMuted)
    {
        m_isMuted = mute;
        m_mpvCore->setProperty("mute", m_isMuted);
        emit muteChanged(m_isMuted);
    }
}

void PlaybackController::toggleMute()
{
    setMute(!m_isMuted);
}

void PlaybackController::onPropertyChanged(const QString &name, const QVariant &value)
{
    if (name == "time-pos")
    {
        m_position = value.toDouble();
        emit positionChanged(m_position);
    }
    else if (name == "duration")
    {
        m_duration = value.toDouble();
        emit durationChanged(m_duration);
    }
    else if (name == "pause")
    {
        m_isPlaying = !value.toBool();
        emit playbackStateChanged(m_isPlaying);
    }
    else if (name == "volume")
    {
        m_volume = value.toInt();
        emit volumeChanged(m_volume);
    }
    else if (name == "mute")
    {
        m_isMuted = value.toBool();
        emit muteChanged(m_isMuted);
    }
}

void PlaybackController::onPlaybackFinished()
{
    m_isPlaying = false;
    emit playbackStateChanged(m_isPlaying);
    emit playbackFinished();
}