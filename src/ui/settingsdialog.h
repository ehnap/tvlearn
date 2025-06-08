#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include "../data/settings.h"

/**
 * @brief The SettingsDialog class provides UI for configuring settings
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param settings Settings instance
     * @param parent Parent widget
     */
    explicit SettingsDialog(Settings *settings, QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~SettingsDialog();

private slots:
    /**
     * @brief Save settings and close dialog
     */
    void onAccepted();

    /**
     * @brief Discard changes and close dialog
     */
    void onRejected();

    /**
     * @brief Reset settings to defaults
     */
    void onResetDefaults();

    /**
     * @brief Browse for channels file
     */
    void onBrowseChannelsFile();

private:
    /**
     * @brief Create general settings tab
     * @return General settings widget
     */
    QWidget *createGeneralTab();

    /**
     * @brief Create video settings tab
     * @return Video settings widget
     */
    QWidget *createVideoTab();

    /**
     * @brief Create audio settings tab
     * @return Audio settings widget
     */
    QWidget *createAudioTab();

    /**
     * @brief Create network settings tab
     * @return Network settings widget
     */
    QWidget *createNetworkTab();

    /**
     * @brief Load settings into UI controls
     */
    void loadSettings();

    /**
     * @brief Save settings from UI controls
     */
    void saveSettings();

    Settings *m_settings;
    QTabWidget *m_tabWidget;

    // General settings
    QLineEdit *m_channelsFileEdit;

    // Video settings
    QComboBox *m_videoOutputCombo;
    QComboBox *m_hwdecCombo;
    QCheckBox *m_keepAspectCheck;

    // Audio settings
    QComboBox *m_audioChannelsCombo;
    QComboBox *m_audioDeviceCombo;

    // Network settings
    QCheckBox *m_cacheCheck;
    QSpinBox *m_cacheSecsSpinBox;
    QSpinBox *m_networkTimeoutSpinBox;
    QLineEdit *m_userAgentEdit;

    // Buttons
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_resetButton;
};

#endif // SETTINGSDIALOG_H