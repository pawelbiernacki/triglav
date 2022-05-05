#include "config.h"
#include "triglav_demo.h"

triglav_demo_window::triglav_demo_window()
{
    set_title(PACKAGE_STRING);

    add(grid);

    action_group = Gtk::ActionGroup::create();

    action_group->add(Gtk::Action::create("GameMenu", "Game"));

    action_group->add(Gtk::Action::create("GameNew", Gtk::Stock::NEW),
                      sigc::mem_fun(*this, &triglav_demo_window::on_menu_game_new));

    action_group->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT),
                      sigc::mem_fun(*this, &triglav_demo_window::on_menu_game_quit));

    action_group->add(Gtk::Action::create("HelpMenu", "Help"));
    action_group->add(Gtk::Action::create("HelpAbout", "About"),
                      sigc::mem_fun(*this, &triglav_demo_window::on_menu_help_about));

    manager = Gtk::UIManager::create();
    manager->insert_action_group(action_group);

    add_accel_group(manager->get_accel_group());

    try
    {
        manager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
        std::cerr << "building menus failed: " <<  ex.what();
    }

    grid.set_row_homogeneous(true);
    grid.set_column_homogeneous(true);

    Gtk::Widget* pMenubar = manager->get_widget("/MenuBar");
    if (pMenubar)
        grid.attach(*pMenubar, 0, 0, 10, 1);

    Gtk::Widget* pToolbar = manager->get_widget("/ToolBar") ;
    if (pToolbar)
        grid.attach(*pToolbar, 0, 1, 10, 1);

    grid.attach(drawing_area, 0, 2, 10, 10);

    show_all_children();
}


void triglav_demo_window::on_menu_game_new()
{
}

void triglav_demo_window::on_menu_game_open()
{
}

void triglav_demo_window::on_menu_game_save_as()
{
}

void triglav_demo_window::on_menu_game_quit()
{
    Gtk::MessageDialog dialog(*this, "Are you sure?",
                              false /* use_markup */, Gtk::MESSAGE_QUESTION,
                              Gtk::BUTTONS_OK_CANCEL);

    dialog.set_transient_for(*this);

    int result = dialog.run();

    switch (result)
    {
    case(Gtk::RESPONSE_OK):
    {
        hide();
        break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
        break;
    }
    }
}

void triglav_demo_window::on_menu_help_about()
{
    Gtk::AboutDialog dialog;

    std::vector<Glib::ustring> authors;
    authors.push_back("Pawel Biernacki (" PACKAGE_BUGREPORT ")<" PACKAGE_BUGREPORT ">");

    dialog.set_transient_for(*this);

    dialog.set_program_name("triglav_demo");
    dialog.set_version(PACKAGE_VERSION);
    dialog.set_authors(authors);
    dialog.set_license(
        "   This program is free software; you can redistribute it and/or modify  \n"
        "   it under the terms of the GNU General Public License as published by  \n"
        "   the Free Software Foundation; either version 3 of the License, or     \n"
        "   (at your option) any later version.                                   \n"
        "                                                                         \n"
        "   This program is distributed in the hope that it will be useful,       \n"
        "   but WITHOUT ANY WARRANTY; without even the implied warranty of        \n"
        "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         \n"
        "   GNU General Public License for more details.                          \n"
        "                                                                         \n"
        "   You should have received a copy of the GNU General Public License     \n"
        "   along with this program; if not, write to the                         \n"
        "   Free Software Foundation, Inc.,                                       \n"
        "   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             \n"
    );
    dialog.set_website("http://www.perkun.org");
    dialog.set_website_label("http://www.perkun.org");
    dialog.set_comments(
        "\n"
    );
    dialog.run();
}


const Glib::ustring triglav_demo_window::ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='GameMenu'>"
    "      <menuitem action='GameNew'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='HelpMenu'>"
    "      <menuitem action='HelpAbout'/>"
    "    </menu>"
    "  </menubar>"
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='Quit'/>"
    "  </toolbar>"
    "</ui>";

