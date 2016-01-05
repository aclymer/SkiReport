#include <pebble.h>
#include "splash.h"
#include "Details.h"
#include "settings.h"
	
#undef APP_LOG
#define APP_LOG(...)
	
// Declare Pointers
Window 				*window;
MenuLayer 			*menu_layer;
DictionaryIterator 	*iter;

// Declare Variables
uint16_t			num_of_areas = 0;
int					details = -1;
bool 				units = false;
int					loaded = 0;
int 				index = 0;
int 				cell_height = 22;
AppSync 			sync;
uint8_t 			sync_buffer[1024];
GFont 				header_font;
GFont 				cell_font;
GFont 				subtitle_font;
GRect				detail_layer_bounds;
int 				three_hr_index;

// Declare AppMessage enumerated keys
enum key {
	INDEX_KEY 				= 0,	// TUPLE_INTEGER
	AREA_NAME_KEY 			= 1,	// TUPLE_CSTRING
	WIND_KEY 				= 2,	// TUPLE_CSTRING
	AREA_TEMPS_KEY 			= 3,	// TUPLE_CSTRING
	AREA_SNOWFALL_KEY 		= 4,	// TUPLE_CSTRING
	UPDATE_TIME_KEY 		= 5,	// TUPLE_CSTRING
	WEATHER_DESC_KEY 		= 6,	// TUPLE_CSTRING
	UNITS_KEY 				= 7,	// TUPLE_INTEGER
	DETAILS_KEY 			= 8,	// TUPLE_INTEGER
	LOADED_KEY				= 9,	// TUPLE_CSTRING
	ERROR_KEY		 		= 10,	// TUPLE_CSTRING
	NUM_OF_AREAS_KEY 		= 11	// TUPLE_INTEGER
};

// timedAppMessage *context struct
typedef struct {
	int key;
	int integer;
	char* cstring;
} MSGDATA;

// Declare functions prototypes
void init(void);
void deinit(void);
void send_ACK(int);
void send_cmd(Tuplet);
void window_load(Window *);
void window_appear(Window *);
void window_unload(Window *);
void timedAppMessage(void *);
void window_disappear(Window *);
void click_config_provider(void *);
void up_single_click_handler(ClickRecognizerRef, void *);
void down_single_click_handler(ClickRecognizerRef, void *);
void select_single_click_handler(ClickRecognizerRef, void *);

// Menu Header Draw function for Title only
void menu_header_simple_draw(GContext *, const Layer *, const char *);

// Menu Header Draw function for Icon and Title
void menu_header_simple_icon_draw(GContext *, const Layer *, const char *, const GBitmap *);

// Menu Header Draw function for Icon, Title, and Subtitle
void menu_header_draw(GContext *, const Layer *, const char *, const char *, const GBitmap *);

// Menu Header Draw function for titles (multiple-lines)
void menu_header_long_draw(GContext *, const Layer *, const char *);

// Menu Row Draw function for Title only
void weather_cell_draw(GContext *, const Layer *, MenuIndex *);

// Menu Row Draw function for Title only (Centered)
void menu_cell_simple_centered_draw(GContext *, const Layer *, const char *);

// Menu Row Draw function for Icon and Title
void menu_cell_simple_icon_draw(GContext*, const Layer*, const GBitmap*, int);

//! Menu row drawing function to draw a basic cell with the title, subtitle, and icon. 
//! Call this function inside the `.draw_row` callback implementation, see \ref MenuLayerCallbacks.
//! @param ctx The destination graphics context
//! @param cell_layer The layer of the cell to draw
//! @param title Draws a title in larger text (18 points, Lucida Console font).
//! @param subtitle Draws a subtitle in smaller text (14 points, Lucida Console font).
//! @param icon Draws an icon to the left of the text.
void menu_cell_draw(GContext *, const Layer *, const char *, const char *, const GBitmap *);