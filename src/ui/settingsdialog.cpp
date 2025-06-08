#include "settingsdialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(500, 400);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createGeneralTab(), tr("General"));
    m_tabWidget->addTab(createVideoTab(), tr("Video"));
    m_tabWidget->addTab(createAudioTab(), tr("Audio"));
    m_tabWidget->addTab(createNetworkTab(), tr("Network"));

    // Create buttons
    m_okButton = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_resetButton = new QPushButton(tr("Reset to Defaults"), this);

    // Create button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);

    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // Connect signals
    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::onAccepted);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onRejected);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetDefaults);

    // Load settings
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
}

QWidget *SettingsDialog::createGeneralTab()
{
    QWidget *widget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(widget);

    // Channels file
    QHBoxLayout *channelsFileLayout = new QHBoxLayout();
    m_channelsFileEdit = new QLineEdit(widget);
    QPushButton *browseButton = new QPushButton(tr("Browse..."), widget);
    channelsFileLayout->addWidget(m_channelsFileEdit);
    channelsFileLayout->addWidget(browseButton);

    layout->addRow(tr("Channels File:"), channelsFileLayout);

    // Connect signals
    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseChannelsFile);

    return widget;
}

QWidget *SettingsDialog::createVideoTab()
{
    QWidget *widget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(widget);

    // Video output
    m_videoOutputCombo = new QComboBox(widget);
    m_videoOutputCombo->addItem(tr("Auto"), "auto");
    m_videoOutputCombo->addItem(tr("OpenGL"), "opengl");
    m_videoOutputCombo->addItem(tr("Direct3D"), "direct3d");
    m_videoOutputCombo->addItem(tr("Vulkan"), "vulkan");
    layout->addRow(tr("Video Output:"), m_videoOutputCombo);

    // Hardware decoding
    m_hwdecCombo = new QComboBox(widget);
    m_hwdecCombo->addItem(tr("Auto"), "auto");
    m_hwdecCombo->addItem(tr("Disabled"), "no");
    m_hwdecCombo->addItem(tr("VAAPI"), "vaapi");
    m_hwdecCombo->addItem(tr("VDPAU"), "vdpau");
    m_hwdecCombo->addItem(tr("DXVA2"), "dxva2");
    m_hwdecCombo->addItem(tr("D3D11VA"), "d3d11va");
    m_hwdecCombo->addItem(tr("VideoToolbox"), "videotoolbox");
    layout->addRow(tr("Hardware Decoding:"), m_hwdecCombo);

    // Keep aspect ratio
    m_keepAspectCheck = new QCheckBox(tr("Maintain aspect ratio"), widget);
    layout->addRow("", m_keepAspectCheck);

    return widget;
}

QWidget *SettingsDialog::createAudioTab()
{
    QWidget *widget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(widget);

    // Audio channels
    m_audioChannelsCombo = new QComboBox(widget);
    m_audioChannelsCombo->addItem(tr("Auto"), "auto");
    m_audioChannelsCombo->addItem(tr("Stereo"), "stereo");
    m_audioChannelsCombo->addItem(tr("5.1"), "5.1");
    m_audioChannelsCombo->addItem(tr("7.1"), "7.1");
    layout->addRow(tr("Audio Channels:"), m_audioChannelsCombo);

    // Audio device
    m_audioDeviceCombo = new QComboBox(widget);
    m_audioDeviceCombo->addItem(tr("Auto"), "auto");
    // Add system-specific audio devices here
    layout->addRow(tr("Audio Device:"), m_audioDeviceCombo);

    return widget;
}

QWidget *SettingsDialog::createNetworkTab()
{
    QWidget *widget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(widget);

    // Cache
    m_cacheCheck = new QCheckBox(tr("Enable cache"), widget);
    layout->addRow("", m_cacheCheck);

    // Cache seconds
    m_cacheSecsSpinBox = new QSpinBox(widget);
    m_cacheSecsSpinBox->setRange(1, 600);
    m_cacheSecsSpinBox->setSuffix(tr(" seconds"));
    layout->addRow(tr("Cache Duration:"), m_cacheSecsSpinBox);

    // Network timeout
    m_networkTimeoutSpinBox = new QSpinBox(widget);
    m_networkTimeoutSpinBox->setRange(1, 60);
    m_networkTimeoutSpinBox->setSuffix(tr(" seconds"));
    layout->addRow(tr("Network Timeout:"), m_networkTimeoutSpinBox);

    // User agent
    m_userAgentEdit = new QLineEdit(widget);
    layout->addRow(tr("User Agent:"), m_userAgentEdit);

    return widget;
}

void SettingsDialog::loadSettings()
{
    // General settings
    m_channelsFileEdit->setText(m_settings->value("channelsFile").toString());

    // Video settings
    QString vo = m_settings->mpvValue("vo", "gpu").toString();
    int voIndex = m_videoOutputCombo->findData(vo);
    if (voIndex >= 0)
    {
        m_videoOutputCombo->setCurrentIndex(voIndex);
    }

    QString hwdec = m_settings->mpvValue("hwdec", "auto").toString();
    int hwdecIndex = m_hwdecCombo->findData(hwdec);
    if (hwdecIndex >= 0)
    {
        m_hwdecCombo->setCurrentIndex(hwdecIndex);
    }

    m_keepAspectCheck->setChecked(m_settings->mpvValue("keepaspect", true).toBool());

    // Audio settings
    QString audioChannels = m_settings->mpvValue("audio-channels", "auto").toString();
    int audioChannelsIndex = m_audioChannelsCombo->findData(audioChannels);
    if (audioChannelsIndex >= 0)
    {
        m_audioChannelsCombo->setCurrentIndex(audioChannelsIndex);
    }

    QString audioDevice = m_settings->mpvValue("audio-device", "auto").toString();
    int audioDeviceIndex = m_audioDeviceCombo->findData(audioDevice);
    if (audioDeviceIndex >= 0)
    {
        m_audioDeviceCombo->setCurrentIndex(audioDeviceIndex);
    }

    // Network settings
    m_cacheCheck->setChecked(m_settings->mpvValue("cache", true).toBool());
    m_cacheSecsSpinBox->setValue(m_settings->mpvValue("cache-secs", 10).toInt());
    m_networkTimeoutSpinBox->setValue(m_settings->mpvValue("network-timeout", 5).toInt());
    m_userAgentEdit->setText(m_settings->mpvValue("user-agent", "HarperTV/1.0").toString());
}

void SettingsDialog::saveSettings()
{
    // General settings
    m_settings->setValue("channelsFile", m_channelsFileEdit->text());

    // Video settings
    m_settings->setMpvValue("vo", m_videoOutputCombo->currentData());
    m_settings->setMpvValue("hwdec", m_hwdecCombo->currentData());
    m_settings->setMpvValue("keepaspect", m_keepAspectCheck->isChecked());

    // Audio settings
    m_settings->setMpvValue("audio-channels", m_audioChannelsCombo->currentData());
    m_settings->setMpvValue("audio-device", m_audioDeviceCombo->currentData());

    // Network settings
    m_settings->setMpvValue("cache", m_cacheCheck->isChecked());
    m_settings->setMpvValue("cache-secs", m_cacheSecsSpinBox->value());
    m_settings->setMpvValue("network-timeout", m_networkTimeoutSpinBox->value());
    m_settings->setMpvValue("user-agent", m_userAgentEdit->text());

    // Save settings
    m_settings->save();
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::onRejected()
{
    reject();
}

void SettingsDialog::onResetDefaults()
{
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        tr("Reset Settings"),
        tr("Are you sure you want to reset all settings to defaults?"),
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        m_settings->resetToDefaults();
        loadSettings();
    }
}

void SettingsDialog::onBrowseChannelsFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Select Channels File"),
        QString(),
        tr("JSON Files (*.json);;All Files (*.*)"));

    if (!filePath.isEmpty())
    {
        m_channelsFileEdit->setText(filePath);
    }
}