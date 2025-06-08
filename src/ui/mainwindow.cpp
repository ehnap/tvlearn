#include "mainwindow.h"
#include <QDebug>
#include <QInputDialog>
#include <QCloseEvent>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_settings(nullptr), m_channelManager(nullptr), m_mediaPlayer(nullptr), m_videoWidget(nullptr), m_playerControls(nullptr), m_channelSelector(nullptr), m_mainToolBar(nullptr), m_isFullscreen(false)
{
    setWindowTitle("HarperTV");
    setMinimumSize(800, 600);

    // Create settings
    m_settings = new Settings(this);

    // Create channel manager
    m_channelManager = new ChannelManager(this);

    // Create media player
    m_mediaPlayer = new MediaPlayer(m_settings, this);

    // Create actions, menus, and toolbar
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();

    // Connect signals
    connect(m_mediaPlayer, &MediaPlayer::error, this, &MainWindow::onMediaPlayerError);

    // Restore window state
    restoreWindowState();
}

MainWindow::~MainWindow()
{
    saveWindowState();
}

bool MainWindow::initialize()
{
    // Initialize media player
    if (!m_mediaPlayer->initialize())
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to initialize media player."));
        return false;
    }

    // Create central widget (after media player is initialized)
    createCentralWidget();

    // Load channels
    loadChannels();

    return true;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        if (m_isFullscreen)
        {
            onToggleFullscreen();
        }
        break;

    case Qt::Key_F:
        onToggleFullscreen();
        break;

    case Qt::Key_Space:
        m_mediaPlayer->playbackController()->togglePlayPause();
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowState();
    event->accept();
}

void MainWindow::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Media File"),
        QString(),
        tr("Media Files (*.mp4 *.mkv *.avi *.mov *.flv *.wmv *.webm *.mp3 *.wav);;All Files (*.*)"));

    if (!filePath.isEmpty())
    {
        m_mediaPlayer->loadMedia(filePath);
    }
}

void MainWindow::onOpenURL()
{
    bool ok;
    QString url = QInputDialog::getText(
        this,
        tr("Open URL"),
        tr("Enter URL:"),
        QLineEdit::Normal,
        QString(),
        &ok);

    if (ok && !url.isEmpty())
    {
        m_mediaPlayer->loadMedia(url);
    }
}

void MainWindow::onShowSettings()
{
    SettingsDialog dialog(m_settings, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        // Reload channels if channels file changed
        loadChannels();
    }
}

void MainWindow::onToggleFullscreen()
{
    if (m_isFullscreen)
    {
        // Exit fullscreen
        menuBar()->show();
        statusBar()->show();
        if (m_mainToolBar)
        {
            m_mainToolBar->show();
        }
        setGeometry(m_normalGeometry);
        m_isFullscreen = false;
    }
    else
    {
        // Enter fullscreen
        m_normalGeometry = geometry();
        menuBar()->hide();
        statusBar()->hide();
        if (m_mainToolBar)
        {
            m_mainToolBar->hide();
        }
        showFullScreen();
        m_isFullscreen = true;
    }
}

void MainWindow::onChannelSelected(const ChannelData &channel)
{
    m_mediaPlayer->loadChannel(channel);
    statusBar()->showMessage(tr("Loading channel: %1").arg(channel.name()), 3000);
}

void MainWindow::onMediaPlayerError(const QString &message)
{
    QMessageBox::warning(this, tr("Media Player Error"), message);
}

void MainWindow::onVideoDoubleClick()
{
    onToggleFullscreen();
}

void MainWindow::onFullscreenButtonClick()
{
    onToggleFullscreen();
}

void MainWindow::createActions()
{
    // File menu actions
    m_openFileAction = new QAction(tr("&Open File..."), this);
    m_openFileAction->setShortcut(QKeySequence::Open);
    m_openFileAction->setStatusTip(tr("Open a media file"));
    connect(m_openFileAction, &QAction::triggered, this, &MainWindow::onOpenFile);

    m_openURLAction = new QAction(tr("Open &URL..."), this);
    m_openURLAction->setShortcut(QKeySequence("Ctrl+U"));
    m_openURLAction->setStatusTip(tr("Open a media URL"));
    connect(m_openURLAction, &QAction::triggered, this, &MainWindow::onOpenURL);

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Exit the application"));
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    // View menu actions
    m_fullscreenAction = new QAction(tr("&Fullscreen"), this);
    m_fullscreenAction->setShortcut(QKeySequence("F"));
    m_fullscreenAction->setStatusTip(tr("Toggle fullscreen mode"));
    connect(m_fullscreenAction, &QAction::triggered, this, &MainWindow::onToggleFullscreen);

    // Tools menu actions
    m_settingsAction = new QAction(tr("&Settings..."), this);
    m_settingsAction->setStatusTip(tr("Configure application settings"));
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onShowSettings);

    // Help menu actions
    m_aboutAction = new QAction(tr("&About"), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAction, &QAction::triggered, [this]()
            { QMessageBox::about(this, tr("About HarperTV"),
                                 tr("<h2>HarperTV 1.0</h2>"
                                    "<p>A Qt6-based media player with MPV integration.</p>"
                                    "<p>Built with Qt %1 and libmpv.</p>")
                                     .arg(QT_VERSION_STR)); });
}

void MainWindow::createMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_openFileAction);
    fileMenu->addAction(m_openURLAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_fullscreenAction);

    // Tools menu
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(m_settingsAction);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBar()
{
    // Store toolbar reference explicitly instead of using toolBar()
    m_mainToolBar = new QToolBar(tr("Main"), this);
    m_mainToolBar->setObjectName("mainToolBar");
    addToolBar(m_mainToolBar);
    m_mainToolBar->addAction(m_openFileAction);
    m_mainToolBar->addAction(m_openURLAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_fullscreenAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_settingsAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createCentralWidget()
{
    // Create video widget
    m_videoWidget = new VideoWidget(m_mediaPlayer->mpvCore(), this);
    connect(m_videoWidget, &VideoWidget::doubleClicked, this, &MainWindow::onVideoDoubleClick);

    // Create player controls
    m_playerControls = new PlayerControls(m_mediaPlayer->playbackController(), this);
    connect(m_playerControls, &PlayerControls::fullscreenClicked, this, &MainWindow::onFullscreenButtonClick);

    // Create channel selector
    m_channelSelector = new ChannelSelector(m_channelManager, this);
    connect(m_channelSelector, &ChannelSelector::channelSelected, this, &MainWindow::onChannelSelected);

    // Create layouts
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_channelSelector);
    topLayout->addStretch();

    layout->addLayout(topLayout);
    layout->addWidget(m_videoWidget, 1);
    layout->addWidget(m_playerControls);

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::loadChannels()
{
    QString channelsFile = m_settings->value("channelsFile", ":/default_channels.json").toString();
    if (!m_channelManager->loadFromFile(channelsFile))
    {
        QMessageBox::warning(
            this,
            tr("Error"),
            tr("Failed to load channels from %1. Using default channels.").arg(channelsFile));

        // Try to load default channels
        if (!m_channelManager->loadFromFile(":/default_channels.json"))
        {
            QMessageBox::critical(
                this,
                tr("Error"),
                tr("Failed to load default channels."));
        }
    }
}

void MainWindow::saveWindowState()
{
    m_settings->setValue("window/geometry", saveGeometry());
    m_settings->setValue("window/state", saveState());
    m_settings->setValue("window/isFullscreen", m_isFullscreen);
}

void MainWindow::restoreWindowState()
{
    if (m_settings->value("window/geometry").isValid())
    {
        restoreGeometry(m_settings->value("window/geometry").toByteArray());
    }

    if (m_settings->value("window/state").isValid())
    {
        restoreState(m_settings->value("window/state").toByteArray());
    }

    if (m_settings->value("window/isFullscreen", false).toBool())
    {
        onToggleFullscreen();
    }
}