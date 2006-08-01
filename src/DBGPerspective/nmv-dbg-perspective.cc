//Author: Dodji Seketeli
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

#include <gtksourceviewmm/init.h>
#include <gtksourceviewmm/sourcelanguagesmanager.h>
#include "nmv-dbg-perspective.h"
#include "nmv-ui-utils.h"
#include "nmv-env.h"
#include "nmv-run-program-dialog.h"
#include "nmv-ui-utils.h"

using namespace std ;
using namespace nemiver::common ;
using namespace nemiver::ui_utils;
using namespace gtksourceview ;

namespace nemiver {

const char *SET_BREAKPOINT    = "nmv-set-breakpoint" ;
const char *CONTINUE          = "nmv-continue" ;
const char *STOP_DEBUGGER     = "nmv-stop-debugger" ;
const char *RUN_DEBUGGER      = "nmv-run-debugger" ;
const char *LINE_POINTER      = "nmv-line-pointer" ;
const char *RUN_TO_CURSOR     = "nmv-run-to-cursor" ;
const char *STEP_INTO         = "nmv-step-into" ;
const char *STEP_OVER         = "nmv-step-over" ;
const char *STEP_OUT          = "nmv-step-out" ;

const Gtk::StockID STOCK_SET_BREAKPOINT (SET_BREAKPOINT) ;
const Gtk::StockID STOCK_CONTINUE (CONTINUE) ;
const Gtk::StockID STOCK_STOP_DEBUGGER (STOP_DEBUGGER) ;
const Gtk::StockID STOCK_RUN_DEBUGGER (RUN_DEBUGGER) ;
const Gtk::StockID STOCK_LINE_POINTER (LINE_POINTER) ;
const Gtk::StockID STOCK_RUN_TO_CURSOR (RUN_TO_CURSOR) ;
const Gtk::StockID STOCK_STEP_INTO (STEP_INTO) ;
const Gtk::StockID STOCK_STEP_OVER (STEP_OVER) ;
const Gtk::StockID STOCK_STEP_OUT (STEP_OUT) ;

class DBGPerspective : public IDBGPerspective {
    //non copyable
    DBGPerspective (const IPerspective&) ;
    DBGPerspective& operator= (const IPerspective&) ;
    struct Priv ;
    SafePtr<Priv> m_priv ;

private:

    struct SlotedButton : Gtk::Button {
        UString uri ;
        DBGPerspective *perspective ;

        SlotedButton () :
            Gtk::Button (),
            perspective (NULL)
        {}

        SlotedButton (const Gtk::StockID &a_id) :
            Gtk::Button (a_id),
            perspective (NULL)
        {}

        void on_clicked ()
        {
            if (perspective) {
                perspective->close_file (uri) ;
            }
        }

        ~SlotedButton ()
        {
        }
    };

    //************
    //<signal slots>
    //************
    void on_open_action () ;
    void on_close_action () ;
    void on_execute_program_action () ;
    void on_run_action () ;
    void on_next_action () ;
    void on_step_into_action () ;
    void on_step_out_action () ;
    void on_continue_action () ;
    void on_set_breakpoint_action () ;
    void on_unset_breakpoint_action () ;

    void on_switch_page_signal (GtkNotebookPage *a_page, guint a_page_num) ;
    void on_debugger_stdout_signal (IDebugger::CommandAndOutput &a_cao) ;
    void on_debugger_ready_signal (bool a_is_ready) ;

    void on_insert_in_command_view_signal (const Gtk::TextBuffer::iterator &a_iter,
                                           const Glib::ustring &a_text,
                                           int a_dont_know) ;
    bool on_button_pressed_in_source_view_signal (GdkEventButton *a_event) ;
    //************
    //</signal slots>
    //************

    string build_resource_path (const UString &a_dir, const UString &a_name) ;
    void add_stock_icon (const UString &a_stock_id,
                         const UString &icon_dir,
                         const UString &icon_name) ;
    void add_perspective_menu_entries () ;
    void add_perspective_toolbar_entries () ;
    void init_icon_factory () ;
    void init_actions () ;
    void init_toolbar () ;
    void init_body () ;
    void init_signals () ;
    void init_debugger_output_handlers () ;
    void append_source_editor (SourceEditor &a_sv,
                               const Glib::RefPtr<Gnome::Vfs::Uri> &a_uri) ;
    SourceEditor* get_current_source_editor () ;
    UString get_current_file_path () ;
    SourceEditor* get_source_editor_from_uri (const UString &a_uri) ;
    void bring_source_as_current (const UString &a_uri) ;
    int get_n_pages () ;
    void popup_source_view_contextual_menu (GdkEventButton *a_event) ;

public:

    DBGPerspective () ;

    virtual ~DBGPerspective () ;

    void get_info (Info &a_info) const ;

    void do_init () ;

    void do_init (IWorkbenchSafePtr &a_workbench) ;

    const UString& get_perspective_identifier () ;

    void get_toolbars (list<Gtk::Toolbar*> &a_tbs)  ;

    Gtk::Widget* get_body ()  ;

    void edit_workbench_menu () ;

    void open_file () ;

    bool open_file (const UString &a_uri,
                    int current_line=-1) ;

    void close_current_file () ;

    void close_file (const UString &a_uri) ;

    void execute_program () ;

    void execute_program (const UString &a_prog_and_args,
                          const UString &a_cwd=".") ;

    void execute_program (const UString &a_prog,
                          const UString &a_args,
                          const UString &a_cwd=".") ;
    void run () ;
    void step_over () ;
    void step_into () ;
    void step_out () ;
    void do_continue () ;
    void set_breakpoint () ;
    void set_breakpoint (const UString &a_file,
                         int a_line) ;
    void append_breakpoints (map<int, IDebugger::BreakPoint> &a_breaks) ;
    bool get_breakpoint_number (const UString &a_file_name,
                                int a_linenum,
                                int &a_break_num) ;
    bool delete_breakpoint () ;
    bool delete_breakpoint (int a_breakpoint_num) ;
    bool delete_breakpoint (const UString &a_file_uri,
                            int a_linenum) ;
    void append_visual_breakpoint (const UString &a_file_name,
                                   int a_linenum) ;
    void delete_visual_breakpoint (const UString &a_file_name, int a_linenum) ;
    void delete_visual_breakpoint (int a_breaknum) ;

    IDebuggerSafePtr& debugger () ;
    Gtk::TextView* get_command_view () ;
    Gtk::TextView* get_program_output_view () ;
    Gtk::TextView* get_error_view () ;
    void add_text_to_command_view (const UString &a_text) ;
    void add_text_to_program_output_view (const UString &a_text) ;
    void add_text_to_error_view (const UString &a_text) ;
    void set_where (const UString &a_uri, int line) ;
    Gtk::Widget* get_contextual_menu () ;
    sigc::signal<void, bool>& activated_signal () ;
    sigc::signal<void, bool>& debugger_ready_signal () ;
};//end class DBGPerspective

struct RefGObject {
    void operator () (Glib::Object *a_object)
    {
        if (a_object) {a_object->reference ();}
    }
};

struct UnrefGObject {
    void operator () (Glib::Object *a_object)
    {
        if (a_object) {a_object->unreference ();}
    }
};

struct RefGObjectNative {
    void operator () (void *a_object)
    {
        if (a_object && G_IS_OBJECT (a_object)) {
            g_object_ref (G_OBJECT (a_object));
        }
    }
};

struct UnrefGObjectNative {
    void operator () (void *a_object)
    {
        if (a_object && G_IS_OBJECT (a_object)) {
            g_object_unref (G_OBJECT (a_object)) ;
        }
    }
};

/// A container of handlers
/// to handle output coming from
/// the debugger engine
struct OutputHandler : Object {

    //a method supposed to return
    //true if the current handler knows
    //how to handle a given debugger output
    virtual bool can_handle (IDebugger::CommandAndOutput &){return false;}

    //a method supposed to return
    //true if the current handler knows
    //how to handle a given debugger output
    virtual void do_handle (IDebugger::CommandAndOutput &) {}
};//end struct OutputHandler
typedef SafePtr<OutputHandler, ObjectRef, ObjectUnref> OutputHandlerSafePtr ;

struct OnStreamRecordHandler: OutputHandler{
    DBGPerspective *m_persp ;

    OnStreamRecordHandler (DBGPerspective *a_persp) :
        m_persp (a_persp)
    {}

    bool can_handle (IDebugger::CommandAndOutput &a_in)
    {
        if (!a_in.output ().has_out_of_band_record ()) {
            return false;
        }
        return true ;
    }

    void do_handle (IDebugger::CommandAndOutput &a_in)
    {
        THROW_IF_FAIL (m_persp
                       && m_persp->get_command_view ()
                       && m_persp->get_program_output_view ()
                       && m_persp->get_error_view ()) ;

        list<IDebugger::Output::OutOfBandRecord>::const_iterator iter ;
        for (iter = a_in.output ().out_of_band_records ().begin ();
             iter != a_in.output ().out_of_band_records ().end ();
             ++iter) {
            if (iter->has_stream_record ()) {
                if (m_persp->get_command_view ()
                    && iter->stream_record ().debugger_console () != ""){
                    m_persp->add_text_to_command_view
                        (iter->stream_record ().debugger_console () + "\n") ;
                }
                if (m_persp->get_program_output_view ()
                    && iter->stream_record ().target_output () != ""){
                    m_persp->add_text_to_program_output_view
                        (iter->stream_record ().target_output () + "\n") ;
                }
                if (m_persp->get_error_view ()
                    && iter->stream_record ().debugger_log () != ""){
                    m_persp->add_text_to_error_view
                        (iter->stream_record ().debugger_log () + "\n") ;
                }
            }
        }

    }
};//end struct OnStreamRecordHandler

struct OnBreakPointHandler: OutputHandler {
    DBGPerspective* m_dbg_perspective ;

    OnBreakPointHandler (DBGPerspective *a_persp=NULL) :
        m_dbg_perspective (a_persp)
    {}

    bool has_breakpoints_set (IDebugger::CommandAndOutput &a_in)
    {
        if (a_in.output ().has_result_record ()
            && a_in.output ().result_record ().breakpoints ().size ()) {
            return true ;
        }
        return false ;
    }

    void append_breakpoints_to_list (map<int, IDebugger::BreakPoint> &a_breaks)
    {
        THROW_IF_FAIL (m_dbg_perspective) ;
        m_dbg_perspective->append_breakpoints (a_breaks) ;
    }

    bool can_handle (IDebugger::CommandAndOutput &a_in)
    {
        if (!a_in.output ().has_result_record ()) {
            return false;
        }
        return true ;
    }

    void do_handle (IDebugger::CommandAndOutput &a_in)
    {
        THROW_IF_FAIL (m_dbg_perspective) ;

        //if breakpoint where set, put them in cache !
        if (has_breakpoints_set (a_in)) {
            LOG ("breakpoint set detected") ;
            append_breakpoints_to_list
                (a_in.output ().result_record ().breakpoints ()) ;
        }

        if (a_in.output ().has_result_record ()
            && a_in.output ().result_record ().kind ()
               == IDebugger::Output::ResultRecord::DONE
            && a_in.command ().value ().find ("-break-delete")
                != Glib::ustring::npos) {
            LOG ("-break-delete command issued detected") ;
            LOG ("command was: " << a_in.command ().value ()) ;
            UString tmp = a_in.command ().value () ;
            tmp = tmp.erase (0, 13) ;
            if (tmp.size () == 0) {return ;}
            tmp.chomp () ;
            LOG ("bkpoint number as str: '" << tmp << "'") ;
            int bkpt_number = atoi (tmp.c_str ()) ;
            LOG ("bkpoint number as int: " << bkpt_number) ;
            if (bkpt_number) {
                m_dbg_perspective->delete_visual_breakpoint (bkpt_number) ;
                LOG ("visual breakpoint deleted: " << (int)bkpt_number) ;
            }
        }
    }
};//end struct OnBreakPointHandler

struct OnStoppedHandler: OutputHandler {
    DBGPerspective* m_dbg_perspective ;
    IDebugger::Output::OutOfBandRecord m_out_of_band_record ;
    bool m_is_stopped ;

    OnStoppedHandler (DBGPerspective *a_persp) :
        m_dbg_perspective (a_persp),
        m_is_stopped (false)
    {}

    bool can_handle (IDebugger::CommandAndOutput &a_in)
    {
        if (!a_in.output ().has_out_of_band_record ()) {
            return false;
        }
        m_is_stopped = true ;
        list<IDebugger::Output::OutOfBandRecord>::iterator iter ;

        for (iter = a_in.output ().out_of_band_records ().begin () ;
             iter != a_in.output ().out_of_band_records ().end () ;
             ++iter) {
            if (iter->is_stopped () && iter->has_frame ()) {
                m_out_of_band_record = *iter ;
                return true ;
            }
        }
        return false;
    }

    void do_handle (IDebugger::CommandAndOutput &a_in)
    {
        THROW_IF_FAIL (m_dbg_perspective->get_command_view ()
                       && m_is_stopped
                       && m_dbg_perspective) ;
        m_dbg_perspective->debugger_ready_signal ().emit (true) ;
        if (m_out_of_band_record.frame ().function () != ""
            && m_out_of_band_record.frame ().line () == 0) {
            display_warning (UString ("No line info available for symbol '")
                             + m_out_of_band_record.frame ().function ()
                             + "' of library "
                             + m_out_of_band_record.frame ().library ()) ;
        } else if (m_out_of_band_record.frame ().line () > 0
                   && m_out_of_band_record.frame ().file_full_name () != "") {
            m_dbg_perspective->set_where
                (m_out_of_band_record.frame ().file_full_name (),
                 m_out_of_band_record.frame ().line ()) ;
        } else {
            display_error ("Symbol not available") ;
        }
    }
};//end struct OnStoppedHandler

struct DBGPerspective::Priv {
    bool initialized ;
    Glib::RefPtr<Gtk::ActionGroup> debugger_ready_action_group ;
    Glib::RefPtr<Gtk::ActionGroup> debugger_busy_action_group ;
    Glib::RefPtr<Gtk::ActionGroup> default_action_group;
    Glib::RefPtr<Gtk::ActionGroup> opened_file_action_group;
    Glib::RefPtr<Gtk::UIManager> ui_manager ;
    Glib::RefPtr<Gtk::IconFactory> icon_factory ;
    Gtk::UIManager::ui_merge_id menubar_merge_id ;
    Gtk::UIManager::ui_merge_id toolbar_merge_id ;
    Gtk::UIManager::ui_merge_id contextual_menu_merge_id;
    Gtk::Widget *contextual_menu ;
    IWorkbench *workbench ;
    Gtk::Toolbar *toolbar ;
    sigc::signal<void, bool> activated_signal;
    sigc::signal<void, bool> debugger_ready_signal;
    Glib::RefPtr<Gnome::Glade::Xml> body_glade ;
    SafePtr<Gtk::Window> body_window ;
    Glib::RefPtr<Gtk::Paned> body_main_paned ;
    Gtk::Notebook *sourceviews_notebook ;
    map<UString, int> uri_2_pagenum_map ;
    map<int, SourceEditor*> pagenum_2_source_editor_map ;
    map<int, UString> pagenum_2_uri_map ;
    Gtk::Notebook *statuses_notebook ;
    Gtk::TextView *command_view ;
    Gtk::TextView *program_output_view;
    Gtk::TextView *error_view ;
    int current_page_num ;
    IDebuggerSafePtr debugger ;
    list<OutputHandlerSafePtr> output_handlers ;
    map<int, IDebugger::BreakPoint> breakpoints ;

    Priv () :
        initialized (false),
        menubar_merge_id (0),
        toolbar_merge_id (0),
        contextual_menu_merge_id(0),
        contextual_menu (NULL),
        workbench (NULL),
        sourceviews_notebook (NULL),
        statuses_notebook (NULL),
        current_page_num (0)
    {}
};//end struct DBGPerspective::Priv

#ifndef CHECK_P_INIT
#define CHECK_P_INIT THROW_IF_FAIL(m_priv && m_priv->initialized) ;
#endif

//****************************
//<slots>
//***************************
void
DBGPerspective::on_open_action ()
{
    NEMIVER_TRY

    open_file () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_close_action ()
{
    NEMIVER_TRY

    close_current_file () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_execute_program_action ()
{
    NEMIVER_TRY

    execute_program () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_run_action ()
{
    NEMIVER_TRY

    run () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_next_action ()
{
    NEMIVER_TRY

    step_over () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_step_into_action ()
{
    NEMIVER_TRY

    step_into () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_step_out_action ()
{
    NEMIVER_TRY

    step_out () ;

    NEMIVER_CATCH
}

void
DBGPerspective::on_continue_action ()
{
    NEMIVER_TRY
    do_continue () ;
    NEMIVER_CATCH
}

void
DBGPerspective::on_set_breakpoint_action ()
{
    NEMIVER_TRY
    set_breakpoint () ;
    NEMIVER_CATCH
}

void
DBGPerspective::on_unset_breakpoint_action ()
{
    NEMIVER_TRY
    delete_breakpoint () ;
    NEMIVER_CATCH
}

void
DBGPerspective::on_switch_page_signal (GtkNotebookPage *a_page, guint a_page_num)
{
    NEMIVER_TRY
    m_priv->current_page_num = a_page_num;
    NEMIVER_CATCH
}

void
DBGPerspective::on_debugger_stdout_signal (IDebugger::CommandAndOutput &a_cao)
{
    NEMIVER_TRY
    list<OutputHandlerSafePtr>::iterator iter ;
    for (iter = m_priv->output_handlers.begin () ;
         iter != m_priv->output_handlers.end ();
         ++iter)
    {
        if ((*iter)->can_handle (a_cao)) {
            (*iter)->do_handle (a_cao) ;
        }
    }
    add_text_to_command_view ("\n(gdb) ") ;
    NEMIVER_CATCH
}

void
DBGPerspective::on_debugger_ready_signal (bool a_is_ready)
{
    NEMIVER_TRY

    if (a_is_ready) {
        m_priv->debugger_ready_action_group->set_sensitive (true) ;
    } else {
        m_priv->debugger_ready_action_group->set_sensitive (false) ;
    }

    NEMIVER_CATCH
}

void
DBGPerspective::on_insert_in_command_view_signal (const Gtk::TextBuffer::iterator &a_it,
                                                  const Glib::ustring &a_text,
                                                  int a_dont_know)
{
    NEMIVER_TRY
    if (a_text == "") {return;}

    if (a_text == "\n") {
        //get the command that is on the current line
        UString line ;
        Gtk::TextBuffer::iterator iter = a_it, tmp_iter, eol_iter = a_it ;
        for (;;) {
            --iter ;
            if (iter.is_start ()) {break;}
            tmp_iter = iter ;
            if (tmp_iter.get_char () == ')'
                && (--tmp_iter).get_char () == 'b'
                && (--tmp_iter).get_char () == 'd'
                && (--tmp_iter).get_char () == 'g'
                && (--tmp_iter).get_char () == '(') {
                ++ iter ;
                line = iter.get_visible_text (eol_iter) ;
                break ;
            }
        }
        if (!line.empty ()) {
            IDebuggerSafePtr dbg = debugger () ;
            THROW_IF_FAIL (dbg) ;
            dbg->execute_command (IDebugger::Command (line)) ;
        }
    }
    NEMIVER_CATCH
}

bool
DBGPerspective::on_button_pressed_in_source_view_signal (GdkEventButton *a_event)
{
    NEMIVER_TRY

    if (a_event->type != GDK_BUTTON_PRESS) {
        return false ;
    }

    if (a_event->button != 3) {
        return false ;
    }
    popup_source_view_contextual_menu (a_event) ;
    NEMIVER_CATCH
    return true ;
}

//****************************
//</slots>
//***************************

//*******************
//<private methods>
//*******************


string
DBGPerspective::build_resource_path (const UString &a_dir, const UString &a_name)
{
    string relative_path = Glib::build_filename (Glib::locale_from_utf8 (a_dir),
                                                 Glib::locale_from_utf8 (a_name));
    string absolute_path ;
    THROW_IF_FAIL (build_absolute_resource_path
                    (Glib::locale_to_utf8 (relative_path),
                                           absolute_path)) ;
    return absolute_path ;
}


void
DBGPerspective::add_stock_icon (const UString &a_stock_id,
                                const UString &a_icon_dir,
                                const UString &a_icon_name)
{
    if (!m_priv->icon_factory) {
        m_priv->icon_factory = Gtk::IconFactory::create () ;
        m_priv->icon_factory->add_default () ;
    }

    Gtk::StockID stock_id (a_stock_id) ;
    string icon_path = build_resource_path (a_icon_dir, a_icon_name) ;
    Glib::RefPtr<Gdk::Pixbuf> pixbuf= Gdk::Pixbuf::create_from_file (icon_path) ;
    Gtk::IconSet icon_set (pixbuf) ;
    m_priv->icon_factory->add (stock_id, icon_set) ;
}

void
DBGPerspective::add_perspective_menu_entries ()
{
    string relative_path = Glib::build_filename ("menus",
                                                 "menus.xml") ;
    string absolute_path ;
    THROW_IF_FAIL (build_absolute_resource_path
                    (Glib::locale_to_utf8 (relative_path),
                                           absolute_path)) ;

    m_priv->menubar_merge_id =
        m_priv->workbench->get_ui_manager ()->add_ui_from_file
                                        (Glib::locale_to_utf8 (absolute_path)) ;

    relative_path = Glib::build_filename ("menus", "contextualmenu.xml") ;
    THROW_IF_FAIL (build_absolute_resource_path
                    (Glib::locale_to_utf8 (relative_path),
                                           absolute_path)) ;
    m_priv->contextual_menu_merge_id =
        m_priv->workbench->get_ui_manager ()->add_ui_from_file
                                        (Glib::locale_to_utf8 (absolute_path)) ;
}

void
DBGPerspective::add_perspective_toolbar_entries ()
{
    string relative_path = Glib::build_filename ("menus",
                                                 "toolbar.xml") ;
    string absolute_path ;
    THROW_IF_FAIL (build_absolute_resource_path
                    (Glib::locale_to_utf8 (relative_path),
                                           absolute_path)) ;

    m_priv->toolbar_merge_id =
    m_priv->workbench->get_ui_manager ()->add_ui_from_file
                                        (Glib::locale_to_utf8 (absolute_path)) ;
}

void
DBGPerspective::init_icon_factory ()
{
    add_stock_icon (nemiver::SET_BREAKPOINT, "icons", "set-breakpoint.xpm") ;
    add_stock_icon (nemiver::CONTINUE, "icons", "continue.xpm") ;
    add_stock_icon (nemiver::STOP_DEBUGGER, "icons", "stop-debugger.xpm") ;
    add_stock_icon (nemiver::RUN_DEBUGGER, "icons", "run-debugger.xpm") ;
    add_stock_icon (nemiver::LINE_POINTER, "icons", "line-pointer.xpm") ;
    add_stock_icon (nemiver::RUN_TO_CURSOR, "icons", "run-to-cursor.xpm") ;
    add_stock_icon (nemiver::STEP_INTO, "icons", "step-into.xpm") ;
    add_stock_icon (nemiver::STEP_OVER, "icons", "step-over.xpm") ;
    add_stock_icon (nemiver::STEP_OUT, "icons", "step-out.xpm") ;
}

void
DBGPerspective::init_actions ()
{
    Gtk::StockID nil_stock_id ("") ;
    sigc::slot<void> nil_slot ;
    static ui_utils::ActionEntry s_debugger_ready_action_entries [] = {
        {
            "RunMenuItemAction",
            nemiver::STOCK_RUN_DEBUGGER,
            "_Run",
            "Run the debugger starting from program's begining",
            sigc::mem_fun (*this, &DBGPerspective::on_run_action)
        }
        ,
        {
            "NextMenuItemAction",
            nemiver::STOCK_STEP_OVER,
            "_Next",
            "Execute next instruction steping over the next function, if any",
            sigc::mem_fun (*this, &DBGPerspective::on_next_action)
        }
        ,
        {
            "StepMenuItemAction",
            nemiver::STOCK_STEP_INTO,
            "_Step",
            "Execute next instruction, steping into the next function, if any",
            sigc::mem_fun (*this, &DBGPerspective::on_step_into_action)
        }
        ,
        {
            "StepOutMenuItemAction",
            nemiver::STOCK_STEP_OUT,
            "Step _out",
            "Finish the execution of the current function",
            sigc::mem_fun (*this, &DBGPerspective::on_step_out_action)
        }
        ,
        {
            "ContinueMenuItemAction",
            nemiver::STOCK_CONTINUE,
            "_Continue",
            "Continue program execution until the next breakpoint",
            sigc::mem_fun (*this, &DBGPerspective::on_continue_action)
        }
        ,
        {
            "SetBreakPointMenuItemAction",
            nemiver::STOCK_SET_BREAKPOINT,
            "_Break",
            "Set a breakpoint the current cursor location",
            sigc::mem_fun (*this, &DBGPerspective::on_set_breakpoint_action)
        },
        {
            "UnSetBreakPointMenuItemAction",
            nil_stock_id,
            "_Unset breakpoint",
            "Unset the breakpoint the current cursor location",
            sigc::mem_fun (*this, &DBGPerspective::on_unset_breakpoint_action)
        }
    };

    static ui_utils::ActionEntry s_debugger_busy_action_entries [] = {
        {
            "StopDebuggerMenuItemAction",
            nil_stock_id,
            "Sto_p",
            "Stop the debugger",
            nil_slot
        }
    };

    static ui_utils::ActionEntry s_default_action_entries [] = {
        {
            "OpenMenuItemAction",
            Gtk::Stock::OPEN,
            "_Open",
            "Open a file",
            sigc::mem_fun (*this, &DBGPerspective::on_open_action)
        },
        {
            "ExecuteProgramMenuItemAction",
            nil_stock_id,
            "_Execute",
            "Execute a program",
            sigc::mem_fun (*this,
                           &DBGPerspective::on_execute_program_action)
        },
    };

    static ui_utils::ActionEntry s_file_opened_action_entries [] = {
        {
            "DebugMenuAction",
            nil_stock_id,
            "_Debug",
            "",
            nil_slot
        }
        ,
        {
            "CloseMenuItemAction",
            Gtk::Stock::CLOSE,
            "_Close",
            "Close the opened file",
            sigc::mem_fun (*this, &DBGPerspective::on_close_action)
        }
    };

    m_priv->debugger_ready_action_group =
                Gtk::ActionGroup::create ("debugger-ready-action-group") ;
    m_priv->debugger_ready_action_group->set_sensitive (false) ;

    m_priv->default_action_group =
                Gtk::ActionGroup::create ("default-action-group") ;
    m_priv->default_action_group->set_sensitive (true) ;

    m_priv->opened_file_action_group =
                Gtk::ActionGroup::create ("opened-file-action-group") ;
    m_priv->opened_file_action_group->set_sensitive (false) ;

    int num_actions =
         sizeof (s_debugger_ready_action_entries)/sizeof (ui_utils::ActionEntry) ;

    ui_utils::add_action_entries_to_action_group
                        (s_debugger_ready_action_entries,
                         num_actions,
                         m_priv->debugger_ready_action_group) ;

    m_priv->debugger_busy_action_group =
                Gtk::ActionGroup::create ("debugger-busy-action-group") ;
    m_priv->debugger_busy_action_group->set_sensitive (false) ;

    num_actions =
         sizeof (s_debugger_busy_action_entries)/sizeof (ui_utils::ActionEntry) ;

    ui_utils::add_action_entries_to_action_group
                        (s_debugger_busy_action_entries,
                         num_actions,
                         m_priv->debugger_busy_action_group) ;

    num_actions =
         sizeof (s_default_action_entries)/sizeof (ui_utils::ActionEntry) ;

    ui_utils::add_action_entries_to_action_group
                        (s_default_action_entries,
                         num_actions,
                         m_priv->default_action_group) ;

    num_actions =
         sizeof (s_file_opened_action_entries)/sizeof (ui_utils::ActionEntry) ;

    ui_utils::add_action_entries_to_action_group
                        (s_file_opened_action_entries,
                         num_actions,
                         m_priv->opened_file_action_group) ;

    m_priv->workbench->get_ui_manager ()->insert_action_group
                                            (m_priv->debugger_busy_action_group) ;
    m_priv->workbench->get_ui_manager ()->insert_action_group
                                            (m_priv->debugger_ready_action_group);
    m_priv->workbench->get_ui_manager ()->insert_action_group
                                            (m_priv->default_action_group);
    m_priv->workbench->get_ui_manager ()->insert_action_group
                                            (m_priv->opened_file_action_group);
}


void
DBGPerspective::init_toolbar ()
{
    add_perspective_toolbar_entries () ;

    m_priv->toolbar = dynamic_cast<Gtk::Toolbar*>
        (m_priv->workbench->get_ui_manager ()->get_widget ("/ToolBar")) ;
    THROW_IF_FAIL (m_priv->toolbar) ;
    m_priv->toolbar->show_all () ;

    Gtk::ToolButton *button=NULL ;

    button = dynamic_cast<Gtk::ToolButton*>
    (m_priv->workbench->get_ui_manager ()->get_widget ("/ToolBar/RunToolItem")) ;
    THROW_IF_FAIL (button) ;

    /*
    button = Gtk::manage (new Gtk::ToolButton ("run")) ;
    m_priv->toolbar->append (*button) ;

    button = Gtk::manage (new Gtk::ToolButton ("next")) ;
    m_priv->toolbar->append (*button) ;

    button = Gtk::manage (new Gtk::ToolButton ("step")) ;
    m_priv->toolbar->append (*button) ;

    button = Gtk::manage (new Gtk::ToolButton ("continue")) ;
    m_priv->toolbar->append (*button) ;

    button = Gtk::manage (new Gtk::ToolButton ("break")) ;
    m_priv->toolbar->append (*button) ;
    */

}

void
DBGPerspective::init_body ()
{
    string relative_path = Glib::build_filename ("glade",
                                                 "bodycontainer.glade") ;
    string absolute_path ;
    THROW_IF_FAIL (build_absolute_resource_path
                    (Glib::locale_to_utf8 (relative_path),
                                           absolute_path)) ;
    m_priv->body_glade = Gnome::Glade::Xml::create (absolute_path) ;
    m_priv->body_window =
        env::get_widget_from_glade<Gtk::Window> (m_priv->body_glade,
                                                 "bodycontainer") ;
    Glib::RefPtr<Gtk::Paned> paned
        (env::get_widget_from_glade<Gtk::Paned> (m_priv->body_glade,
                                                 "mainbodypaned")) ;
    paned->reference () ;
    m_priv->body_main_paned = paned ;

    m_priv->sourceviews_notebook =
        env::get_widget_from_glade<Gtk::Notebook> (m_priv->body_glade,
                                                   "sourceviewsnotebook") ;
    m_priv->sourceviews_notebook->remove_page () ;
    m_priv->sourceviews_notebook->set_show_tabs () ;

    m_priv->statuses_notebook =
        env::get_widget_from_glade<Gtk::Notebook> (m_priv->body_glade,
                                                   "statusesnotebook") ;
    m_priv->command_view =
        env::get_widget_from_glade<Gtk::TextView> (m_priv->body_glade,
                                                   "commandview") ;
    m_priv->command_view->set_editable (true) ;
    m_priv->command_view->get_buffer ()->signal_insert ().connect (sigc::mem_fun
            (*this, &DBGPerspective::on_insert_in_command_view_signal)) ;

    m_priv->program_output_view =
        env::get_widget_from_glade<Gtk::TextView> (m_priv->body_glade,
                                                   "programoutputview") ;
    m_priv->program_output_view->set_editable (false) ;

    m_priv->error_view =
        env::get_widget_from_glade<Gtk::TextView> (m_priv->body_glade,
                                                   "errorview") ;
    m_priv->error_view->set_editable (false) ;

    m_priv->body_main_paned->unparent () ;
    m_priv->body_main_paned->show_all () ;

}

void
DBGPerspective::init_signals ()
{
    m_priv->sourceviews_notebook->signal_switch_page ().connect
        (sigc::mem_fun (*this, &DBGPerspective::on_switch_page_signal)) ;
    debugger_ready_signal ().connect (sigc::mem_fun
            (*this, &DBGPerspective::on_debugger_ready_signal)) ;
}

void
DBGPerspective::init_debugger_output_handlers ()
{
    m_priv->output_handlers.push_back
        (OutputHandlerSafePtr (new OnStreamRecordHandler (this))) ;

    m_priv->output_handlers.push_back
        (OutputHandlerSafePtr (new OnStoppedHandler (this))) ;

    m_priv->output_handlers.push_back
        (OutputHandlerSafePtr (new OnBreakPointHandler (this))) ;
}

void
DBGPerspective::append_source_editor (SourceEditor &a_sv,
                                      const Glib::RefPtr<Gnome::Vfs::Uri> &a_uri)
{
    if (!a_uri) {return;}

    if (m_priv->uri_2_pagenum_map.find (a_uri->to_string ())
        != m_priv->uri_2_pagenum_map.end ()) {
        THROW (UString ("File of '")
               + a_uri->to_string ()
               + "' is already loaded") ;
    }

    SafePtr<Gtk::Label> label (Gtk::manage
                            (new Gtk::Label (a_uri->extract_short_name ()))) ;
    SafePtr<Gtk::Image> cicon (manage
                (new Gtk::Image (Gtk::StockID ("gtk-close"),
                                               Gtk::ICON_SIZE_BUTTON))) ;

    int w=0, h=0 ;
    Gtk::IconSize::lookup (Gtk::ICON_SIZE_MENU, w, h) ;
    SafePtr<SlotedButton> close_button (Gtk::manage (new SlotedButton ())) ;
    close_button->perspective = this ;
    close_button->set_size_request (w+4, h+4) ;
    close_button->set_relief (Gtk::RELIEF_NONE) ;
    close_button->add (*cicon) ;
    close_button->uri = a_uri->to_string () ;
    close_button->signal_clicked ().connect
            (sigc::mem_fun (*close_button, &SlotedButton::on_clicked)) ;

    SafePtr<Gtk::Table> table (Gtk::manage (new Gtk::Table (1, 2))) ;
    table->attach (*label, 0, 1, 0, 1) ;
    table->attach (*close_button, 1, 2, 0, 1) ;

    table->show_all () ;
    int page_num = m_priv->sourceviews_notebook->insert_page (a_sv,
                                                              *table,
                                                              -1);
    m_priv->uri_2_pagenum_map[a_uri->get_path ()] = page_num ;
    m_priv->pagenum_2_source_editor_map[page_num] = &a_sv;
    m_priv->pagenum_2_uri_map[page_num] = a_uri->get_path ();

    table.release () ;
    close_button.release () ;
    label.release () ;
    cicon.release () ;
}

SourceEditor*
DBGPerspective::get_current_source_editor ()
{
    THROW_IF_FAIL (m_priv) ;

    if (!m_priv->sourceviews_notebook) {return NULL;}

    if (m_priv->sourceviews_notebook
        && !m_priv->sourceviews_notebook->get_n_pages ()) {
        return NULL ;
    }

    map<int, SourceEditor*>::iterator iter, nil ;
    nil = m_priv->pagenum_2_source_editor_map.end () ;

    iter = m_priv->pagenum_2_source_editor_map.find (m_priv->current_page_num) ;
    if (iter == nil) {return NULL ;}

    return iter->second ;
}

UString
DBGPerspective::get_current_file_path ()
{
    SourceEditor *source_editor = get_current_source_editor () ;
    if (!source_editor) {return "";}
    return source_editor->get_path () ;
}

SourceEditor*
DBGPerspective::get_source_editor_from_uri (const UString &a_uri)
{
    map<UString, int>::iterator iter =
        m_priv->uri_2_pagenum_map.find (a_uri) ;
    if (iter == m_priv->uri_2_pagenum_map.end ()) {
        return NULL ;
    }
    return m_priv->pagenum_2_source_editor_map[iter->second] ;
}

void
DBGPerspective::bring_source_as_current (const UString &a_uri)
{
    SourceEditor *source_editor = get_source_editor_from_uri (a_uri) ;
    if (!source_editor) {
        open_file (a_uri) ;
    }
    source_editor = get_source_editor_from_uri (a_uri) ;
    THROW_IF_FAIL (source_editor) ;
    map<UString, int>::iterator iter =
        m_priv->uri_2_pagenum_map.find (a_uri) ;
    THROW_IF_FAIL (iter != m_priv->uri_2_pagenum_map.end ()) ;
    m_priv->sourceviews_notebook->set_current_page (iter->second) ;
}

void
DBGPerspective::set_where (const UString &a_uri,
                           int a_line)
{
    bring_source_as_current (a_uri) ;
    SourceEditor *source_editor = get_source_editor_from_uri (a_uri) ;
    THROW_IF_FAIL (source_editor) ;
    source_editor->move_where_marker_to_line (a_line) ;
}

Gtk::Widget*
DBGPerspective::get_contextual_menu ()
{
    THROW_IF_FAIL (m_priv && m_priv->contextual_menu_merge_id) ;

    if (!m_priv->contextual_menu) {

        m_priv->workbench->get_ui_manager ()->add_ui
            (m_priv->contextual_menu_merge_id,
             "/ContextualMenu",
             "UnSetBreakPointMenuItem",
             "UnSetBreakPointMenuItemAction") ;

        m_priv->workbench->get_ui_manager ()->add_ui
            (m_priv->contextual_menu_merge_id,
             "/ContextualMenu",
             "SetBreakPointMenuItem",
             "SetBreakPointMenuItemAction") ;

        m_priv->workbench->get_ui_manager ()->ensure_update () ;
        m_priv->contextual_menu =
            m_priv->workbench->get_ui_manager ()->get_widget
            ("/ContextualMenu") ;
        THROW_IF_FAIL (m_priv->contextual_menu) ;
    }
    return m_priv->contextual_menu ;
}

int
DBGPerspective::get_n_pages ()
{
    THROW_IF_FAIL (m_priv && m_priv->sourceviews_notebook) ;

    return m_priv->sourceviews_notebook->get_n_pages () ;
}

void
DBGPerspective::popup_source_view_contextual_menu (GdkEventButton *a_event)
{
    SourceEditor *editor = get_current_source_editor () ;
    THROW_IF_FAIL (editor) ;
    int buffer_x=0, buffer_y=0, line_top=0;
    Gtk::TextBuffer::iterator cur_iter ;
    UString file_name ;
    int break_num=0 ;

    editor->source_view ().window_to_buffer_coords (Gtk::TEXT_WINDOW_TEXT,
                                                    (int)a_event->x,
                                                    (int)a_event->y,
                                                    buffer_x, buffer_y) ;
    editor->source_view ().get_line_at_y (cur_iter, buffer_y, line_top) ;

    file_name = editor->get_path () ;
    bool breakpoint_found=false ;
    if (get_breakpoint_number (file_name,
                                cur_iter.get_line () + 1,
                                break_num)) {
        breakpoint_found = true ;
    }
    Glib::RefPtr<Gtk::Action> break_action =
        m_priv->debugger_ready_action_group->get_action
                                            ("SetBreakPointMenuItemAction") ;
    THROW_IF_FAIL (break_action) ;
    Glib::RefPtr<Gtk::Action> unbreak_action =
        m_priv->debugger_ready_action_group->get_action
                                            ("UnSetBreakPointMenuItemAction") ;
    THROW_IF_FAIL (unbreak_action) ;
    if (breakpoint_found) {
        break_action->set_sensitive (false) ;
        unbreak_action->set_sensitive (true) ;
    } else {
        break_action->set_sensitive (true) ;
        unbreak_action->set_sensitive (false) ;
    }

    Gtk::Menu *menu = dynamic_cast<Gtk::Menu*> (get_contextual_menu ()) ;
    THROW_IF_FAIL (menu) ;
    editor->source_view ().get_buffer ()->place_cursor (cur_iter) ;
    menu->popup (a_event->button, a_event->time) ;
}


//*******************
//</private methods>
//*******************

DBGPerspective::DBGPerspective ()
{
    m_priv = new Priv ;
}

void
DBGPerspective::get_info (Info &a_info) const
{
    static Info s_info ("Debugger perspective plugin",
                        "The debugger perspective of Nemiver",
                        "1.0") ;
    a_info = s_info ;
}

void
DBGPerspective::do_init ()
{
}

void
DBGPerspective::do_init (IWorkbenchSafePtr &a_workbench)
{
    THROW_IF_FAIL (m_priv) ;
    m_priv->workbench = a_workbench ;
    init_icon_factory () ;
    init_actions () ;
    init_toolbar () ;
    init_body () ;
    init_signals () ;
    init_debugger_output_handlers () ;
    m_priv->initialized = true ;
}

DBGPerspective::~DBGPerspective ()
{
    m_priv = NULL ;
}

const UString&
DBGPerspective::get_perspective_identifier ()
{
    static UString s_id = "org.nemiver.DebuggerPerspective" ;
    return s_id ;
}

void
DBGPerspective::get_toolbars (list<Gtk::Toolbar*>  &a_tbs)
{
    CHECK_P_INIT ;
    a_tbs.push_back (m_priv->toolbar) ;
}

Gtk::Widget*
DBGPerspective::get_body ()
{
    CHECK_P_INIT ;
    return m_priv->body_main_paned.operator->() ;
}

void
DBGPerspective::edit_workbench_menu ()
{
    CHECK_P_INIT ;

    add_perspective_menu_entries () ;
}

void
DBGPerspective::open_file ()
{
    Gtk::FileChooserDialog file_chooser ("Open file",
                                         Gtk::FILE_CHOOSER_ACTION_OPEN) ;

    file_chooser.add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL) ;
    file_chooser.add_button (Gtk::Stock::OK, Gtk::RESPONSE_OK) ;
    file_chooser.set_select_multiple (true) ;

    int result = file_chooser.run () ;

    if (result != Gtk::RESPONSE_OK) {return;}

    list<UString> uris = file_chooser.get_uris () ;
    list<UString>::const_iterator iter ;

    for (iter = uris.begin () ; iter != uris.end () ; ++iter) {
        open_file (*iter) ;
    }
}


bool
DBGPerspective::open_file (const UString &a_uri,
                           int a_current_line)
{
    if (!a_uri) {return false;}

    Glib::RefPtr<Gnome::Vfs::Uri> uri = Gnome::Vfs::Uri::create (a_uri) ;
    Gnome::Vfs::Handle handle ;

    NEMIVER_TRY

    handle.open (a_uri, Gnome::Vfs::OPEN_READ) ;
    UString base_name = uri->extract_short_name () ;
    UString mime_type = gnome_vfs_get_mime_type_for_name (base_name.c_str ()) ;
    if (mime_type == "") {
        mime_type = "text/plain" ;
    }

    SourceLanguagesManager lang_manager ;
    Glib::RefPtr<SourceLanguage> lang =
        lang_manager.get_language_from_mime_type (mime_type) ;

    Glib::RefPtr<SourceBuffer> source_buffer = SourceBuffer::create (lang) ;
    THROW_IF_FAIL (source_buffer) ;

    guint buf_size = 10 * 1024 ;
    SafePtr<gchar> buf (new gchar [buf_size]) ;
    guint nb_bytes_read (0) ;

    for (;;) {
        nb_bytes_read = handle.read (buf, buf_size) ;
        if (nb_bytes_read) {
            source_buffer->insert (source_buffer->end (), buf,
                                   buf.get () + nb_bytes_read) ;
        }
        if (nb_bytes_read != buf_size) {break;}
    }
    handle.close () ;

    source_buffer->set_highlight () ;
    SourceEditor *source_editor (Gtk::manage
                                (new SourceEditor (plugin_path (),
                                 source_buffer)));
    source_editor->set_path (uri->get_path ()) ;

    if (a_current_line > 0) {
        Gtk::TextIter cur_line_iter =
                source_buffer->get_iter_at_line (a_current_line) ;
        if (cur_line_iter) {
            Glib::RefPtr<SourceMarker> where_marker =
                source_buffer->create_marker ("where-marker",
                                              "line-pointer-marker",
                                              cur_line_iter) ;
            THROW_IF_FAIL (where_marker) ;
        }
    }
    source_editor->show_all () ;
    append_source_editor (*source_editor, uri) ;

    if (!source_editor->source_view ().has_no_window ()) {
        source_editor->source_view ().add_events (Gdk::BUTTON3_MOTION_MASK) ;
        source_editor->source_view ().signal_button_press_event ().connect
            (sigc::mem_fun
             (*this,
              &DBGPerspective::on_button_pressed_in_source_view_signal)) ;
    }

    m_priv->opened_file_action_group->set_sensitive (true) ;

    NEMIVER_CATCH_AND_RETURN (false);
    return true ;
}

void
DBGPerspective::close_current_file ()
{
    if (!get_n_pages ()) {return;}

    close_file (m_priv->pagenum_2_uri_map[m_priv->current_page_num]) ;
}

void
DBGPerspective::close_file (const UString &a_uri)
{
    map<UString, int>::const_iterator nil, iter ;
    iter = m_priv->uri_2_pagenum_map.find (a_uri) ;
    if (iter == nil) {return;}

    int page_num = m_priv->uri_2_pagenum_map[a_uri] ;
    m_priv->sourceviews_notebook->remove_page (page_num) ;
    m_priv->uri_2_pagenum_map.erase (a_uri) ;
    m_priv->pagenum_2_source_editor_map.erase (page_num) ;
    m_priv->pagenum_2_uri_map.erase (page_num) ;

    if (!get_n_pages ()) {
        m_priv->opened_file_action_group->set_sensitive (false) ;
    }
}

void
DBGPerspective::execute_program ()
{
    RunProgramDialog dialog (plugin_path ()) ;

    int result = dialog.run () ;
    if (result != Gtk::RESPONSE_OK) {
        LOG ("user asked to cancel") ;
        return;
    }

    UString prog, args, cwd ;
    prog = dialog.program_name () ;
    THROW_IF_FAIL (prog != "") ;
    args = dialog.arguments () ;
    cwd = dialog.working_directory () ;
    THROW_IF_FAIL (cwd != "") ;

    execute_program (prog, args, cwd) ;
}

void
DBGPerspective::execute_program (const UString &a_prog_and_args,
                                 const UString &a_cwd)
{
    vector<UString> argv = a_prog_and_args.split (" ") ;
    vector<UString>::const_iterator iter = argv.begin () ;
    vector<UString>::const_iterator end_iter = argv.end () ;
    ++iter ;
    UString prog_name=argv[0], args = UString::join (iter, end_iter);
    execute_program (prog_name, args, a_cwd) ;
}

void
DBGPerspective::execute_program (const UString &a_prog,
                                 const UString &a_args,
                                 const UString &a_cwd)
{
    NEMIVER_TRY

    LOG ("a_prog: " << a_prog
         << " a_args: " << a_args
         << " a_cwd: " << a_cwd) ;

    IDebuggerSafePtr dbg_engine = debugger () ;
    THROW_IF_FAIL (dbg_engine) ;
    vector<UString> args = a_args.split (" ") ;
    args.insert (args.begin (), a_prog) ;
    vector<UString> source_search_dirs = a_cwd.split (" ") ;

    dbg_engine->load_program (args, source_search_dirs) ;

    dbg_engine->queue_command (IDebugger::Command ("-break-insert main")) ;

    dbg_engine->queue_command (IDebugger::Command ("-exec-run")) ;

    debugger_ready_signal ().emit (true) ;

    NEMIVER_CATCH
}

void
DBGPerspective::run ()
{
    debugger ()->run () ;
    debugger_ready_signal ().emit (false) ;
}
void
DBGPerspective::step_over ()
{
    debugger ()->step_over () ;
    debugger_ready_signal ().emit (false) ;
}

void
DBGPerspective::step_into ()
{
    debugger ()->step_in () ;
    debugger_ready_signal ().emit (false) ;
}

void
DBGPerspective::step_out ()
{
    debugger ()->step_out () ;
    debugger_ready_signal ().emit (false) ;
}

void
DBGPerspective::do_continue ()
{
    debugger ()->do_continue () ;
    debugger_ready_signal ().emit (false) ;
}

void
DBGPerspective::set_breakpoint ()
{
    SourceEditor *source_editor = get_current_source_editor () ;
    THROW_IF_FAIL (source_editor) ;
    THROW_IF_FAIL (source_editor->get_path () != "") ;

    gint current_line =
        source_editor->source_view ().get_source_buffer ()->get_insert
            ()->get_iter ().get_line () + 1;
    set_breakpoint (source_editor->get_path (), current_line) ;
}

void
DBGPerspective::set_breakpoint (const UString &a_file_path,
                                int a_line)
{
    debugger ()->set_breakpoint (a_file_path, a_line) ;
    debugger_ready_signal ().emit (false) ;
}

void
DBGPerspective::append_breakpoints (map<int, IDebugger::BreakPoint> &a_breaks)
{
    map<int, IDebugger::BreakPoint>::const_iterator iter ;
    for (iter = a_breaks.begin () ; iter != a_breaks.end () ; ++iter) {
        m_priv->breakpoints[iter->first] = iter->second ;
        append_visual_breakpoint (iter->second.full_file_name (),
                                  iter->second.line ()-1) ;
    }
}

bool
DBGPerspective::get_breakpoint_number (const UString &a_file_name,
                                       int a_line_num,
                                       int &a_break_num)
{
    map<int, IDebugger::BreakPoint>::const_iterator iter ;
    for (iter = m_priv->breakpoints.begin () ;
         iter != m_priv->breakpoints.end () ;
         ++iter) {
        if (   (iter->second.full_file_name () == a_file_name)
            && (iter->second.line () == a_line_num)) {
            a_break_num= iter->second.number () ;
            return true ;
        }
    }
    return false ;
}

bool
DBGPerspective::delete_breakpoint ()
{
    SourceEditor *source_editor = get_current_source_editor () ;
    THROW_IF_FAIL (source_editor) ;
    THROW_IF_FAIL (source_editor->get_path () != "") ;

    gint current_line =
        source_editor->source_view ().get_source_buffer ()->get_insert
            ()->get_iter ().get_line () + 1;
    int break_num=0 ;
    if (!get_breakpoint_number (source_editor->get_path (),
                                current_line,
                                break_num)) {
        return false ;
    }
    THROW_IF_FAIL (break_num) ;
    return delete_breakpoint (break_num) ;
}

bool
DBGPerspective::delete_breakpoint (int a_breakpoint_num)
{
    map<int, IDebugger::BreakPoint>::iterator iter =
        m_priv->breakpoints.find (a_breakpoint_num) ;
    if (iter == m_priv->breakpoints.end ()) {
        LOG ("breakpoint " << (int) a_breakpoint_num << " not found") ;
        return false ;
    }
    debugger ()->delete_breakpoint (a_breakpoint_num) ;
    debugger_ready_signal ().emit (false) ;
    return true ;
}

void
DBGPerspective::append_visual_breakpoint (const UString &a_file_name,
                                          int a_linenum)
{
    LOG ("file_name: " << a_file_name << "\nlinenum: " << (int)a_linenum) ;
    if (a_linenum < 0) {a_linenum = 0;}

    SourceEditor *source_editor = get_source_editor_from_uri (a_file_name) ;
    if (!source_editor) {
        open_file (a_file_name) ;
        source_editor = get_source_editor_from_uri (a_file_name) ;
    }
    THROW_IF_FAIL (source_editor) ;
    source_editor->set_visual_breakpoint_at_line (a_linenum) ;
}

void
DBGPerspective::delete_visual_breakpoint (const UString &a_file_name, int a_linenum)
{
    LOG ("filename: " << a_file_name << "\nlinenum: " << (int)a_linenum) ;

    SourceEditor *source_editor = get_source_editor_from_uri (a_file_name) ;
    THROW_IF_FAIL (source_editor) ;
    source_editor->remove_visual_breakpoint_from_line (a_linenum) ;
}

void
DBGPerspective::delete_visual_breakpoint (int a_breakpoint_num)
{
    map<int, IDebugger::BreakPoint>::iterator iter =
        m_priv->breakpoints.find (a_breakpoint_num) ;
    if (iter == m_priv->breakpoints.end ()) {
        LOG ("breakpoint " << (int) a_breakpoint_num << " not found") ;
        return ;
    }

    SourceEditor *source_editor =
        get_source_editor_from_uri (iter->second.full_file_name ()) ;
    THROW_IF_FAIL (source_editor) ;
    LOG ("removing visual bkpt marker from line: " << (int) iter->second.line ()) ;
    source_editor->remove_visual_breakpoint_from_line (iter->second.line ()-1) ;
    m_priv->breakpoints.erase (iter);
}

bool
DBGPerspective::delete_breakpoint (const UString &a_file_name,
                                   int a_line_num)
{
    int breakpoint_number=0 ;
    if (!get_breakpoint_number (a_file_name,
                                a_line_num,
                                breakpoint_number)) {
        return false ;
    }
    if (breakpoint_number < 1) {return false;}

    return delete_breakpoint (breakpoint_number) ;
}

IDebuggerSafePtr&
DBGPerspective::debugger ()
{
    if (!m_priv->debugger) {
        DynamicModule::Loader *loader = plugin_entry_point_loader () ;
        THROW_IF_FAIL (loader) ;
        DynamicModuleManager *module_manager =
                            loader->get_dynamic_module_manager () ;
        THROW_IF_FAIL (module_manager) ;

        m_priv->debugger =
            module_manager->load<IDebugger> ("gdbengine",
                                             *plugin_entry_point_loader ()) ;
        m_priv->debugger->set_event_loop_context
                                    (Glib::MainContext::get_default ()) ;
        m_priv->debugger->stdout_signal ().connect (sigc::mem_fun
                (*this, &DBGPerspective::on_debugger_stdout_signal)) ;
    }
    THROW_IF_FAIL (m_priv->debugger) ;
    return m_priv->debugger ;
}

Gtk::TextView*
DBGPerspective::get_command_view ()
{
    return m_priv->command_view ;
}

Gtk::TextView*
DBGPerspective::get_program_output_view ()
{
    return m_priv->program_output_view ;
}

Gtk::TextView*
DBGPerspective::get_error_view ()
{
    return m_priv->error_view ;
}

struct ScrollTextViewToEndClosure {
    Gtk::TextView *text_view ;

    ScrollTextViewToEndClosure (Gtk::TextView *a_view=NULL) :
        text_view (a_view)
    {
    }

    bool do_exec ()
    {
        if (!text_view) {return false;}
        if (!text_view->get_buffer ()) {return false;}

        Gtk::TextIter end_iter = text_view->get_buffer ()->end () ;
        text_view->scroll_to (end_iter) ;
        return false ;
    }
};//end struct ScrollTextViewToEndClosure

void
DBGPerspective::add_text_to_command_view (const UString &a_text)
{
    THROW_IF_FAIL (m_priv && m_priv->command_view) ;
    m_priv->command_view->get_buffer ()->insert
        (m_priv->command_view->get_buffer ()->end (),
         a_text) ;
    static ScrollTextViewToEndClosure s_scroll_to_end_closure ;
    s_scroll_to_end_closure.text_view = m_priv->command_view ;
    Glib::signal_idle ().connect (sigc::mem_fun
            (s_scroll_to_end_closure, &ScrollTextViewToEndClosure::do_exec)) ;
}

void
DBGPerspective::add_text_to_program_output_view (const UString &a_text)
{
    THROW_IF_FAIL (m_priv && m_priv->program_output_view) ;
    m_priv->program_output_view->get_buffer ()->insert
        (m_priv->program_output_view->get_buffer ()->end (),
         a_text) ;
    static ScrollTextViewToEndClosure s_scroll_to_end_closure ;
    s_scroll_to_end_closure.text_view = m_priv->program_output_view ;
    Glib::signal_idle ().connect (sigc::mem_fun
            (s_scroll_to_end_closure, &ScrollTextViewToEndClosure::do_exec)) ;
}

void
DBGPerspective::add_text_to_error_view (const UString &a_text)
{
    THROW_IF_FAIL (m_priv && m_priv->error_view) ;
    m_priv->error_view->get_buffer ()->insert
        (m_priv->error_view->get_buffer ()->end (), a_text) ;
    static ScrollTextViewToEndClosure s_scroll_to_end_closure ;
    s_scroll_to_end_closure.text_view = m_priv->error_view ;
    Glib::signal_idle ().connect (sigc::mem_fun
            (s_scroll_to_end_closure, &ScrollTextViewToEndClosure::do_exec)) ;
}

sigc::signal<void, bool>&
DBGPerspective::activated_signal ()
{
    CHECK_P_INIT ;
    return m_priv->activated_signal ;
}

sigc::signal<void, bool>&
DBGPerspective::debugger_ready_signal ()
{
    return m_priv->debugger_ready_signal ;
}

}//end namespace nemiver

//the dynmod initial factory.
extern "C" {
bool
NEMIVER_API nemiver_common_create_dynamic_module_instance (void **a_new_instance)
{
    gtksourceview::init () ;
    *a_new_instance = new nemiver::DBGPerspective () ;
    return (*a_new_instance != 0) ;
}

}//end extern C
