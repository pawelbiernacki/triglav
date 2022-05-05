#include <gtkmm.h>
#include "triglav.h"


class triglav_drawing_area: public Gtk::DrawingArea
{
};

class triglav_demo_window: public Gtk::Window
{
public:
    triglav_demo_window();

private:
    static const Glib::ustring ui_info;

protected:
    void on_menu_game_new();

    void on_menu_game_open();

    void on_menu_game_save_as();

    void on_menu_game_quit();

    void on_menu_help_about();

    triglav_drawing_area drawing_area;

    Gtk::Grid grid;

    Glib::RefPtr<Gtk::UIManager> manager;

    Glib::RefPtr<Gtk::ActionGroup> action_group;
};
