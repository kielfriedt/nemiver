//Author: Fabien Parent
/*
 *This file is part of the Nemiver project
 *
 *Nemiver is free software; you can redistribute
 *it and/or modify it under the terms of
 *the GNU General Public License as published by the
 *Free Software Foundation; either version 2,
 *or (at your option) any later version.
 *
 *Nemiver is distributed in the hope that it will
 *be useful, but WITHOUT ANY WARRANTY;
 *without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *See the GNU General Public License for more details.
 *
 *You should have received a copy of the
 *GNU General Public License along with Nemiver;
 *see the file COPYING.
 *If not, write to the Free Software Foundation,
 *Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *See COPYRIGHT file copyright information.
 */

#include "common/nmv-namespace.h"
#include "common/nmv-safe-ptr.h"
#include "common/nmv-exception.h"

#include <glibmm.h>
#include <giomm.h>
#include <glib/gi18n.h>

#include <iostream>

using nemiver::common::SafePtr;

NEMIVER_BEGIN_NAMESPACE (nemiver)

static const char *const NMV_BUS_NAME = "org.gnome.nemiver.profiler";
static const char *const NMV_DBUS_PROFILER_SERVER_INTROSPECTION_DATA =
    "<node>"
    "  <interface name='org.gnome.nemiver.profilers'>"
    "    <method name='AttachToPID'>"
    "        <arg type='i' name='pid' direction='in' />"
    "        <arg type='u' name='cookie' direction='in' />"
    "        <arg type='as' name='arguments' direction='in' />"
    "        <arg type='s' name='data_filepath' direction='out' />"
    "    </method>"
    "  </interface>"
    "</node>";

class PerfServer {

    PerfServer (const PerfServer &);
    PerfServer& operator= (const PerfServer &);

    struct Priv;
    SafePtr<Priv> m_priv;

public:

    PerfServer ();
    ~PerfServer ();
}; // end namespace PerfServer

struct PerfServer::Priv {
//    PolkitSubject *subject;
    unsigned int bus_id;
    unsigned int registration_id;
    Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data;
    Gio::DBus::InterfaceVTable interface_vtable;

    Priv () :
//        subject (polkit_system_bus_name_new ("org.gnome.nemiver")),
        bus_id (0),
        registration_id (0),
        introspection_data (0),
        interface_vtable
            (sigc::mem_fun (*this, &PerfServer::Priv::on_new_request))
    {
        init ();
    }

    void
    on_new_request (const Glib::RefPtr<Gio::DBus::Connection>&,
                    const Glib::ustring&,
                    const Glib::ustring&,
                    const Glib::ustring&,
                    const Glib::ustring &a_request_name,
                    const Glib::VariantContainerBase &a_parameters,
                    const Glib::RefPtr<Gio::DBus::MethodInvocation>
                        &a_invocation)
    {
        NEMIVER_TRY;

        THROW_IF_FAIL (a_invocation);

        if(a_request_name == "AttachToPID")
        {
            Glib::Variant<int> pid_param;
            a_parameters.get_child (pid_param);

            std::cout << "PID: " << pid_param.get () << std::endl;
        }
        else
        {
            Gio::DBus::Error error
                (Gio::DBus::Error::UNKNOWN_METHOD, _("Invalid request"));
            a_invocation->return_error (error);
        }

        NEMIVER_CATCH_NOX
    }

    void
    init ()
    {
        introspection_data = Gio::DBus::NodeInfo::create_for_xml
            (NMV_DBUS_PROFILER_SERVER_INTROSPECTION_DATA);

        bus_id = Gio::DBus::own_name
            (Gio::DBus::BUS_TYPE_SESSION,
             NMV_BUS_NAME,
             sigc::mem_fun(*this, &PerfServer::Priv::on_bus_acquired),
             sigc::mem_fun(*this, &PerfServer::Priv::on_name_acquired),
             sigc::mem_fun(*this, &PerfServer::Priv::on_name_lost));
    }

    void
    on_bus_acquired(const Glib::RefPtr<Gio::DBus::Connection> &a_connection,
                    const Glib::ustring&)
    {
        NEMIVER_TRY;

        THROW_IF_FAIL (a_connection);
        registration_id = a_connection->register_object
            ("/org/gnome/nemiver/profiler",
             introspection_data->lookup_interface(),
             interface_vtable);

        NEMIVER_CATCH_NOX;
    }

    void
    on_name_acquired (const Glib::RefPtr<Gio::DBus::Connection>&,
                      const Glib::ustring&)
    {
    }

    void
    on_name_lost (const Glib::RefPtr<Gio::DBus::Connection> &a_connection,
                  const Glib::ustring&)
    {
        NEMIVER_TRY;

        THROW_IF_FAIL (a_connection);
        a_connection->unregister_object (registration_id);

        NEMIVER_CATCH_NOX;
    }
};

PerfServer::PerfServer () :
    m_priv (new Priv)
{
}

PerfServer::~PerfServer ()
{
}

NEMIVER_END_NAMESPACE (nemiver)

int main(int, char**)
{
    NEMIVER_TRY;

    Glib::init ();
    Gio::init ();

    nemiver::PerfServer server;

    Glib::RefPtr<Glib::MainLoop> loop = Glib::MainLoop::create ();
    THROW_IF_FAIL (loop);
    loop->run ();

    NEMIVER_CATCH_NOX;

    return 0;
}
