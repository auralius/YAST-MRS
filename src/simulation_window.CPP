#include "simulation_window.H"


CSimulationWindow::CSimulationWindow(int w,int h,const char *l)
    :Fl_Double_Window(0 , 0, w, h, l)
{
    // Initialization
    m_simulation_running = false;
    m_simulation_speed = FAST;
    m_properties_window = NULL;

    m_world = new CWorld(w, h);
    m_canvas = new CCanvas(0, 25, w, h + 25, ""); // Width of menu bar = 25
    m_menu = new Fl_Menu_Bar(0, 0, w, 25);

	build_menu();

    this->callback(window_cb, this);
}

CSimulationWindow::~CSimulationWindow(void)
{
	fprintf(stdout, "Quitting\n...Cleaning memory [CSimulationWindow]\n");
	fflush(stdout);

    // Clean up memory
    Fl::delete_widget(m_menu);
    Fl::delete_widget(m_canvas);

    if (m_world)
        delete m_world;

    if (m_properties_window)
        delete[] m_properties_window;
}


///////////////////////////////////////////////////////////////////////////////
// GET
///////////////////////////////////////////////////////////////////////////////

bool CSimulationWindow::get_simulation_flag()
{
    return m_simulation_running;
}

int CSimulationWindow::get_simulation_speed()
{
    return m_simulation_speed;
}

const bool *CSimulationWindow::get_binary_map()
{
    if (m_world == NULL)
        return NULL;

    return m_world->get_binary_map();
}

int CSimulationWindow::get_area()
{
    return m_area;
}

int CSimulationWindow::get_map_height()
{
    return m_world->get_height();
}

int CSimulationWindow::get_map_width()
{
    return m_world->get_width();
}

CCanvas *CSimulationWindow::get_canvas()
{
    return m_canvas;
}

CPropertiesWindow *CSimulationWindow::get_properties_window()
{
    return m_properties_window; 
}

void CSimulationWindow::set_cfg_data(const AZ_CONFIG *cfg)
{
	// Set m_cfg in CSimulaitonWindow and in CCanvas to point m_cfg in CRobot
	m_cfg = cfg;
	m_canvas->set_cfg_data(cfg);
}

void CSimulationWindow::set_text_propoerties_window(const char *t)
{
    if (m_properties_window)
        m_properties_window->set_disp(t);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS
///////////////////////////////////////////////////////////////////////////////

void CSimulationWindow::build_menu()
{
    // Menu bar
    m_menu->add("File/Open map...", FL_CTRL + 'o', menu_cb, (void*)this);
    m_menu->add("File/Load script...", FL_CTRL + 'l', menu_cb, (void*)this);
    m_menu->add("File/Quit", FL_CTRL + 'q', menu_cb, (void*)this);
    m_menu->add("View/Robot properties...", FL_CTRL + 'p', menu_cb, (void*)this);
    m_menu->add("Simulation/Run", FL_CTRL + 'r', menu_cb, (void*)this);
    m_menu->add("Simulation/Stop", FL_CTRL + 'r', menu_cb, (void*)this);
    m_menu->add("Simulation/Speed/Slow", FL_CTRL + '1', menu_cb, (void*)this);
    m_menu->add("Simulation/Speed/Medium", FL_CTRL + '2', menu_cb, (void*)this);
    m_menu->add("Simulation/Speed/Fast", FL_CTRL + '3', menu_cb, (void*)this);
    m_menu->add("Help/About...", 0, menu_cb, (void*)this);

    // Radio menu bar
    Fl_Menu_Item *m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Speed/Slow");
    m->setonly();
    m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Speed/Medium");
    m->setonly();
    m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Speed/Fast");
    m->setonly();
  
    // Deactivate "Simulation/Stop" menu
    m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Stop");
    m->deactivate();
}

void CSimulationWindow::stop_simulation()
{
    if (m_simulation_running == true) {
        m_simulation_running = false;
        fprintf(stdout, "Simulation stopped\n");
        fflush(stdout);

        m_canvas->notify_simulation_status(false);

        // Give some time for all the simulation threads to stop properly
#ifdef WIN32
        Sleep(1000);
#else
        usleep(1000000);
#endif
    }
}

void CSimulationWindow::menu_cb(Fl_Widget*w, void*data)
{
    CSimulationWindow *o = (CSimulationWindow*)data;
    o->menu_cb_worker();
}

void CSimulationWindow::menu_cb_worker()
{
    char picked[80];
    m_menu->item_pathname(picked, sizeof(picked)-1);

    // CALL BACK HANDLER!!
    if (strcmp(picked, "File/Open map...") == 0)
    {
        Fl_Native_File_Chooser fnfc;
        fnfc.title("Open a PNG file");
        fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
        fnfc.filter("PNG image file\t*.png\n");
        fnfc.directory(".");              // default directory to use                          
        fnfc.show();

        if (strlen(fnfc.filename()) == 0) // Cancelled?
            return;

        if (m_world)
            delete m_world;
        
        // Load the map image file to memory
        m_world = new CWorld(fnfc.filename(), m_cfg);
        
        // Resize the window and the menu bar to follow the size of the map
        this->resize(0, 0, m_world->get_width(), m_world->get_height() + 25);
        m_menu->resize(0, 0, m_world->get_width(), 25);
        m_canvas->resize(0, 25, m_world->get_width(), m_world->get_height() + 25);

        // Update the area
        m_area = m_world->get_width() * m_world->get_height();

        // Display the map on canvas
        m_canvas->set_map_image(m_world->get_map_image());

        // After resizing the window, the title bar goes over the top of the screen.
        // This might be a bug in FLTK for Windows?
        this->position(0, 30);
    }

    else if (strcmp(picked, "File/Quit") == 0)
    {
        stop_simulation();
    	this->hide(); // The best way to exit is by hiding the window
    }

    else if (strcmp(picked, "Simulation/Run") == 0)
    {
        if (m_world == NULL) {
            fl_message("Please load a map file (a PNG file).");
            return;
        }

        m_simulation_running = true;
        fprintf(stdout, "Simulation is running\n");
        fflush(stdout);

        m_canvas->notify_simulation_status(true);

        Fl_Menu_Item *m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Run");
        m->deactivate();
        m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Stop");
        m->activate();
    }

    else if (strcmp(picked, "Simulation/Stop") == 0)
    {
        stop_simulation();

        Fl_Menu_Item *m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Run");
        m->activate();
        m = (Fl_Menu_Item *)m_menu->find_item("Simulation/Stop");
        m->deactivate();
    }

    else if (strcmp(picked, "Simulation/Speed/Slow") == 0)
    {
        m_simulation_speed= SLOW;
    }

    else if (strcmp(picked, "Simulation/Speed/Medium") == 0)
    {
        m_simulation_speed= MEDIUM;
    }

    else if (strcmp(picked, "Simulation/Speed/Fast") == 0)
    {
        m_simulation_speed= FAST;
    }

    else if (strcmp(picked, "Help/About...") == 0)
    {
        fl_message("Yet Another Simple 2D - Mobile Robot Simulator\n"
            "YAST-MRS\nAuralius Manurung\n"
            "(manurunga at yandex.com)");
    }

    else if (strcmp(picked, "View/Robot properties...") == 0)
    {
        if (m_properties_window == NULL) {
            m_properties_window = new CPropertiesWindow(640, 480, "Robot properties");
        }
        
        m_properties_window->end();
        m_properties_window->show();
    }
}

void CSimulationWindow::window_cb(Fl_Widget *widget, void *data)
{
    CSimulationWindow *o = (CSimulationWindow*)data;
    o->window_cb_worker();
}

void CSimulationWindow::window_cb_worker()
{
    stop_simulation();
    this->hide();
}