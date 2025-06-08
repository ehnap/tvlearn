#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <QString>
#include "mpvcore.h"
#include "playbackcontroller.h"
#include "../data/settings.h"
#include "../data/channeldata.h"

/**
 * @brief The MediaPlayer class is the central controller for media playback
 */
class MediaPlayer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param settings Settings instance
     * @param parent Parent object
     */
    explicit MediaPlayer(Settings *settings, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MediaPlayer();

    /**
     * @brief Initialize the media player
     * @return True if successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get the MPV core instance
     * @return MPV core instance
     */
    MPVCore *mpvCore() const;

    /**
     * @brief Get the playback controller instance
     * @return Playback controller instance
     */
    PlaybackController *playbackController() const;

    /**
     * @brief Load a media file or URL
     * @param path File path or URL
     */
    void loadMedia(const QString &path);

    /**
     * @brief Load a channel
     * @param channel Channel data
     */
    void loadChannel(const ChannelData &channel);

    /**
     * @brief Check if the current media is a network stream
     * @return True if network stream, false if local file
     */
    bool isNetworkStream() const;

    /**
     * @brief Get the current media path
     * @return Current media path
     */
    QString currentMedia() const;

    /**
     * @brief Apply settings to MPV
     */
    void applySettings();

public slots:
    /**
     * @brief Handle MPV errors
     * @param message Error message
     */
    void onMpvError(const QString &message);

    /**
     * @brief Handle settings changes
     */
    void onSettingsChanged();

    /**
     * @brief Handle MPV settings changes
     */
    void onMpvSettingsChanged();

signals:
    /**
     * @brief Signal emitted when media is loaded
     * @param path Media path
     */
    void mediaLoaded(const QString &path);

    /**
     * @brief Signal emitted when an error occurs
     * @param message Error message
     */
    void error(const QString &message);

private:
    /**
     * @brief Check if a path is a network URL
     * @param path Path to check
     * @return True if network URL, false if local file
     */
    bool isNetworkUrl(const QString &path) const;

    MPVCore *m_mpvCore;
    PlaybackController *m_playbackController;
    Settings *m_settings;
    QString m_currentMedia;
    bool m_isNetworkStream;
};

#endif // MEDIAPLAYER_H