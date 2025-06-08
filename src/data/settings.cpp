#include "settings.h"

Settings::Settings(QObject *parent)
    : QObject(parent), m_appSettings(new QSettings("HarperTV", "HarperTV", this))
{
    initDefaults();
    load();
}

Settings::~Settings()
{
    save();
}

void Settings::load()
{
    // Load application settings
    // Nothing to do here as QSettings loads automatically

    // Load MPV settings
    int size = m_appSettings->beginReadArray("MPVSettings");
    for (int i = 0; i < size; ++i)
    {
        m_appSettings->setArrayIndex(i);
        QString key = m_appSettings->value("key").toString();
        QVariant value = m_appSettings->value("value");
        m_mpvSettings[key] = value;
    }
    m_appSettings->endArray();

    emit settingsChanged();
    emit mpvSettingsChanged();
}

void Settings::save()
{
    // Save MPV settings
    m_appSettings->beginWriteArray("MPVSettings");
    int i = 0;
    for (auto it = m_mpvSettings.constBegin(); it != m_mpvSettings.constEnd(); ++it)
    {
        m_appSettings->setArrayIndex(i++);
        m_appSettings->setValue("key", it.key());
        m_appSettings->setValue("value", it.value());
    }
    m_appSettings->endArray();

    m_appSettings->sync();
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return m_appSettings->value(key, defaultValue);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    m_appSettings->setValue(key, value);
    emit settingsChanged();
}

QVariant Settings::mpvValue(const QString &key, const QVariant &defaultValue) const
{
    return m_mpvSettings.value(key, defaultValue);
}

void Settings::setMpvValue(const QString &key, const QVariant &value)
{
    m_mpvSettings[key] = value;
    emit mpvSettingsChanged();
}

QMap<QString, QVariant> Settings::allMpvSettings() const
{
    return m_mpvSettings;
}

void Settings::resetToDefaults()
{
    m_appSettings->clear();
    m_mpvSettings.clear();
    initDefaults();

    emit settingsChanged();
    emit mpvSettingsChanged();
}

void Settings::initDefaults()
{
    // Application defaults
    if (!m_appSettings->contains("volume"))
    {
        m_appSettings->setValue("volume", 100);
    }

    if (!m_appSettings->contains("lastChannelIndex"))
    {
        m_appSettings->setValue("lastChannelIndex", 0);
    }

    if (!m_appSettings->contains("channelsFile"))
    {
        m_appSettings->setValue("channelsFile", ":/default_channels.json");
    }

    // MPV defaults
    if (m_mpvSettings.isEmpty())
    {
        // Video settings
        m_mpvSettings["vo"] = "gpu";
        m_mpvSettings["hwdec"] = "auto";

        // Audio settings
        m_mpvSettings["audio-channels"] = "auto";
        m_mpvSettings["audio-device"] = "auto";

        // Cache settings
        m_mpvSettings["cache"] = true;
        m_mpvSettings["cache-secs"] = 10;

        // Network settings
        m_mpvSettings["network-timeout"] = 5;
        m_mpvSettings["user-agent"] = "HarperTV/1.0";
    }
}