#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include "../core/mediaplayer.h"
#include "../core/channelmanager.h"
#include "../data/settings.h"
#include "videowidget.h"
#include "playercontrols.h"
#include "channelselector.h"
#include "settingsdialog.h"

/**
 * @brief The MainWindow class is the main application window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MainWindow();

    /**
     * @brief Initialize the main window
     * @return True if successful, false otherwise
     */
    bool initialize();

protected:
    /**
     * @brief Handle key press events
     * @param event Key press event
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Handle close events
     * @param event Close event
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief Open a media file
     */
    void onOpenFile();

    /**
     * @brief Open a URL
     */
    void onOpenURL();

    /**
     * @brief Show settings dialog
     */
    void onShowSettings();

    /**
     * @brief Toggle fullscreen mode
     */
    void onToggleFullscreen();

    /**
     * @brief Handle channel selection
     * @param channel Selected channel
     */
    void onChannelSelected(const ChannelData &channel);

    /**
     * @brief Handle media player errors
     * @param message Error message
     */
    void onMediaPlayerError(const QString &message);

    /**
     * @brief Handle video widget double click
     */
    void onVideoDoubleClick();

    /**
     * @brief Handle fullscreen button click
     */
    void onFullscreenButtonClick();

private:
    /**
     * @brief Create actions
     */
    void createActions();

    /**
     * @brief Create menus
     */
    void createMenus();

    /**
     * @brief Create toolbar
     */
    void createToolBar();

    /**
     * @brief Create status bar
     */
    void createStatusBar();

    /**
     * @brief Create central widget
     */
    void createCentralWidget();

    /**
     * @brief Load channels
     */
    void loadChannels();

    /**
     * @brief Save window state
     */
    void saveWindowState();

    /**
     * @brief Restore window state
     */
    void restoreWindowState();

    // Core components
    Settings *m_settings;
    ChannelManager *m_channelManager;
    MediaPlayer *m_mediaPlayer;

    // UI components
    VideoWidget *m_videoWidget;
    PlayerControls *m_playerControls;
    ChannelSelector *m_channelSelector;

    // Actions
    QAction *m_openFileAction;
    QAction *m_openURLAction;
    QAction *m_exitAction;
    QAction *m_settingsAction;
    QAction *m_fullscreenAction;
    QAction *m_aboutAction;

    // Toolbar
    QToolBar *m_mainToolBar;

    // State
    bool m_isFullscreen;
    QRect m_normalGeometry;
};

#endif // MAINWINDOW_H