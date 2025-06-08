#include "mpvcore.h"
#include <QDebug>
#include <stdexcept>
#include <clocale>

// Helper function to handle MPV errors
static inline void checkError(int status)
{
    if (status < 0)
    {
        qWarning() << "MPV error:" << mpv_error_string(status);
        // Don't throw an exception, just log the error
    }
}

// Static callback for MPV render context
static void on_mpv_events(void *ctx)
{
    MPVCore *core = static_cast<MPVCore *>(ctx);
    QMetaObject::invokeMethod(core, "handleEvents", Qt::QueuedConnection);
}

// Static callback for MPV render context update
static void on_mpv_render_update(void *ctx)
{
    MPVCore *core = static_cast<MPVCore *>(ctx);
    emit core->frameSwapped();
}

MPVCore::MPVCore(QObject *parent)
    : QObject(parent), m_mpv(nullptr), m_mpvGL(nullptr)
{
    // Set C locale to ensure consistent number formatting
    std::setlocale(LC_NUMERIC, "C");
}

MPVCore::~MPVCore()
{
    if (m_mpvGL)
    {
        mpv_render_context_free(m_mpvGL);
        m_mpvGL = nullptr;
    }

    if (m_mpv)
    {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

bool MPVCore::initialize()
{
    m_mpv = mpv_create();
    if (!m_mpv)
    {
        qWarning() << "Failed to create MPV instance";
        return false;
    }

    // Set default options
    mpv_set_option_string(m_mpv, "video-sync", "display-resample");
    mpv_set_option_string(m_mpv, "hwdec", "auto");
    mpv_set_option_string(m_mpv, "vo", "gpu");
    mpv_set_option_string(m_mpv, "gpu-api", "auto");
    mpv_set_option_string(m_mpv, "keep-open", "yes");

    // Enable message handling
    int result = mpv_request_log_messages(m_mpv, "warn");
    if (result < 0)
    {
        qWarning() << "Failed to set log messages:" << mpv_error_string(result);
    }

    // Initialize MPV
    result = mpv_initialize(m_mpv);
    if (result < 0)
    {
        qWarning() << "Failed to initialize MPV:" << mpv_error_string(result);
        return false;
    }

    // Set up event handling
    mpv_set_wakeup_callback(m_mpv, on_mpv_events, this);

    // Observe properties
    observeProperty("time-pos");
    observeProperty("duration");
    observeProperty("pause");
    observeProperty("volume");
    observeProperty("eof-reached");

    return true;
}

bool MPVCore::initializeRenderer(QOpenGLContext *context)
{
    if (!m_mpv)
    {
        qWarning() << "MPV not initialized";
        return false;
    }

    if (m_mpvGL)
    {
        mpv_render_context_free(m_mpvGL);
        m_mpvGL = nullptr;
    }

    // Make sure the context is current
    if (!context->isValid())
    {
        qWarning() << "OpenGL context is not valid";
        return false;
    }

    // Initialize with standard struct initialization for compatibility
    mpv_opengl_init_params gl_init_params;
    gl_init_params.get_proc_address = [](void *ctx, const char *name) -> void *
    {
        QOpenGLContext *glctx = static_cast<QOpenGLContext *>(ctx);
        if (!glctx)
            return nullptr;

        // Try to get the function address
        void *addr = reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));

        // Debug output to help diagnose issues
        if (!addr)
        {
            qDebug() << "Failed to get OpenGL function:" << name;
        }

        return addr;
    };
    gl_init_params.get_proc_address_ctx = context;

    // Set additional parameters for better compatibility
    int advanced_control = 1;

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advanced_control},
        {MPV_RENDER_PARAM_INVALID, nullptr}};

    int result = mpv_render_context_create(&m_mpvGL, m_mpv, params);
    if (result < 0)
    {
        qWarning() << "Failed to initialize MPV GL:" << mpv_error_string(result);

        // Try to continue without OpenGL rendering
        // This allows the application to run with limited functionality
        emit error(QString("OpenGL rendering not available: %1").arg(mpv_error_string(result)));
        return false;
    }

    mpv_render_context_set_update_callback(m_mpvGL, on_mpv_render_update, this);

    return true;
}

void MPVCore::loadFile(const QString &path)
{
    if (!m_mpv)
    {
        qWarning() << "MPV not initialized";
        return;
    }

    const QByteArray pathUtf8 = path.toUtf8();
    const char *args[] = {"loadfile", pathUtf8.constData(), nullptr};

    // Use mpv_command_string for compatibility
    int error = mpv_command_string(m_mpv, QString("loadfile \"%1\"").arg(path).toUtf8().constData());
    if (error < 0)
    {
        qWarning() << "Failed to load file:" << mpv_error_string(error);
    }
}

void MPVCore::play()
{
    setProperty("pause", false);
}

void MPVCore::pause()
{
    setProperty("pause", true);
}

void MPVCore::togglePause()
{
    QVariant paused = getProperty("pause");
    setProperty("pause", !paused.toBool());
}

void MPVCore::stop()
{
    const char *args[] = {"stop", nullptr};
    // Use mpv_command_string for compatibility
    int error = mpv_command_string(m_mpv, "stop");
    if (error < 0)
    {
        qWarning() << "Failed to stop playback:" << mpv_error_string(error);
    }
}

void MPVCore::seek(double position)
{
    if (!m_mpv)
    {
        return;
    }

    char posStr[64];
    snprintf(posStr, sizeof(posStr), "%.2f", position);

    // Use mpv_set_property for seeking
    int error = mpv_set_property(m_mpv, "time-pos", MPV_FORMAT_DOUBLE, &position);
    if (error < 0)
    {
        qWarning() << "Failed to seek:" << mpv_error_string(error);
    }
}

void MPVCore::setVolume(int volume)
{
    setProperty("volume", volume);
}

void MPVCore::setProperty(const QString &name, const QVariant &value)
{
    if (!m_mpv)
    {
        return;
    }

    mpv_node node;
    if (!variantToMpvNode(value, &node))
    {
        qWarning() << "Failed to convert value for property:" << name;
        return;
    }

    int result = mpv_set_property_async(m_mpv, 0, name.toUtf8().constData(), MPV_FORMAT_NODE, &node);
    freeMpvNode(&node);

    if (result < 0)
    {
        qWarning() << "Failed to set property:" << name << "error:" << mpv_error_string(result);
    }
}

QVariant MPVCore::getProperty(const QString &name)
{
    if (!m_mpv)
    {
        qDebug() << "MPV not initialized when getting property:" << name;
        return QVariant();
    }

    mpv_node node;
    int result = mpv_get_property(m_mpv, name.toUtf8().constData(), MPV_FORMAT_NODE, &node);
    if (result < 0)
    {
        // Don't log warnings for properties that are expected to be unavailable initially
        if (result == MPV_ERROR_PROPERTY_UNAVAILABLE &&
            (name == "duration" || name == "time-pos"))
        {
            qDebug() << "Property not yet available:" << name;
        }
        else
        {
            qWarning() << "Failed to get property:" << name << "error:" << mpv_error_string(result);
        }
        return QVariant();
    }

    QVariant value = mpvPropertyToVariant(node);
    mpv_free_node_contents(&node);

    return value;
}

void MPVCore::observeProperty(const QString &name)
{
    if (!m_mpv)
    {
        return;
    }

    mpv_observe_property(m_mpv, 0, name.toUtf8().constData(), MPV_FORMAT_NODE);
}

void MPVCore::command(const QVariantList &args)
{
    if (!m_mpv || args.isEmpty())
    {
        return;
    }

    QVector<QByteArray> byteArrays;
    byteArrays.reserve(args.size());

    QVector<const char *> mpvArgs;
    mpvArgs.reserve(args.size() + 1);

    for (const QVariant &arg : args)
    {
        byteArrays.append(arg.toString().toUtf8());
        mpvArgs.append(byteArrays.last().constData());
    }

    mpvArgs.append(nullptr);

    // Use mpv_command_string for compatibility
    QString cmdStr;
    for (int i = 0; i < byteArrays.size(); i++)
    {
        if (i > 0)
            cmdStr += " ";
        QString arg = QString::fromUtf8(byteArrays[i]);
        // Simple escaping for spaces
        if (arg.contains(' '))
        {
            cmdStr += "\"" + arg + "\"";
        }
        else
        {
            cmdStr += arg;
        }
    }

    int error = mpv_command_string(m_mpv, cmdStr.toUtf8().constData());
    if (error < 0)
    {
        qWarning() << "Failed to execute command:" << mpv_error_string(error);
    }
}

void MPVCore::renderFrame(unsigned int fbo, int width, int height)
{
    if (!m_mpvGL)
    {
        // Just return if OpenGL rendering is not available
        return;
    }

    try
    {
        // Initialize with standard struct initialization for compatibility
        mpv_opengl_fbo mpfbo;
        mpfbo.fbo = static_cast<int>(fbo);
        mpfbo.w = width;
        mpfbo.h = height;
        mpfbo.internal_format = 0;

        int flip_y{1};
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
            {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
            {MPV_RENDER_PARAM_INVALID, nullptr}};

        int result = mpv_render_context_render(m_mpvGL, params);
        if (result < 0)
        {
            qWarning() << "Error rendering frame:" << mpv_error_string(result);
        }
    }
    catch (const std::exception &e)
    {
        qWarning() << "Exception in renderFrame:" << e.what();
    }
    catch (...)
    {
        qWarning() << "Unknown exception in renderFrame";
    }
}

void MPVCore::setupHardwareAcceleration(const QString &method)
{
    if (!m_mpv)
    {
        return;
    }

    setProperty("hwdec", method);
}

void MPVCore::handleEvents()
{
    if (!m_mpv)
    {
        return;
    }

    try
    {
        while (m_mpv)
        {
            mpv_event *event = mpv_wait_event(m_mpv, 0);
            if (!event || event->event_id == MPV_EVENT_NONE)
            {
                break;
            }

            switch (event->event_id)
            {
            case MPV_EVENT_PROPERTY_CHANGE:
            {
                mpv_event_property *prop = static_cast<mpv_event_property *>(event->data);
                if (prop && prop->format == MPV_FORMAT_NODE)
                {
                    QVariant value = mpvPropertyToVariant(*static_cast<mpv_node *>(prop->data));
                    emit propertyChanged(QString::fromUtf8(prop->name), value);

                    // Check for end of file
                    if (QString::fromUtf8(prop->name) == "eof-reached" && value.toBool())
                    {
                        emit playbackFinished();
                    }
                }
                break;
            }

            case MPV_EVENT_FILE_LOADED:
                emit fileLoaded();
                break;

            case MPV_EVENT_LOG_MESSAGE:
            {
                mpv_event_log_message *msg = static_cast<mpv_event_log_message *>(event->data);
                if (msg)
                {
                    qDebug() << "MPV [" << msg->prefix << "] " << msg->level << ": " << msg->text;

                    // Check if this is an error message
                    QString level = QString::fromUtf8(msg->level);
                    if (level == "error")
                    {
                        emit error(QString::fromUtf8(msg->text));
                    }
                }
                break;
            }

            case MPV_EVENT_COMMAND_REPLY:
            {
                if (event->error < 0)
                {
                    emit error(QString::fromUtf8(mpv_error_string(event->error)));
                }
                break;
            }

            default:
                break;
            }
        }
    }
    catch (const std::exception &e)
    {
        qWarning() << "Exception in handleEvents:" << e.what();
    }
    catch (...)
    {
        qWarning() << "Unknown exception in handleEvents";
    }
}

QVariant MPVCore::mpvPropertyToVariant(mpv_node prop)
{
    switch (prop.format)
    {
    case MPV_FORMAT_STRING:
        return QString::fromUtf8(prop.u.string);
    case MPV_FORMAT_FLAG:
        return QVariant(static_cast<bool>(prop.u.flag));
    case MPV_FORMAT_INT64:
        return QVariant(static_cast<qlonglong>(prop.u.int64));
    case MPV_FORMAT_DOUBLE:
        return QVariant(prop.u.double_);
    case MPV_FORMAT_NODE_ARRAY:
    {
        mpv_node_list *list = prop.u.list;
        QVariantList variantList;
        for (int i = 0; i < list->num; i++)
        {
            variantList.append(mpvPropertyToVariant(list->values[i]));
        }
        return variantList;
    }
    case MPV_FORMAT_NODE_MAP:
    {
        mpv_node_list *list = prop.u.list;
        QVariantMap variantMap;
        for (int i = 0; i < list->num; i++)
        {
            variantMap.insert(QString::fromUtf8(list->keys[i]),
                              mpvPropertyToVariant(list->values[i]));
        }
        return variantMap;
    }
    default:
        return QVariant();
    }
}

bool MPVCore::variantToMpvNode(const QVariant &value, mpv_node *node)
{
    // Use QMetaType instead of deprecated QVariant::type()
    int metaType = QMetaType(value.userType()).id();

    if (metaType == QMetaType::Bool)
    {
        node->format = MPV_FORMAT_FLAG;
        node->u.flag = value.toBool();
    }
    else if (metaType == QMetaType::Int || metaType == QMetaType::LongLong)
    {
        node->format = MPV_FORMAT_INT64;
        node->u.int64 = value.toLongLong();
    }
    else if (metaType == QMetaType::Double)
    {
        node->format = MPV_FORMAT_DOUBLE;
        node->u.double_ = value.toDouble();
    }
    else if (metaType == QMetaType::QString)
    {
        node->format = MPV_FORMAT_STRING;
        // Replace mpv_strdup with standard C++ memory allocation
        QByteArray ba = value.toString().toUtf8();
        node->u.string = static_cast<char *>(malloc(ba.size() + 1));
        if (node->u.string)
        {
            memcpy(node->u.string, ba.constData(), ba.size() + 1);
        }
    }
    else if (metaType == QMetaType::QVariantList)
    {
        QVariantList list = value.toList();
        node->format = MPV_FORMAT_NODE_ARRAY;

        mpv_node_list *node_list = new mpv_node_list;
        node_list->num = static_cast<int>(list.size());
        node_list->values = new mpv_node[list.size()];
        node_list->keys = nullptr;
        node->u.list = node_list;

        for (int i = 0; i < list.size(); i++)
        {
            if (!variantToMpvNode(list[i], &node->u.list->values[i]))
            {
                freeMpvNode(node);
                return false;
            }
        }
    }
    else if (metaType == QMetaType::QVariantMap)
    {
        QVariantMap map = value.toMap();
        node->format = MPV_FORMAT_NODE_MAP;

        mpv_node_list *node_list = new mpv_node_list;
        node_list->num = static_cast<int>(map.size());
        node_list->values = new mpv_node[map.size()];
        node_list->keys = new char *[map.size()];
        node->u.list = node_list;

        int i = 0;
        for (auto it = map.constBegin(); it != map.constEnd(); ++it, ++i)
        {
            // Replace mpv_strdup with standard C++ memory allocation
            QByteArray ba = it.key().toUtf8();
            node->u.list->keys[i] = static_cast<char *>(malloc(ba.size() + 1));
            if (node->u.list->keys[i])
            {
                memcpy(node->u.list->keys[i], ba.constData(), ba.size() + 1);
            }
            if (!variantToMpvNode(it.value(), &node->u.list->values[i]))
            {
                freeMpvNode(node);
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

void MPVCore::freeMpvNode(mpv_node *node)
{
    switch (node->format)
    {
    case MPV_FORMAT_STRING:
        free(node->u.string);
        break;
    case MPV_FORMAT_NODE_ARRAY:
    case MPV_FORMAT_NODE_MAP:
    {
        mpv_node_list *list = node->u.list;
        if (list)
        {
            for (int i = 0; i < list->num; i++)
            {
                if (node->format == MPV_FORMAT_NODE_MAP)
                {
                    free(list->keys[i]);
                }
                freeMpvNode(&list->values[i]);
            }
            delete[] list->values;
            if (node->format == MPV_FORMAT_NODE_MAP)
            {
                delete[] list->keys;
            }
            delete list;
        }
        break;
    }
    default:
        break;
    }

    node->format = MPV_FORMAT_NONE;
}