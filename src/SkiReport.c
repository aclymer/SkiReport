#include "SkiReport.h"

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
	if (menu_layer_get_selected_index(menu_layer).section == section_index)
		return 1;
	else 
		return 1;
}

// A callback is used to specify the height of the section header
int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
	return 24;
}

// A callback is used to specify the height of the cell
int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
	// This is a define provided in pebble.h that you may use for the default height
	//if (menu_layer_get_selected_index(menu_layer).section == cell_index->section)
		return cell_height;
	//else
	//	return 16;
}

int16_t menu_get_separator_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
	return 0;
}

// Here we draw what each header is
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data)
{
	menu_header_simple_draw(ctx, cell_layer, ski_data[section_index].Name);
}

// This is the menu item draw callback where you specify what each item should look like
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
	weather_cell_draw(ctx, cell_layer, cell_index);
}

void menu_layer_selection_will_change_callback(MenuLayer *menu_Layer, MenuIndex *cell_index, MenuIndex old_index, void *context)
{	
}

void menu_layer_selection_changed_callback(MenuLayer *menu_Layer, MenuIndex cell_index, MenuIndex old_index, void *context)
{
	menu_layer_reload_data(menu_layer);
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{	
	index = cell_index->section;
	show_details(index);
	APP_LOG(APP_LOG_LEVEL_INFO, "Show details...");
}

void menu_long_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
	show_settings();
}

void app_message_received(DictionaryIterator* dict_in, void* context)
{
	if (dict_in == NULL)
		return;
	
	Tuple* tuple_in = dict_read_first(dict_in);
	
	while (tuple_in != NULL)
	{
		switch (tuple_in->key)
		{
			case NUM_OF_AREAS_KEY:
				if (num_of_areas != tuple_in->value->uint8)
				{
					APP_LOG(APP_LOG_LEVEL_INFO, "num_of_areas: %u", tuple_in->value->uint8);
					num_of_areas = tuple_in->value->uint8;
				}
			break;
			
			case INDEX_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "index: %u", tuple_in->value->uint8);
				index = tuple_in->value->uint8;
			break;
			
			case AREA_NAME_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "area_name: %d - %s", index, tuple_in->value->cstring);
				ski_data[index].Name = malloc((strlen(tuple_in->value->cstring) + 1) * sizeof(char));
				strcpy(ski_data[index].Name, tuple_in->value->cstring);
			break;
			
			case AREA_SNOWFALL_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "area_snowfall: %s", tuple_in->value->cstring);
				ski_data[index].Snowfall = malloc((strlen(tuple_in->value->cstring) + 1) * sizeof(char));
				strcpy(ski_data[index].Snowfall, tuple_in->value->cstring);
			break;
			
			case WEATHER_DESC_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "weather_desc: %s", tuple_in->value->cstring);
				ski_data[index].Weather = malloc((strlen(tuple_in->value->cstring) + 1) * sizeof(char));
				strcpy(ski_data[index].Weather, tuple_in->value->cstring);
			break;
			
			case WIND_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "wind: %s", tuple_in->value->cstring);
				ski_data[index].Wind = malloc((strlen(tuple_in->value->cstring) + 1) * sizeof(char));
				strcpy(ski_data[index].Wind, tuple_in->value->cstring);
			break;
			
			case AREA_TEMPS_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "area_temps: %s", tuple_in->value->cstring);
				ski_data[index].Temps = malloc((strlen(tuple_in->value->cstring) + 1) * sizeof(char));
				strcpy(ski_data[index].Temps, tuple_in->value->cstring);
			break;
			
			case DETAILS_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "Three hour index: %d", tuple_in->value->uint8);
				details = tuple_in->value->uint8;
			break;
			
			case UPDATE_TIME_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "update_times: %s", tuple_in->value->cstring);
				ski_data[index].Time = malloc((strlen(tuple_in->value->cstring) + 1) * sizeof(char));
				strcpy(ski_data[index].Time, tuple_in->value->cstring);
			break;
			
			case LOADED_KEY:
				APP_LOG(APP_LOG_LEVEL_INFO, "loaded: %d", tuple_in->value->uint8);
				loaded = tuple_in->value->uint8;
				if (tuple_in->value->uint8)
				{
					hide_splash();
					menu_layer_reload_data(menu_layer);
					return;
				}
		}
		
		tuple_in = dict_read_next(dict_in);
	}
	send_ACK(1);
	if (window_stack_get_top_window() == window)
		menu_layer_reload_data(menu_layer);
	
	loaded = 0;
}

void timedAppMessage(void *data)
{	
	MSGDATA *msgData = malloc(sizeof(data));
	
	if (data != NULL)
	{
		msgData = ((MSGDATA*)data);
		if (&(msgData->key) != NULL)
		{
			if (&(msgData->integer) != NULL)
			{
				APP_LOG(APP_LOG_LEVEL_INFO, "Tuple: key - %d,  value - %d", msgData->key, msgData->integer);
				send_cmd(TupletInteger(msgData->key, msgData->integer));
			}
			else if (&(msgData->cstring) != NULL)
			{				
				APP_LOG(APP_LOG_LEVEL_INFO, "Tuple: key - %d,  value - %s", msgData->key, msgData->cstring);
				send_cmd(TupletInteger(msgData->key, *(msgData->cstring)));
			}
		}
	}
	else
	{
		APP_LOG(APP_LOG_LEVEL_INFO, "No data available to send...");
	}
	
	free(msgData);
}

void send_cmd(Tuplet data)
{	
	app_message_outbox_begin(&iter);	
	dict_write_tuplet(iter, &data);	
	dict_write_end(iter);	
	app_message_outbox_send();	
}

void send_ACK(int ACK)
{
	app_message_outbox_begin(&iter);	
	dict_write_tuplet(iter, &TupletInteger(INDEX_KEY, ACK));	
	dict_write_end(iter);	
	app_message_outbox_send();
}

static void app_message_dropped(AppMessageResult app_message_error, void *context)
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped: %i", app_message_error);
	send_ACK(0);
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

void window_load(Window *window)
{	
	for (int i = 0; i < num_of_areas; i++)
	{
		ski_data[i].Name 	 = malloc(16 * sizeof(char));
		ski_data[i].Name 	 = "Loading...";
		ski_data[i].Snowfall = malloc(16 * sizeof(char));
		ski_data[i].Weather	 = malloc(16 * sizeof(char));
		ski_data[i].Weather	 = "Loading...";
		ski_data[i].Wind 	 = malloc(16 * sizeof(char));
		ski_data[i].Temps	 = malloc(16 * sizeof(char));
		ski_data[i].Time	 = malloc(16 * sizeof(char));
	}
	
	// Set all the callbacks for the menu layer
	menu_layer_set_highlight_colors(menu_layer, GColorBlack, GColorChromeYellow);
	menu_layer_set_normal_colors(menu_layer, GColorCobaltBlue, GColorDarkGray);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
								 .get_num_sections 		= menu_get_num_sections_callback,
								 .get_num_rows 			= menu_get_num_rows_callback,
								 .get_header_height 	= menu_get_header_height_callback,
								 .get_separator_height 	= menu_get_separator_height_callback,
								 .get_cell_height		= menu_get_cell_height_callback,
								 .draw_header 			= menu_draw_header_callback,
								 .draw_row 				= menu_draw_row_callback,
								 .select_click 			= menu_select_callback,
								 .selection_will_change	= menu_layer_selection_will_change_callback,
								 .select_long_click		= menu_long_select_callback
							});
	
	// Bind the menu layer's click config provider to the window for interactivity
	menu_layer_set_click_config_onto_window(menu_layer, window);			
}

void window_appear(Window *window)
{
	menu_layer_reload_data(menu_layer);
}

void window_disappear(Window *window)
{
}

void window_unload(Window *window)
{
}

void init(void)
{	
	// Create Main Window & Load
	window 				= window_create();
	
	// Load custom font	
	header_font 		= fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
	cell_font 			= fonts_get_system_font(FONT_KEY_GOTHIC_24);
	subtitle_font 		= fonts_get_system_font(FONT_KEY_GOTHIC_18);
	
	// Set up MenuLayer
	menu_layer = menu_layer_create(layer_get_frame(window_get_root_layer(window)));	
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
	
	window_set_window_handlers(window, (WindowHandlers) {
		.load 				= window_load,
		.appear 			= window_appear,
		.disappear 			= window_disappear,
		.unload 			= window_unload
	});
	
	app_message_register_inbox_received(app_message_received);
	app_message_register_inbox_dropped(app_message_dropped);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	window_stack_push(window, true);
	show_splash();
		
	MSGDATA *appMessage 	= malloc(sizeof(MSGDATA));
	appMessage->key 		= UNITS_KEY;
	appMessage->integer 	= units;
	appMessage->cstring 	= NULL;
	
	app_timer_register(3000, timedAppMessage, (void*)appMessage);
}

void deinit(void)
{
	free(iter);
	for (int i = 0; i < 10; i++)
	{
		free(ski_data[i].Name);
		free(ski_data[i].Weather);
		free(ski_data[i].Snowfall);
		free(ski_data[i].Wind);
		free(ski_data[i].Temps);
		free(ski_data[i].Time);
	}
  
  menu_layer_destroy(menu_layer);
	window_destroy(window);
}

int main(void)
{
	units = persist_exists(0) ? persist_read_int(0) : 1;
	init();
	app_event_loop();
	deinit();
	app_message_deregister_callbacks();

	return 0;
}

// Menu Header Draw function for Title only
void menu_header_simple_draw(GContext* ctx, const Layer *cell_layer, const char *title)
{
	graphics_context_set_text_color(ctx, GColorBlack);
	GRect titleOrigin 			= layer_get_bounds(cell_layer);
	titleOrigin.origin.y		-= 5;
	graphics_draw_text(ctx, title, header_font, titleOrigin, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

// Menu Header Draw function for Icon and Title
void menu_header_simple_icon_draw(GContext* ctx, const Layer *cell_layer, const char *title, const GBitmap* bitmap)
{
	graphics_context_set_text_color(ctx, GColorBlack);
	GRect bitmap_bounds 		= gbitmap_get_bounds(bitmap);
	GRect title_bounds 			= layer_get_bounds(cell_layer);
	bitmap_bounds.origin.x 		+= 2;
	bitmap_bounds.origin.y		= (title_bounds.size.h - bitmap_bounds.size.h) / 2;
	graphics_draw_bitmap_in_rect(ctx, bitmap, bitmap_bounds);
	title_bounds.origin.x 		= bitmap_bounds.size.w + bitmap_bounds.origin.x * 2;
	title_bounds.origin.y 		-= 1;
	graphics_draw_text(ctx, title, header_font, title_bounds, GTextOverflowModeFill, GTextAlignmentLeft, NULL);
}

// Menu Header Draw function for Icon, Title, and Subtitle
void menu_header_draw(GContext* ctx, const Layer *cell_layer, const char *title, const char* subtitle, const GBitmap* bitmap)
{
	graphics_context_set_text_color(ctx, GColorBlack);
	GRect bitmap_bounds 		= gbitmap_get_bounds(bitmap);
	GRect title_bounds 			= layer_get_bounds(cell_layer);
	GRect subtitle_bounds 		= title_bounds;
	bitmap_bounds.origin.x 		+= 2;
	bitmap_bounds.origin.y		= (title_bounds.size.h - bitmap_bounds.size.h) / 2 + 3;
	graphics_draw_bitmap_in_rect(ctx, bitmap, bitmap_bounds);
	title_bounds.origin.x 		= bitmap_bounds.size.w + bitmap_bounds.origin.x * 2;
	title_bounds.origin.y		= ((layer_get_bounds(cell_layer).size.h - bitmap_bounds.size.h - 24) / 2);
	title_bounds.origin.y		-= 1;
	subtitle_bounds.origin.x 	= title_bounds.origin.x;
	subtitle_bounds.origin.y	= subtitle_bounds.size.h / 2;
	graphics_draw_text(ctx, title, header_font, title_bounds, GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	graphics_draw_text(ctx, subtitle, cell_font, subtitle_bounds, GTextOverflowModeFill, GTextAlignmentLeft, NULL);
}

// Menu Row Draw function for Title only
void weather_cell_draw(GContext* ctx, const Layer *cell_layer, MenuIndex *index)
{
#ifdef PBL_PLATFORM_BASALT
	graphics_context_set_text_color(ctx, GColorYellow);
#else
	graphics_context_set_text_color(ctx, GColorWhite);
#endif
	GRect cell_bounds = layer_get_bounds(cell_layer);
	cell_bounds.origin.y			-= 5;
	graphics_draw_text(ctx, ski_data[index->section].Snowfall, header_font, cell_bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

// Menu Row Draw function for Title only (Centered)
void menu_cell_simple_centered_draw(GContext* ctx, const Layer *cell_layer, const char *title)
{
	graphics_context_set_text_color(ctx, GColorBlack);
	GRect titleOrigin = layer_get_bounds(cell_layer);
	titleOrigin.origin.y 		-= 5;
	graphics_draw_text(ctx, title, cell_font, titleOrigin, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

// Menu Row Draw function for Icon and Title
void menu_cell_simple_icon_draw(GContext* ctx, const Layer *cell_layer, const GBitmap* bitmap, int position)
{
	graphics_context_set_text_color(ctx, GColorBlack);
	GRect bitmap_bounds 		= gbitmap_get_bounds(bitmap);
	bitmap_bounds.origin.x 		= bitmap_bounds.size.w * position;
	graphics_draw_bitmap_in_rect(ctx, bitmap, bitmap_bounds);
}