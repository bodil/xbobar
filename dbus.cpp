#include "dbus.h"
#include "uuid.h"

namespace DBus {

  ByteBuffer::ByteBuffer(const QByteArray& other, QObject* parent)
    : QObject(parent)
    , array(other)
  {
    data = array.constData();
  }

  const char* const* ByteBuffer::constData() const {
    return &data;
  }

  MethodCall::MethodCall(DBusConnection* connection,
                         const QString& dest,
                         const QString& path,
                         const QString& iface,
                         const QString& method,
                         const QVariantList& args,
                         QObject* parent)
    : QObject(parent)
    , connection(connection)
    , reply(NULL)
  {
    msg = dbus_message_new_method_call(dest.toLocal8Bit().data(),
                                       path.toLocal8Bit().data(),
                                       iface.toLocal8Bit().data(),
                                       method.toLocal8Bit().data());
    marshalArgs(msg, args);
  }

  MethodCall::~MethodCall() {
    dbus_message_unref(msg);
    if (reply) dbus_message_unref(reply);
  }

  void MethodCall::marshalIter(DBusMessageIter* iter, const QVariant& arg) {
    QString t(arg.typeName());
    if (t == "QString") {
      ByteBuffer* s = new ByteBuffer(arg.toString().toLocal8Bit(), this);
      dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, s->constData());
    } else {
      qFatal("DBus::marshalIter: cannot marshal type \'%s\'", qPrintable(t));
    }
  }

  void MethodCall::marshalArgs(DBusMessage* msg, const QVariantList& args) {
    DBusMessageIter iter;
    dbus_message_iter_init_append(msg, &iter);
    QListIterator<QVariant> i(args);
    while (i.hasNext())
      marshalIter(&iter, i.next());
  }

  void handleComplete(DBusPendingCall*, void* userData) {
    MethodCall* me = static_cast<MethodCall*>(userData);
    me->onComplete();
  }

  bool MethodCall::invoke() {
    if (!dbus_connection_send_with_reply(connection, msg, &pending, 1000)) {
      emit done("unable to queue message", QVariant());
      return false;
    }
    dbus_pending_call_set_notify(pending, &handleComplete, this, NULL);
    return true;
  }

  QVariant unmarshalIter(DBusMessageIter*);

  QVariantList unmarshalRecurse(DBusMessageIter* iter) {
    QVariantList out;
    DBusMessageIter sub;
    dbus_message_iter_recurse(iter, &sub);
    do {
      out << unmarshalIter(&sub);
      dbus_message_iter_next(&sub);
    } while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID);
    return out;
  }

  QVariant unmarshalIter(DBusMessageIter* iter) {
    switch (dbus_message_iter_get_arg_type(iter)) {
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH: {
      const char* value;
      dbus_message_iter_get_basic(iter, &value);
      return QString(value);
    }
    case DBUS_TYPE_BOOLEAN: {
      bool value;
      dbus_message_iter_get_basic(iter, &value);
      return QVariant(value);
    }
    case DBUS_TYPE_UINT32: {
      quint32 value;
      dbus_message_iter_get_basic(iter, &value);
      return QVariant(value);
    }
    case DBUS_TYPE_DICT_ENTRY: {
      return unmarshalRecurse(iter);
    }
    case DBUS_TYPE_VARIANT: {
      return unmarshalRecurse(iter)[0];
    }
    case DBUS_TYPE_STRUCT: {
      return unmarshalRecurse(iter);
    }
    case DBUS_TYPE_ARRAY: {
      int elt = dbus_message_iter_get_element_type(iter);
      switch (elt) {
      case DBUS_TYPE_DICT_ENTRY: {
        QVariantMap out;
        DBusMessageIter sub;
        dbus_message_iter_recurse(iter, &sub);
        do {
          QVariantList el(unmarshalIter(&sub).toList());
          out.insert(el[0].toString(), el[1]);
          dbus_message_iter_next(&sub);
        } while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID);
        return out;
      }
      default:
        return unmarshalRecurse(iter);
      }
    }
    default:
      qFatal("DBus::unmarshalIter: unhandled dbus type %c",
             dbus_message_iter_get_arg_type(iter));
    }
  }

  QVariant unmarshal(DBusMessage* m) {
    QVariantList out;
    DBusMessageIter iter;
    dbus_message_iter_init(m, &iter);
    do {
      out << unmarshalIter(&iter);
      dbus_message_iter_next(&iter);
    } while (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_INVALID);
    return out;
  }

  void MethodCall::onComplete() {
    reply = dbus_pending_call_steal_reply(pending);
    if (!reply) qFatal("MethodCall::onComplete: expected reply but was null!");
    switch (dbus_message_get_type(reply)) {
    case DBUS_MESSAGE_TYPE_METHOD_RETURN: {
      emit done(QVariant(), unmarshal(reply));
      break;
    }
    case DBUS_MESSAGE_TYPE_ERROR: {
      DBusError err;
      dbus_error_init(&err);
      dbus_set_error_from_message(&err, reply);
      emit done(QVariantMap({{"name", QString(err.name)},
              {"message", QString(err.message)}}),
        QVariant());
      break;
    }
    default:
      qFatal("DBus::MethodCall::onComplete: unexpected reply message type %d",
             dbus_message_get_type(reply));
    }
    deleteLater();
  }

  Interface::Interface(DBusBusType bus, const QString& name,
                       QObject* parent)
    : QObject(parent)
  {
    setObjectName(name);
    DBusError err;
    dbus_error_init(&err);
    connection = dbus_bus_get(bus, &err);
    if (dbus_error_is_set(&err)) {
      qFatal("DBus::Interface.constructor: error %s: %s", err.name, err.message);
    }
    dbus_error_free(&err);
    startTimer(50);
  }

  Interface::~Interface() {
    dbus_connection_unref(connection);
  }

  void Interface::timerEvent(QTimerEvent*) {
    dbus_connection_read_write_dispatch(connection, 0);
  }

  QObject* Interface::call(const QString& dest,
                           const QString& path,
                           const QString& iface,
                           const QString& method,
                           const QVariantList& args)
  {
    MethodCall* call = new MethodCall(connection, dest, path, iface, method, args, this);
    call->invoke();
    return call;
  }

};
