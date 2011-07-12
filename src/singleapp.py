import dbus
import dbus.service
import dbus.mainloop.glib

class Toggler(dbus.service.Object):
    def __init__(self, session_bus, name):
        dbus.service.Object.__init__(self, session_bus, name)
        self.callback = None
 
    @dbus.service.method("net.satas.QSRun", in_signature='', out_signature='')
    def show(self):
        try:
            self.callback()
        except TypeError:
            pass

class OtherInstanceFoundError:
    pass

class SingleApp:
    def __init__(self):
        self._path = '/QSRun'
        self._uri = 'net.satas.QSRun'
        self._toggler = None
        self._window = None
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        self._session_bus = dbus.SessionBus()
        self._sname = None

    def start(self):
        self.check_single_instance()
        self.start_server()

    def check_single_instance(self):
        try:
            remote_object = self._session_bus.get_object(
                    self._uri, self._path)
            iface = dbus.Interface(remote_object, self._uri)
            iface.show()
            raise OtherInstanceFoundError()
        except dbus.DBusException, e:
            pass
     
    def start_server(self):
        self._sname = dbus.service.BusName(self._uri, self._session_bus)
        self._toggler = Toggler(self._session_bus, self._path)

    def set_window(self, window):
        self._toggler.callback = window.show

