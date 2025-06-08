#ifndef MPVCORE_H
#define MPVCORE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QOpenGLContext>
#include <mpv/client.h>
#include <mpv/render_gl.h>

/**
 * @brief The MPVCore class wraps libmpv functionality
 */
class MPVCore : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit MPVCore(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MPVCore();

    /**
     * @brief Initialize MPV
     * @return True if successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Initialize the OpenGL renderer
     * @param context OpenGL context
     * @return True if successful, false otherwise
     */
    bool initializeRenderer(QOpenGLContext *context);

    /**
     * @brief Load a file or URL
     * @param path File path or URL
     */
    void loadFile(const QString &path);

    /**
     * @brief Play the current file
     */
    void play();

    /**
     * @brief Pause the current file
     */
    void pause();

    /**
     * @brief Toggle play/pause
     */
    void togglePause();

    /**
     * @brief Stop playback
     */
    void stop();

    /**
     * @brief Seek to a position
     * @param position Position in seconds
     */
    void seek(double position);

    /**
     * @brief Set the volume
     * @param volume Volume level (0-100)
     */
    void setVolume(int volume);

    /**
     * @brief Set a property
     * @param name Property name
     * @param value Property value
     */
    void setProperty(const QString &name, const QVariant &value);

    /**
     * @brief Get a property
     * @param name Property name
     * @return Property value
     */
    QVariant getProperty(const QString &name);

    /**
     * @brief Observe a property for changes
     * @param name Property name
     */
    void observeProperty(const QString &name);

    /**
     * @brief Execute an MPV command
     * @param args Command arguments
     */
    void command(const QVariantList &args);

    /**
     * @brief Render a new frame
     * @param fbo OpenGL framebuffer object
     * @param width Width of the framebuffer
     * @param height Height of the framebuffer
     */
    void renderFrame(unsigned int fbo, int width, int height);

    /**
     * @brief Set up hardware acceleration
     * @param method Hardware acceleration method (auto, vaapi, vdpau, etc.)
     */
    void setupHardwareAcceleration(const QString &method = "auto");

signals:
    /**
     * @brief Signal emitted when a property changes
     * @param name Property name
     * @param value New property value
     */
    void propertyChanged(const QString &name, const QVariant &value);

    /**
     * @brief Signal emitted when a file is loaded
     */
    void fileLoaded();

    /**
     * @brief Signal emitted when playback ends
     */
    void playbackFinished();

    /**
     * @brief Signal emitted when a new frame is available
     */
    void frameSwapped();

    /**
     * @brief Signal emitted when an error occurs
     * @param message Error message
     */
    void error(const QString &message);

private slots:
    /**
     * @brief Handle MPV events
     */
    void handleEvents();

private:
    /**
     * @brief Convert MPV property to QVariant
     * @param prop MPV property
     * @return QVariant value
     */
    QVariant mpvPropertyToVariant(mpv_node prop);

    /**
     * @brief Convert QVariant to MPV node
     * @param value QVariant value
     * @param node MPV node to fill
     * @return True if successful, false otherwise
     */
    bool variantToMpvNode(const QVariant &value, mpv_node *node);

    /**
     * @brief Free MPV node
     * @param node MPV node to free
     */
    void freeMpvNode(mpv_node *node);

    mpv_handle *m_mpv;
    mpv_render_context *m_mpvGL;
};

#endif // MPVCORE_H