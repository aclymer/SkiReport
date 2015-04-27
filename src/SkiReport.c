#include "SkiReport.h"
#include "uthash.h"

struct my_struct {
    const char *name;          /* key */
    int id;
    UT_hash_handle hh;         /* makes this structure hashable */
};

void areas(int number)
{
	num_of_areas = number;
}

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data)
{
	return num_of_areas;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
	return 4;
}

// A callback is used to specify the height of the section header
int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
	// This is a define provided in pebble.h that you may use for the default height
	return 10;
}

// A callback is used to specify the height of the cell
int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
	// This is a define provided in pebble.h that you may use for the default height
	return 20; 
}

int16_t menu_get_separator_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
	return 1;
}

// Here we draw what each header is
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data)
{
	// Deterine which section we're working with
	switch(section_index)
	{
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Loading...");
		break;
	}
}

// This is the menu item draw callback where you specify what each item should look like
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
	switch(cell_index->row)
	{
		case 1:
			menu_cell_basic_draw(ctx, cell_layer, "Wind:", "wind data", NULL);
		break;
	}
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
	switch(cell_index->row)
	{
		case 1:
		break;
	}
}

void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
	switch (key)
	{		
		case NUM_OF_AREAS_KEY:
			num_of_areas = new_tuple->value->uint8;
		break;
		
		case AREA_NAME_KEY:
//			text_layer_set_text(resort_name_layer, new_tuple->value->cstring);
		break;
		
		case WIND_KEY:
//			text_layer_set_text(wind_layer, new_tuple->value->cstring);
		break;
		
		case AREA_TEMPS_KEY:
//			text_layer_set_text(temps_layer, new_tuple->value->cstring);
		break;
		
		case AREA_SNOWFALL_KEY:
//			text_layer_set_text(snowfall_layer, new_tuple->value->cstring);
		break;
		
		case UPDATE_TIME_KEY:
//			text_layer_set_text(update_layer, new_tuple->value->cstring);
		break;	  
		
		case WEATHER_DESC_KEY:
//			text_layer_set_text(weather_layer, new_tuple->value->cstring);
		break;
	}
	
	menu_layer_reload_data(menu_layer);
}

void send_cmd(int details)
{
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	Tuplet value = TupletInteger(0, index);
	Tuplet detail = TupletInteger(8, details);
	Tuplet unit = TupletInteger(7, units);	
	dict_write_tuplet(iter, &value);
	dict_write_tuplet(iter, &detail);
	dict_write_tuplet(iter, &unit);
	
	dict_write_end(iter);
	
	app_message_outbox_send();
	
	return;
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	// For Select Feature - Select Resort and show data
	window_stack_push(settings, true);
}

void settings_up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (units == 1)
	{
		text_layer_set_font(settings_US_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		text_layer_set_font(settings_METRIC_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	}
	units = 0;
}

void settings_down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (units == 0)
	{
		text_layer_set_font(settings_US_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
		text_layer_set_font(settings_METRIC_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	}
	units = 1;
}

void settings_select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	// For Select Feature - Set Settings & return to main Window
	// send_cmd(1);
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context)
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Dictionary Result Error: %d", dict_error);
	//if (app_message_error == 64) {send_cmd(2);}
}

void click_config_provider(void *context)
{	
	//	Single Click Up Action
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	
	//	Single Click Down
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
	
	//	Single Click Select
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

void settings_click_config_provider(void *context)
{	
	//	Single Click Up Action
	window_single_click_subscribe(BUTTON_ID_UP, settings_up_single_click_handler);
	
	//	Single Click Down
	window_single_click_subscribe(BUTTON_ID_DOWN, settings_down_single_click_handler);
	
	//	Single Click Select
	window_single_click_subscribe(BUTTON_ID_SELECT, settings_select_single_click_handler);
}

void settings_load(Window *settings)
{
	Layer *settings_layer = window_get_root_layer(settings);
	
	//Set up Settings Action Bar
	up_arrow_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_UP_ARROW);
	down_arrow_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN_ARROW);
	ok_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_OK);
	settings_action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(settings_action_bar, settings);
	action_bar_layer_set_background_color(settings_action_bar, GColorWhite);
	action_bar_layer_set_click_config_provider(settings_action_bar, settings_click_config_provider);
	action_bar_layer_set_icon(settings_action_bar, BUTTON_ID_UP, up_arrow_icon);
	action_bar_layer_set_icon(settings_action_bar, BUTTON_ID_DOWN, down_arrow_icon);
	action_bar_layer_set_icon(settings_action_bar, BUTTON_ID_SELECT, ok_icon);
	
	// Settings US Text Layer
	settings_US_text_layer = text_layer_create(GRect(20, 32, 80, 30));
	text_layer_set_text_color(settings_US_text_layer, GColorWhite);
	text_layer_set_background_color(settings_US_text_layer, GColorClear);
	if (units == 0) {
		text_layer_set_font(settings_US_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	}
	else {
		text_layer_set_font(settings_US_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	}
	text_layer_set_text_alignment(settings_US_text_layer, GTextAlignmentCenter);
	text_layer_set_text(settings_US_text_layer, "US");
	layer_add_child(settings_layer, text_layer_get_layer(settings_US_text_layer));
	
	// Settings Metric Text Layer
	settings_METRIC_text_layer = text_layer_create(GRect(20, 82, 80, 30));
	text_layer_set_text_color(settings_METRIC_text_layer, GColorWhite);
	text_layer_set_background_color(settings_METRIC_text_layer, GColorClear);
	if (units == 0) {
		text_layer_set_font(settings_METRIC_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	}
	else {
		text_layer_set_font(settings_METRIC_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	}
	text_layer_set_text_alignment(settings_METRIC_text_layer, GTextAlignmentCenter);
	text_layer_set_text(settings_METRIC_text_layer, "METRIC");
	layer_add_child(settings_layer, text_layer_get_layer(settings_METRIC_text_layer));
}

void window_load(Window *window)
{	
	// Set all the callbacks for the menu layer
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
								 .get_num_sections 		= menu_get_num_sections_callback,
								 .get_num_rows 			= menu_get_num_rows_callback,
								 .get_header_height 	= menu_get_header_height_callback,
								 .get_separator_height 	= menu_get_separator_height_callback,
								 .get_cell_height		= menu_get_cell_height_callback,
								 .draw_header 			= menu_draw_header_callback,
								 .draw_row 				= menu_draw_row_callback,
								 .select_click 			= menu_select_callback,
							 });
	
	// Bind the menu layer's click config provider to the window for interactivity
	menu_layer_set_click_config_onto_window(menu_layer, window);	
	
	#ifdef PBL_PLATFORM_BASALT
		menu_layer_shadow_enable(menu_layer, true);
	#endif
	
	// Load custom font	
	header_font 		= fonts_load_custom_font(FONT_KEY_GOTHIC_18);
	cell_font 			= fonts_load_custom_font(FONT_KEY_GOTHIC_24);
	subtitle_font 		= fonts_load_custom_font(FONT_KEY_GOTHIC_24);
		
	//Set up Window Action Bar
	up_arrow_icon 		= gbitmap_create_with_resource(RESOURCE_ID_ICON_UP_ARROW);
	down_arrow_icon 	= gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN_ARROW);
	settings_icon 		= gbitmap_create_with_resource(RESOURCE_ID_ICON_SETTINGS);
	
	/*
	window_action_bar 	= action_bar_layer_create();
	
	action_bar_layer_add_to_window(window_action_bar, window);
	action_bar_layer_set_background_color(window_action_bar, GColorBlack);
	action_bar_layer_set_click_config_provider(window_action_bar, click_config_provider);
	action_bar_layer_set_icon(window_action_bar, BUTTON_ID_SELECT, settings_icon);
	
	// Resort Name Layer
	resort_name_layer = text_layer_create(GRect(0, it++ * row_height, text_layer_width, row_height));
	text_layer_set_text_color(resort_name_layer, GColorWhite);
	text_layer_set_background_color(resort_name_layer, GColorClear);
	text_layer_set_font(resort_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(resort_name_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(resort_name_layer));
	
	// Weather Description Layer
	weather_layer = text_layer_create(GRect(0, it++ * row_height, text_layer_width, row_height));
	text_layer_set_text_color(weather_layer, GColorWhite);
	text_layer_set_background_color(weather_layer, GColorClear);
	text_layer_set_font(weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(weather_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(weather_layer));
	
	// Snowfall Layer	
	snowfall_layer = text_layer_create(GRect(0, it++ * row_height, text_layer_width, row_height));
	text_layer_set_text_color(snowfall_layer, GColorWhite);
	text_layer_set_background_color(snowfall_layer, GColorClear);
	text_layer_set_font(snowfall_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(snowfall_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(snowfall_layer));
	
	// Temperature Layer
	temps_layer = text_layer_create(GRect(0, it++ * row_height, text_layer_width, row_height));
	text_layer_set_text_color(temps_layer, GColorWhite);
	text_layer_set_background_color(temps_layer, GColorClear);
	text_layer_set_font(temps_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(temps_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(temps_layer));
	
	// Wind Layer	
	wind_layer = text_layer_create(GRect(0, it++ * row_height, text_layer_width, row_height));
	text_layer_set_text_color(wind_layer, GColorWhite);
	text_layer_set_background_color(wind_layer, GColorClear);
	text_layer_set_font(wind_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(wind_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(wind_layer));
	
	// Update Time Layer
	update_layer = text_layer_create(GRect(0, it++ * row_height + 4, text_layer_width, row_height));
	text_layer_set_text_color(update_layer, GColorWhite);
	text_layer_set_background_color(update_layer, GColorClear);
	text_layer_set_font(update_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text_alignment(update_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(update_layer));
	*/
	
	Tuplet initial_values[] = {
		TupletInteger(INDEX_KEY, (uint8_t) 0),
		TupletCString(AREA_NAME_KEY, "Loading..."),
		TupletCString(WEATHER_DESC_KEY, "Chance of Snow"),
		TupletCString(WIND_KEY, "Wind"),
		TupletCString(AREA_TEMPS_KEY, "Temp"),
		TupletCString(AREA_SNOWFALL_KEY, "Snow(24h)"),
		TupletCString(UPDATE_TIME_KEY, "Updated @"),
		TupletInteger(UNITS_KEY, (uint8_t) 1),
		TupletCString(PREVIOUS_AREA_NAME_KEY, ""),
		TupletCString(NEXT_AREA_NAME_KEY, ""),
		TupletInteger(NUM_OF_AREAS_KEY, (uint8_t) 1),
	};
	
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_tuple_changed_callback, sync_error_callback, NULL);
}

void window_appear(Window *window)
{
	send_cmd(1);
}

void window_disappear(Window *window)
{
	layer_mark_dirty(window_get_root_layer(window));
}

void settings_disappear(Window *settings)
{
	persist_write_int(0, units);
	layer_mark_dirty(window_get_root_layer(settings));	
}

void settings_unload(Window *settings)
{
	text_layer_destroy(settings_US_text_layer);
	text_layer_destroy(settings_METRIC_text_layer);
	action_bar_layer_destroy(settings_action_bar);
}

void window_unload(Window *window)
{
	gbitmap_destroy(settings_icon);
}

void init(void)
{	
	// Create Main Window & Load
	window = window_create();
	
	// Set up MenuLayer
	menu_layer = menu_layer_create(layer_get_frame(window_get_root_layer(window)));	
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
	
	// Create Settings Window & Load
	settings = window_create();	
	
	window_set_window_handlers(settings, (WindowHandlers) {
		.load 		= settings_load,
		.disappear 	= settings_disappear,
		.unload 	= settings_unload
	});
	
	window_set_window_handlers(window, (WindowHandlers) {
		.load 		= window_load,
		.appear 	= window_appear,
		.disappear 	= window_disappear,
		.unload 	= window_unload
	});
	
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	//window_stack_push(settings, false);
	window_stack_push(window, true);
}

void deinit(void)
{
  menu_layer_destroy(menu_layer);
	window_destroy(window);
	window_destroy(settings);
}

int main(void)
{
	units = persist_exists(0) ? persist_read_int(0) : 0;
	init();
	//window_stack_remove(settings, false);
	//window_stack_remove(window, false);
	app_event_loop();
	deinit();
	app_sync_deinit(&sync);
	
	return 0;
}