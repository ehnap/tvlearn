#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFramebufferObject>
#include <QTimer>
#include "../core/mpvcore.h"

/**
 * @brief The VideoWidget class renders video content using MPV
 */
class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param mpvCore MPV core instance
     * @param parent Parent widget
     */
    explicit VideoWidget(MPVCore *mpvCore, QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~VideoWidget();

    /**
     * @brief Set the aspect ratio mode
     * @param keepAspect True to maintain aspect ratio, false to stretch
     */
    void setKeepAspect(bool keepAspect);

    /**
     * @brief Check if aspect ratio is maintained
     * @return True if aspect ratio is maintained, false if stretched
     */
    bool keepAspect() const;

protected:
    /**
     * @brief Handle resize events
     * @param event Resize event
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief Handle mouse press events
     * @param event Mouse press event
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handle mouse double click events
     * @param event Mouse double click event
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    /**
     * @brief Handle key press events
     * @param event Key press event
     */
    void keyPressEvent(QKeyEvent *event) override;

signals:
    /**
     * @brief Signal emitted when the widget is clicked
     */
    void clicked();

    /**
     * @brief Signal emitted when the widget is double clicked
     */
    void doubleClicked();

    /**
     * @brief Signal emitted when a key is pressed
     * @param key Key code
     */
    void keyPressed(int key);

private slots:
    /**
     * @brief Handle frame swapped signal from MPV
     */
    void onFrameSwapped();

    /**
     * @brief Update the video widget
     */
    void update();

private:
    /**
     * @brief Initialize the OpenGL widget
     */
    void initializeGL();

    /**
     * @brief Render the current frame
     */
    void renderFrame();

    class GLWidget : public QOpenGLWidget
    {
    public:
        explicit GLWidget(VideoWidget *videoWidget);
        ~GLWidget();

        QOpenGLFramebufferObject *fbo() const;

    protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int width, int height) override;

    private:
        VideoWidget *m_videoWidget;
        QOpenGLFramebufferObject *m_fbo;
    };

    MPVCore *m_mpvCore;
    GLWidget *m_glWidget;
    QTimer m_updateTimer;
    bool m_keepAspect;
};

#endif // VIDEOWIDGET_H