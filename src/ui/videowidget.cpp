#include "videowidget.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QDebug>

VideoWidget::GLWidget::GLWidget(VideoWidget *videoWidget)
    : QOpenGLWidget(videoWidget), m_videoWidget(videoWidget), m_fbo(nullptr)
{
    // Set OpenGL format
    QSurfaceFormat format;
    format.setDepthBufferSize(0);
    format.setStencilBufferSize(0);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(format);

    // Set attributes
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

VideoWidget::GLWidget::~GLWidget()
{
    makeCurrent();
    delete m_fbo;
    doneCurrent();
}

QOpenGLFramebufferObject *VideoWidget::GLWidget::fbo() const
{
    return m_fbo;
}

void VideoWidget::GLWidget::initializeGL()
{
    qDebug() << "GLWidget::initializeGL() - Starting OpenGL initialization";

    // Initialize OpenGL functions
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context)
    {
        qWarning() << "No OpenGL context available in initializeGL";
        return;
    }

    qDebug() << "GLWidget::initializeGL() - OpenGL context created:"
             << "valid=" << context->isValid()
             << "version=" << context->format().majorVersion() << "." << context->format().minorVersion()
             << "profile=" << (context->format().profile() == QSurfaceFormat::CoreProfile ? "Core" : (context->format().profile() == QSurfaceFormat::CompatibilityProfile ? "Compatibility" : "None"));

    QOpenGLFunctions *f = context->functions();
    if (!f)
    {
        qWarning() << "Could not obtain OpenGL functions";
        return;
    }

    qDebug() << "GLWidget::initializeGL() - OpenGL functions obtained";
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Create framebuffer object with error handling
    try
    {
        qDebug() << "GLWidget::initializeGL() - Creating FBO with size" << size();
        m_fbo = new QOpenGLFramebufferObject(size(), QOpenGLFramebufferObject::CombinedDepthStencil);

        if (!m_fbo->isValid())
        {
            qWarning() << "Created FBO is not valid";
            delete m_fbo;
            m_fbo = nullptr;
        }
        else
        {
            qDebug() << "GLWidget::initializeGL() - FBO created successfully with handle" << m_fbo->handle();
        }
    }
    catch (const std::exception &e)
    {
        qWarning() << "Exception creating FBO:" << e.what();
        m_fbo = nullptr;
    }
    catch (...)
    {
        qWarning() << "Unknown exception creating FBO";
        m_fbo = nullptr;
    }

    qDebug() << "GLWidget::initializeGL() - OpenGL initialization completed, FBO valid:" << (m_fbo != nullptr && m_fbo->isValid());
}

void VideoWidget::GLWidget::paintGL()
{
    static int frameCount = 0;
    if (frameCount++ % 60 == 0)
    { // Log every 60 frames to avoid flooding
        qDebug() << "GLWidget::paintGL() - Frame" << frameCount;
    }

    // Clear the background
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context)
    {
        qWarning() << "GLWidget::paintGL() - No OpenGL context available";
        return;
    }

    QOpenGLFunctions *f = context->functions();
    if (!f)
    {
        qWarning() << "GLWidget::paintGL() - Could not obtain OpenGL functions";
        return;
    }

    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the video frame
    try
    {
        m_videoWidget->renderFrame();
    }
    catch (const std::exception &e)
    {
        qWarning() << "Error in paintGL:" << e.what();
    }
    catch (...)
    {
        qWarning() << "Unknown error in paintGL";
    }
}

void VideoWidget::GLWidget::resizeGL(int width, int height)
{
    qDebug() << "GLWidget::resizeGL() - Resizing to" << width << "x" << height;

    // Recreate framebuffer object with error handling
    try
    {
        makeCurrent();
        qDebug() << "GLWidget::resizeGL() - Made context current";

        if (m_fbo)
        {
            qDebug() << "GLWidget::resizeGL() - Deleting old FBO";
            delete m_fbo;
            m_fbo = nullptr;
        }

        qDebug() << "GLWidget::resizeGL() - Creating new FBO";
        m_fbo = new QOpenGLFramebufferObject(QSize(width, height), QOpenGLFramebufferObject::CombinedDepthStencil);

        if (!m_fbo->isValid())
        {
            qWarning() << "Created FBO is not valid in resizeGL";
            delete m_fbo;
            m_fbo = nullptr;
        }
        else
        {
            qDebug() << "GLWidget::resizeGL() - FBO created successfully with handle" << m_fbo->handle();
        }

        doneCurrent();
        qDebug() << "GLWidget::resizeGL() - Resize completed, FBO valid:" << (m_fbo != nullptr && m_fbo->isValid());
    }
    catch (const std::exception &e)
    {
        qWarning() << "Exception creating FBO in resizeGL:" << e.what();
        if (QOpenGLContext::currentContext())
        {
            doneCurrent();
        }
    }
    catch (...)
    {
        qWarning() << "Unknown exception creating FBO in resizeGL";
        if (QOpenGLContext::currentContext())
        {
            doneCurrent();
        }
    }
}

VideoWidget::VideoWidget(MPVCore *mpvCore, QWidget *parent)
    : QWidget(parent), m_mpvCore(mpvCore), m_glWidget(new GLWidget(this)), m_keepAspect(true)
{
    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_glWidget);
    setLayout(layout);

    // Set focus policy
    setFocusPolicy(Qt::StrongFocus);

    // Connect signals if MPV core is available
    if (m_mpvCore)
    {
        connect(m_mpvCore, &MPVCore::frameSwapped, this, &VideoWidget::onFrameSwapped);
    }

    // Set up update timer
    m_updateTimer.setInterval(16); // ~60 FPS
    connect(&m_updateTimer, &QTimer::timeout, this, &VideoWidget::update);
    m_updateTimer.start();

    // Initialize OpenGL
    initializeGL();
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::setKeepAspect(bool keepAspect)
{
    m_keepAspect = keepAspect;
    update();
}

bool VideoWidget::keepAspect() const
{
    return m_keepAspect;
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Update MPV with new size
    if (m_mpvCore)
    {
        m_mpvCore->setProperty("video-unscaled", m_keepAspect);
        if (m_keepAspect)
        {
            m_mpvCore->setProperty("keepaspect", true);
        }
    }
}

void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit doubleClicked();
    }

    QWidget::mouseDoubleClickEvent(event);
}

void VideoWidget::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event->key());
    QWidget::keyPressEvent(event);
}

void VideoWidget::onFrameSwapped()
{
    // Update the widget when a new frame is available
    m_glWidget->update();
}

void VideoWidget::update()
{
    // Update the widget periodically
    m_glWidget->update();
}

void VideoWidget::initializeGL()
{
    qDebug() << "VideoWidget::initializeGL() - Starting OpenGL initialization";

    // Initialize MPV renderer
    if (!m_mpvCore)
    {
        qWarning() << "MPV core not initialized";
        return;
    }

    try
    {
        // Check if the GL widget has a valid context
        QOpenGLContext *context = m_glWidget->context();
        if (!context)
        {
            qWarning() << "VideoWidget::initializeGL() - No OpenGL context available";
            return;
        }

        qDebug() << "VideoWidget::initializeGL() - GL context exists, attempting to make current";

        // Safely make the context current
        // QOpenGLWidget inherits from QWidget which implements QSurface
        if (!m_glWidget)
        {
            qWarning() << "VideoWidget::initializeGL() - No surface available for context";
            return;
        }

        try
        {
            qDebug() << "VideoWidget::initializeGL() - Making GL context current";
            // Use m_glWidget->makeCurrent() instead of context->makeCurrent(surface)
            m_glWidget->makeCurrent();
            if (!context->isValid())
            {
                qWarning() << "VideoWidget::initializeGL() - Failed to make OpenGL context current";
                return;
            }
            qDebug() << "VideoWidget::initializeGL() - GL context made current successfully";
        }
        catch (const std::exception &e)
        {
            qWarning() << "Exception making context current:" << e.what();
            return;
        }
        catch (...)
        {
            qWarning() << "Unknown exception making context current";
            return;
        }

        qDebug() << "VideoWidget::initializeGL() - GL context valid:" << context->isValid();
        qDebug() << "VideoWidget::initializeGL() - GL context format:" << context->format().majorVersion()
                 << "." << context->format().minorVersion();

        // Try to initialize the renderer, but don't fail if it doesn't work
        // This allows the application to run even if video rendering isn't available
        qDebug() << "VideoWidget::initializeGL() - Initializing MPV renderer";
        bool rendererInitialized = false;

        try
        {
            rendererInitialized = m_mpvCore->initializeRenderer(context);
            qDebug() << "VideoWidget::initializeGL() - MPV renderer initialized:" << rendererInitialized;
        }
        catch (const std::exception &e)
        {
            qWarning() << "Exception initializing MPV renderer:" << e.what();
        }
        catch (...)
        {
            qWarning() << "Unknown exception initializing MPV renderer";
        }

        if (!rendererInitialized)
        {
            qWarning() << "Failed to initialize MPV renderer - video playback may not work";
        }

        // Safely release the context
        try
        {
            context->doneCurrent();
            qDebug() << "VideoWidget::initializeGL() - OpenGL context released successfully";
        }
        catch (const std::exception &e)
        {
            qWarning() << "Exception releasing context:" << e.what();
        }
        catch (...)
        {
            qWarning() << "Unknown exception releasing context";
        }

        qDebug() << "VideoWidget::initializeGL() - OpenGL initialization completed";
    }
    catch (const std::exception &e)
    {
        qWarning() << "Exception in VideoWidget::initializeGL():" << e.what();
        QOpenGLContext *currentContext = QOpenGLContext::currentContext();
        if (currentContext)
        {
            try
            {
                currentContext->doneCurrent();
            }
            catch (...)
            {
                qWarning() << "Exception while releasing context in error handler";
            }
        }
    }
    catch (...)
    {
        qWarning() << "Unknown exception in VideoWidget::initializeGL()";
        QOpenGLContext *currentContext = QOpenGLContext::currentContext();
        if (currentContext)
        {
            try
            {
                currentContext->doneCurrent();
            }
            catch (...)
            {
                qWarning() << "Exception while releasing context in error handler";
            }
        }
    }
}

void VideoWidget::renderFrame()
{
    static int frameCount = 0;
    if (frameCount++ % 60 == 0)
    { // Log every 60 frames to avoid flooding
        qDebug() << "VideoWidget::renderFrame() - Frame" << frameCount;
    }

    // Get current OpenGL context and functions
    QOpenGLContext *context = QOpenGLContext::currentContext();
    QOpenGLFunctions *f = context ? context->functions() : nullptr;

    // Just clear the frame if we can't render video
    if (!m_mpvCore)
    {
        qDebug() << "VideoWidget::renderFrame() - No MPV core available";
        if (f)
        {
            f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            f->glClear(GL_COLOR_BUFFER_BIT);
        }
        return;
    }

    if (!m_glWidget->fbo())
    {
        qDebug() << "VideoWidget::renderFrame() - No FBO available";
        if (f)
        {
            f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            f->glClear(GL_COLOR_BUFFER_BIT);
        }
        return;
    }

    if (!f)
    {
        qDebug() << "VideoWidget::renderFrame() - No OpenGL functions available";
        return;
    }

    // Check if FBO is valid before using it
    QOpenGLFramebufferObject *fbo = m_glWidget->fbo();
    if (!fbo || !fbo->isValid())
    {
        qDebug() << "VideoWidget::renderFrame() - Invalid FBO, skipping frame render";
        if (f)
        {
            f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            f->glClear(GL_COLOR_BUFFER_BIT);
        }
        return;
    }

    try
    {
        // Render the frame to the framebuffer object
        if (frameCount % 60 == 0)
        { // Log every 60 frames
            qDebug() << "VideoWidget::renderFrame() - Rendering frame to FBO"
                     << fbo->handle() << "size:" << m_glWidget->width() << "x" << m_glWidget->height();
        }
        m_mpvCore->renderFrame(fbo->handle(), m_glWidget->width(), m_glWidget->height());
    }
    catch (const std::exception &e)
    {
        // Catch any exceptions during rendering
        qWarning() << "Error rendering frame:" << e.what();
        if (f)
        {
            f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            f->glClear(GL_COLOR_BUFFER_BIT);
        }
    }
    catch (...)
    {
        qWarning() << "Unknown error rendering frame";
        if (f)
        {
            f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            f->glClear(GL_COLOR_BUFFER_BIT);
        }
    }
}