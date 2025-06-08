#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include <QObject>
#include "mpvcore.h"

/**
 * @brief The PlaybackController class manages playback state and controls
 */
class PlaybackController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY playbackStateChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    /**
     * @brief Constructor
     * @param mpvCore MPV core instance
     * @param parent Parent object
     */
    explicit PlaybackController(MPVCore *mpvCore, QObject *parent = nullptr);

    /**
     * @brief Check if playback is active
     * @return True if playing, false if paused or stopped
     */
    bool isPlaying() const;

    /**
     * @brief Get the duration of the current media
     * @return Duration in seconds
     */
    double duration() const;

    /**
     * @brief Get the current playback position
     * @return Position in seconds
     */
    double position() const;

    /**
     * @brief Set the playback position
     * @param position Position in seconds
     */
    void setPosition(double position);

    /**
     * @brief Get the current volume
     * @return Volume level (0-100)
     */
    int volume() const;

    /**
     * @brief Set the volume
     * @param volume Volume level (0-100)
     */
    void setVolume(int volume);

public slots:
    /**
     * @brief Play the current media
     */
    void play();

    /**
     * @brief Pause the current media
     */
    void pause();

    /**
     * @brief Toggle between play and pause
     */
    void togglePlayPause();

    /**
     * @brief Stop playback
     */
    void stop();

    /**
     * @brief Seek forward by a specified amount
     * @param seconds Seconds to seek forward
     */
    void seekForward(double seconds = 10.0);

    /**
     * @brief Seek backward by a specified amount
     * @param seconds Seconds to seek backward
     */
    void seekBackward(double seconds = 10.0);

    /**
     * @brief Mute or unmute audio
     * @param mute True to mute, false to unmute
     */
    void setMute(bool mute);

    /**
     * @brief Toggle mute state
     */
    void toggleMute();

signals:
    /**
     * @brief Signal emitted when playback state changes
     * @param playing True if playing, false if paused or stopped
     */
    void playbackStateChanged(bool playing);

    /**
     * @brief Signal emitted when duration changes
     * @param duration New duration in seconds
     */
    void durationChanged(double duration);

    /**
     * @brief Signal emitted when position changes
     * @param position New position in seconds
     */
    void positionChanged(double position);

    /**
     * @brief Signal emitted when volume changes
     * @param volume New volume level (0-100)
     */
    void volumeChanged(int volume);

    /**
     * @brief Signal emitted when mute state changes
     * @param muted True if muted, false if unmuted
     */
    void muteChanged(bool muted);

    /**
     * @brief Signal emitted when playback ends
     */
    void playbackFinished();

private slots:
    /**
     * @brief Handle MPV property changes
     * @param name Property name
     * @param value Property value
     */
    void onPropertyChanged(const QString &name, const QVariant &value);

    /**
     * @brief Handle playback finished event
     */
    void onPlaybackFinished();

private:
    MPVCore *m_mpvCore;
    bool m_isPlaying;
    double m_duration;
    double m_position;
    int m_volume;
    bool m_isMuted;
    int m_lastVolume;
};

#endif // PLAYBACKCONTROLLER_H