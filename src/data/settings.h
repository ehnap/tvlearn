#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMap>
#include <QString>
#include <QVariant>

/**
 * @brief The Settings class manages application and MPV settings
 */
class Settings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit Settings(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Settings();

    /**
     * @brief Load settings from storage
     */
    void load();

    /**
     * @brief Save settings to storage
     */
    void save();

    /**
     * @brief Get an application setting value
     * @param key Setting key
     * @param defaultValue Default value if setting doesn't exist
     * @return Setting value
     */
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    /**
     * @brief Set an application setting value
     * @param key Setting key
     * @param value Setting value
     */
    void setValue(const QString &key, const QVariant &value);

    /**
     * @brief Get an MPV setting value
     * @param key MPV setting key
     * @param defaultValue Default value if setting doesn't exist
     * @return MPV setting value
     */
    QVariant mpvValue(const QString &key, const QVariant &defaultValue = QVariant()) const;

    /**
     * @brief Set an MPV setting value
     * @param key MPV setting key
     * @param value MPV setting value
     */
    void setMpvValue(const QString &key, const QVariant &value);

    /**
     * @brief Get all MPV settings
     * @return Map of all MPV settings
     */
    QMap<QString, QVariant> allMpvSettings() const;

    /**
     * @brief Reset all settings to defaults
     */
    void resetToDefaults();

signals:
    /**
     * @brief Signal emitted when settings are changed
     */
    void settingsChanged();

    /**
     * @brief Signal emitted when MPV settings are changed
     */
    void mpvSettingsChanged();

private:
    /**
     * @brief Initialize default settings
     */
    void initDefaults();

    QSettings *m_appSettings;
    QMap<QString, QVariant> m_mpvSettings;
};

#endif // SETTINGS_H