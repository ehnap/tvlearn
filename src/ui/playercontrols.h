#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/playbackcontroller.h"

/**
 * @brief The PlayerControls class provides UI controls for playback
 */
class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param playbackController Playback controller instance
     * @param parent Parent widget
     */
    explicit PlayerControls(PlaybackController *playbackController, QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PlayerControls();

    /**
     * @brief Set the duration of the current media
     * @param duration Duration in seconds
     */
    void setDuration(double duration);

    /**
     * @brief Set the current position
     * @param position Position in seconds
     */
    void setPosition(double position);

    /**
     * @brief Set the volume
     * @param volume Volume level (0-100)
     */
    void setVolume(int volume);

    /**
     * @brief Set the play/pause state
     * @param playing True if playing, false if paused
     */
    void setPlaying(bool playing);

    /**
     * @brief Set the mute state
     * @param muted True if muted, false if unmuted
     */
    void setMuted(bool muted);

signals:
    /**
     * @brief Signal emitted when the play button is clicked
     */
    void playClicked();

    /**
     * @brief Signal emitted when the pause button is clicked
     */
    void pauseClicked();

    /**
     * @brief Signal emitted when the stop button is clicked
     */
    void stopClicked();

    /**
     * @brief Signal emitted when the position slider is moved
     * @param position New position in seconds
     */
    void positionChanged(double position);

    /**
     * @brief Signal emitted when the volume slider is moved
     * @param volume New volume level (0-100)
     */
    void volumeChanged(int volume);

    /**
     * @brief Signal emitted when the mute button is clicked
     * @param muted True if muted, false if unmuted
     */
    void muteClicked(bool muted);

    /**
     * @brief Signal emitted when the fullscreen button is clicked
     */
    void fullscreenClicked();

private slots:
    /**
     * @brief Handle play/pause button click
     */
    void onPlayPauseClicked();

    /**
     * @brief Handle stop button click
     */
    void onStopClicked();

    /**
     * @brief Handle position slider value change
     * @param value New slider value
     */
    void onPositionSliderValueChanged(int value);

    /**
     * @brief Handle position slider release
     */
    void onPositionSliderReleased();

    /**
     * @brief Handle volume slider value change
     * @param value New slider value
     */
    void onVolumeSliderValueChanged(int value);

    /**
     * @brief Handle mute button click
     */
    void onMuteClicked();

    /**
     * @brief Handle fullscreen button click
     */
    void onFullscreenClicked();

    /**
     * @brief Update time labels
     */
    void updateTimeLabels();

private:
    /**
     * @brief Format time as a string
     * @param seconds Time in seconds
     * @return Formatted time string (HH:MM:SS)
     */
    QString formatTime(double seconds) const;

    PlaybackController *m_playbackController;

    QPushButton *m_playPauseButton;
    QPushButton *m_stopButton;
    QPushButton *m_muteButton;
    QPushButton *m_fullscreenButton;

    QSlider *m_positionSlider;
    QSlider *m_volumeSlider;

    QLabel *m_currentTimeLabel;
    QLabel *m_totalTimeLabel;

    double m_duration;
    double m_position;
    bool m_isPlaying;
    bool m_isMuted;
    bool m_isPositionSliderPressed;
};

#endif // PLAYERCONTROLS_H