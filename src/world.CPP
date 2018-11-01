#include "world.H"

CWorld::CWorld(const char * f, const AZ_CONFIG *cfg)
{
    m_cfg = cfg;
    m_map_image = new Fl_PNG_Image(f);

    m_width = m_map_image->w();
    m_height = m_map_image->h();

    // Create dynamic memory allocation
    m_gs_map = new unsigned char[m_width * m_height];
    m_binary_map = new bool [m_width * m_height];

    // Convert to grayscale
    const unsigned char *datap = (const unsigned char *)m_map_image->data()[0];
    unsigned char r,g,b;
    for ( int y = 0; y < m_map_image->h(); y++ ) {
        for ( int x = 0; x < m_map_image->w(); x++ ) {
            long index = (y * m_map_image->w() + x) * m_map_image->d();
            r = *(datap + index + 0);
            g = *(datap + index + 1);
            b = *(datap + index + 2);

            if ((0.30 * r + 0.59 * g + 0.11 * b) > 127) // Treshold color
                m_gs_map[x + y * m_width] = 255;        // White (empty)
            else
                m_gs_map[x + y * m_width] = 0;          // Black (objects)

            // Unit grid (1 grid square = 1 meter square)
            if (x % (int) m_cfg->SCALE_FACTOR == 0)
                m_gs_map[x + y * m_width] = 200;
            if (y % (int) m_cfg->SCALE_FACTOR == 0)
                m_gs_map[x + y * m_width] = 200;
        }
    }

    // Create binary map
    for ( int y = 0; y < m_map_image->h(); y++ ) {
        for ( int x = 0; x < m_map_image->w(); x++ ) {
            if (m_gs_map[x + y * m_width] >= 60)
                m_binary_map[x + y * m_width] = false;  // No objects
            else
                m_binary_map[x + y * m_width] = true;;  // Yes objects
        }
    }
}

CWorld::CWorld(int w, int h)
{
    m_gs_map = new unsigned char[w * h];
    m_binary_map = new bool[w * h];
    m_map_image = NULL;

    m_width = w;
    m_height = h;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            m_gs_map[x + y * w] = 255;
            m_binary_map[x + y * w] = false;
        }
    }

}

CWorld::~CWorld(void)
{
	fprintf(stdout, "Cleaning memory [CWorld]\n");
	fflush(stdout);

    delete [] m_gs_map;
    delete [] m_binary_map;

    if (m_map_image)
        delete[] m_map_image;
}


unsigned char* CWorld::get_map()
{
    return m_gs_map;
}

bool* CWorld::get_binary_map()
{
    return m_binary_map;
}

Fl_PNG_Image *CWorld::get_map_image()
{
    return m_map_image;
}

int CWorld::get_height()
{
    return m_height;
}

int CWorld::get_width()
{
    return m_width;
}
