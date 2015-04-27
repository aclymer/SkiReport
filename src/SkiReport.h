#include <pebble.h>
	
Window 			*window;
MenuLayer 		*menu_layer;
GBitmap 		*settings_icon;

Window 			*settings;
TextLayer 		*settings_US_text_layer;
TextLayer 		*settings_METRIC_text_layer;
GBitmap 		*ok_icon;
GBitmap 		*up_arrow_icon;
GBitmap 		*down_arrow_icon;
ActionBarLayer 	*settings_action_bar;

uint16_t		num_of_areas = 1;
int 			units;
int 			index = 0;
AppSync 		sync;
uint8_t 		sync_buffer[4096];
GFont 			header_font;
GFont 			cell_font;
GFont 			subtitle_font;

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
	PREVIOUS_AREA_NAME_KEY	= 9,	// TUPLE_CSTRING
	NEXT_AREA_NAME_KEY 		= 10,	// TUPLE_CSTRING
	NUM_OF_AREAS_KEY 		= 11	// TUPLE_INTEGER
};

void init(void);
void areas(int);
void deinit(void);
void send_cmd(int);
void window_load(Window *);
void window_appear(Window *);
void settings_load(Window *);
void window_unload(Window *);
void settings_unload(Window *);
void window_disappear(Window *);
void settings_disappear(Window *);
void click_config_provider(void *);
void settings_click_config_provider(void *);
void up_single_click_handler(ClickRecognizerRef, void *);
void down_single_click_handler(ClickRecognizerRef, void *);
void select_single_click_handler(ClickRecognizerRef, void *);
void settings_up_single_click_handler(ClickRecognizerRef, void *);
void settings_down_single_click_handler(ClickRecognizerRef, void *);
void settings_select_single_click_handler(ClickRecognizerRef, void *);

// Menu Header Draw function for Title only
void 	menu_header_simple_draw(GContext *, const Layer *, const char *);

// Menu Header Draw function for Icon and Title
void 	menu_header_simple_icon_draw(GContext *, const Layer *, const char *, const GBitmap *);

// Menu Header Draw function for Icon, Title, and Subtitle
void 	menu_header_draw(GContext *, const Layer *, const char *, const char *, const GBitmap *);

// Menu Header Draw function for titles (multiple-lines)
void 	menu_header_long_draw(GContext *, const Layer *, const char *);

// Menu Row Draw function for Title only
void 	menu_cell_simple_draw(GContext *, const Layer *, const char *);

// Menu Row Draw function for Title only (Centered)
void 	menu_cell_simple_centered_draw(GContext *, const Layer *, const char *);

// Menu Row Draw function for Icon and Title
void 	menu_cell_simple_icon_draw(GContext *, const Layer *, const char *, const GBitmap *);

//! Menu row drawing function to draw a basic cell with the title, subtitle, and icon. 
//! Call this function inside the `.draw_row` callback implementation, see \ref MenuLayerCallbacks.
//! @param ctx The destination graphics context
//! @param cell_layer The layer of the cell to draw
//! @param title Draws a title in larger text (18 points, Lucida Console font).
//! @param subtitle Draws a subtitle in smaller text (14 points, Lucida Console font).
//! @param icon Draws an icon to the left of the text.
void 	menu_cell_draw(GContext *, const Layer *, const char *, const char *, const GBitmap *);