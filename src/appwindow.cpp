#include "appwindow.hpp"

AppWindow::AppWindow()
{
  set_title("Advanced Ergonomics Laboratory");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));

  m_menu_app.items().push_back(MenuElem("_Reset Position", Gtk::AccelKey("i"),
    sigc::mem_fun(m_viewer, &Viewer::reset_position)));

  m_menu_app.items().push_back(MenuElem("_Reset Orientation", Gtk::AccelKey("o"),
    sigc::mem_fun(m_viewer, &Viewer::reset_orientation)));

  m_menu_mode.items().push_back(MenuElem("_Position", Gtk::AccelKey("p"),
    sigc::mem_fun(m_viewer, &Viewer::set_position)));

  m_menu_mode.items().push_back(MenuElem("_Joint", Gtk::AccelKey("j"),
    sigc::mem_fun(m_viewer, &Viewer::set_joint)));

  m_menu_edit.items().push_back(MenuElem("_Undo", Gtk::AccelKey("u"),
    sigc::mem_fun(m_viewer, &Viewer::undo)));

  m_menu_edit.items().push_back(MenuElem("_Redo", Gtk::AccelKey("r"),
    sigc::mem_fun(m_viewer, &Viewer::redo)));

  m_menu_option.items().push_back(MenuElem("_Z-Buffer", Gtk::AccelKey("z"),
    sigc::mem_fun(m_viewer, &Viewer::set_z_buffer)));

  m_menu_option.items().push_back(MenuElem("_Font-Cull", Gtk::AccelKey("f"),
    sigc::mem_fun(m_viewer, &Viewer::set_front_cull)));

  m_menu_option.items().push_back(MenuElem("_Back-Cull", Gtk::AccelKey("b"),
    sigc::mem_fun(m_viewer, &Viewer::set_back_cull)));
  
  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
  
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));

  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Edit", m_menu_edit));

  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Option", m_menu_option));

  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  m_viewer.set_size_request(300, 300);
  m_vbox.pack_start(m_viewer);

  show_all();
}

void AppWindow::set_scene_node(SceneNode *root) {
  m_viewer.set_scene_node(root);
}
