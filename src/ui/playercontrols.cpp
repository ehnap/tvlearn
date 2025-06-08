#include "playercontrols.h"
#include <QDebug>
#include <QStyle>
#include <QTime>

PlayerControls::PlayerControls(PlaybackController *playbackController, QWidget *parent)
    : QWidget(parent), m_playbackController(playbackController), m_duration(0.0), m_position(0.0), m_isPlaying(false), m_isMuted(false), m_isPositionSliderPressed(false)
{
    // Create buttons
    m_playPauseButton = new QPushButton(this);
    m_playPauseButton->setIcon(QIcon(":/icons/play.png"));
    m_playPauseButton->setToolTip(tr("Play"));
    m_playPauseButton->setIconSize(QSize(24, 24));
    m_playPauseButton->setFlat(true);

    m_stopButton = new QPushButton(this);
    m_stopButton->setIcon(QIcon(":/icons/stop.png"));
    m_stopButton->setToolTip(tr("Stop"));
    m_stopButton->setIconSize(QSize(24, 24));
    m_stopButton->setFlat(true);

    m_muteButton = new QPushButton(this);
    m_muteButton->setIcon(QIcon(":/icons/volume.png"));
    m_muteButton->setToolTip(tr("Mute"));
    m_muteButton->setIconSize(QSize(24, 24));
    m_muteButton->setFlat(true);

    m_fullscreenButton = new QPushButton(this);
    m_fullscreenButton->setIcon(QIcon(":/icons/fullscreen.png"));
    m_fullscreenButton->setToolTip(tr("Fullscreen"));
    m_fullscreenButton->setIconSize(QSize(24, 24));
    m_fullscreenButton->setFlat(true);

    // Create sliders
    m_positionSlider = new QSlider(Qt::Horizontal, this);
    m_positionSlider->setRange(0, 1000);
    m_positionSlider->setToolTip(tr("Position"));

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(100);
    m_volumeSlider->setToolTip(tr("Volume"));
    m_volumeSlider->setMaximumWidth(100);

    // Create labels
    m_currentTimeLabel = new QLabel("00:00:00", this);
    m_totalTimeLabel = new QLabel("00:00:00", this);

    // Create layouts
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(m_playPauseButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addWidget(m_currentTimeLabel);
    controlLayout->addWidget(m_positionSlider);
    controlLayout->addWidget(m_totalTimeLabel);
    controlLayout->addWidget(m_muteButton);
    controlLayout->addWidget(m_volumeSlider);
    controlLayout->addWidget(m_fullscreenButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);

    // Connect signals
    connect(m_playPauseButton, &QPushButton::clicked, this, &PlayerControls::onPlayPauseClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &PlayerControls::onStopClicked);
    connect(m_muteButton, &QPushButton::clicked, this, &PlayerControls::onMuteClicked);
    connect(m_fullscreenButton, &QPushButton::clicked, this, &PlayerControls::onFullscreenClicked);

    connect(m_positionSlider, &QSlider::valueChanged, this, &PlayerControls::onPositionSliderValueChanged);
    connect(m_positionSlider, &QSlider::sliderReleased, this, &PlayerControls::onPositionSliderReleased);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayerControls::onVolumeSliderValueChanged);

    // Connect playback controller signals
    connect(m_playbackController, &PlaybackController::playbackStateChanged, this, &PlayerControls::setPlaying);
    connect(m_playbackController, &PlaybackController::durationChanged, this, &PlayerControls::setDuration);
    connect(m_playbackController, &PlaybackController::positionChanged, this, &PlayerControls::setPosition);
    connect(m_playbackController, &PlaybackController::volumeChanged, this, &PlayerControls::setVolume);
    connect(m_playbackController, &PlaybackController::muteChanged, this, &PlayerControls::setMuted);

    // Initialize controls
    setPlaying(m_playbackController->isPlaying());
    setDuration(m_playbackController->duration());
    setPosition(m_playbackController->position());
    setVolume(m_playbackController->volume());
}

PlayerControls::~PlayerControls()
{
}

void PlayerControls::setDuration(double duration)
{
    m_duration = duration;
    updateTimeLabels();
}

void PlayerControls::setPosition(double position)
{
    if (!m_isPositionSliderPressed)
    {
        m_position = position;

        // Update position slider
        if (m_duration > 0)
        {
            int sliderValue = static_cast<int>((position / m_duration) * 1000);
            m_positionSlider->setValue(sliderValue);
        }
        else
        {
            m_positionSlider->setValue(0);
        }

        updateTimeLabels();
    }
}

void PlayerControls::setVolume(int volume)
{
    m_volumeSlider->setValue(volume);
}

void PlayerControls::setPlaying(bool playing)
{
    m_isPlaying = playing;

    if (playing)
    {
        m_playPauseButton->setIcon(QIcon(":/icons/pause.png"));
        m_playPauseButton->setToolTip(tr("Pause"));
    }
    else
    {
        m_playPauseButton->setIcon(QIcon(":/icons/play.png"));
        m_playPauseButton->setToolTip(tr("Play"));
    }
}

void PlayerControls::setMuted(bool muted)
{
    m_isMuted = muted;

    if (muted)
    {
        m_muteButton->setIcon(QIcon(":/icons/mute.png"));
        m_muteButton->setToolTip(tr("Unmute"));
    }
    else
    {
        m_muteButton->setIcon(QIcon(":/icons/volume.png"));
        m_muteButton->setToolTip(tr("Mute"));
    }
}

void PlayerControls::onPlayPauseClicked()
{
    if (m_isPlaying)
    {
        m_playbackController->pause();
        emit pauseClicked();
    }
    else
    {
        m_playbackController->play();
        emit playClicked();
    }
}

void PlayerControls::onStopClicked()
{
    m_playbackController->stop();
    emit stopClicked();
}

void PlayerControls::onPositionSliderValueChanged(int value)
{
    if (m_positionSlider->isSliderDown())
    {
        m_isPositionSliderPressed = true;

        // Update position
        if (m_duration > 0)
        {
            m_position = (value / 1000.0) * m_duration;
            updateTimeLabels();
        }
    }
}

void PlayerControls::onPositionSliderReleased()
{
    m_isPositionSliderPressed = false;

    // Set position
    if (m_duration > 0)
    {
        double position = (m_positionSlider->value() / 1000.0) * m_duration;
        m_playbackController->setPosition(position);
        emit positionChanged(position);
    }
}

void PlayerControls::onVolumeSliderValueChanged(int value)
{
    m_playbackController->setVolume(value);
    emit volumeChanged(value);
}

void PlayerControls::onMuteClicked()
{
    m_playbackController->toggleMute();
    emit muteClicked(!m_isMuted);
}

void PlayerControls::onFullscreenClicked()
{
    emit fullscreenClicked();
}

void PlayerControls::updateTimeLabels()
{
    m_currentTimeLabel->setText(formatTime(m_position));
    m_totalTimeLabel->setText(formatTime(m_duration));
}

QString PlayerControls::formatTime(double seconds) const
{
    int totalSeconds = static_cast<int>(seconds);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs = totalSeconds % 60;

    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));
}