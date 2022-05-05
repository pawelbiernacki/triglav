#include "triglav_demo.h"

int main(int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app =
        Gtk::Application::create(argc, argv, "net.pawelbiernacki.triglav_demo");

    triglav_demo_window window;
    window.set_default_size(1024, 768);

    int i = app->run(window);

    return 0;
}
