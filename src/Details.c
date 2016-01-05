#include <pebble.h>
#include "Details.h"

void details_layer_set_text(int);
	
static Window *s_window;
static GFont s_res_gothic_24;

static GBitmap *s_res_snowing;
static BitmapLayer *snow_bitmap_layer;
	
static TextLayer *weather_layer;
static TextLayer *snowfall_layer;
static TextLayer *temps_layer;
static TextLayer *wind_layer;
static TextLayer *time_layer;
static TextLayer *area_name_layer;

static int index;

static void click_down_callback(ClickRecognizerRef ref, void *ctx)
{
	index++;
	if (index >= (int)ARRAY_LENGTH(ski_data))
		details_layer_set_text(0);
	else
		details_layer_set_text(index);
}

static void click_up_callback(ClickRecognizerRef ref, void *ctx)
{
	index--;
	if (index < 0)
		details_layer_set_text((int)ARRAY_LENGTH(ski_data) - 1);
	else
		details_layer_set_text(index);
}

static void click_select_callback(ClickRecognizerRef ref, void *ctx)
{
	window_stack_remove(s_window, true);
}


static void click_config_provider_callback(void *ctx)
{
	window_single_click_subscribe(BUTTON_ID_UP, click_up_callback);
	window_single_click_subscribe(BUTTON_ID_DOWN, click_down_callback);
	window_single_click_subscribe(BUTTON_ID_SELECT, click_select_callback);
}

static void initialise_ui(void) {
	s_window = window_create();
	
#ifdef PBL_PLATFORM_BASALT
	window_set_background_color(s_window, GColorCobaltBlue);
#endif

	s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

	// snow_bitmap_layer
	snow_bitmap_layer = bitmap_layer_create(GRect(-6, 58, 150, 120));
	s_res_snowing = gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
	bitmap_layer_set_bitmap(snow_bitmap_layer, s_res_snowing);
	layer_add_child(window_get_root_layer(s_window), (Layer *)snow_bitmap_layer);

	// area_name_layer
	area_name_layer = text_layer_create(GRect(0, 0, 144, 26));
	text_layer_set_background_color(area_name_layer, GColorClear);
	text_layer_set_text(area_name_layer, "SkiReport");
	text_layer_set_text_alignment(area_name_layer, GTextAlignmentCenter);
	text_layer_set_font(area_name_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)area_name_layer);
	
	// weather_layer
	weather_layer = text_layer_create(GRect(0, 28, 144, 26));
	text_layer_set_background_color(weather_layer, GColorClear);
	text_layer_set_text(weather_layer, "by A.Clymer");
	text_layer_set_text_alignment(weather_layer, GTextAlignmentCenter);
	text_layer_set_font(weather_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)weather_layer);

	// snowfall_layer
	snowfall_layer = text_layer_create(GRect(0, 56, 144, 26));
	text_layer_set_background_color(snowfall_layer, GColorClear);
	text_layer_set_text_alignment(snowfall_layer, GTextAlignmentCenter);
	text_layer_set_font(snowfall_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)snowfall_layer);

	// temps_layer
	temps_layer = text_layer_create(GRect(0, 84, 144, 26));
	text_layer_set_background_color(temps_layer, GColorClear);
	text_layer_set_text_alignment(temps_layer, GTextAlignmentCenter);
	text_layer_set_font(temps_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)temps_layer);

	// wind_layer
	wind_layer = text_layer_create(GRect(0, 110, 144, 26));
	text_layer_set_background_color(wind_layer, GColorClear);
	text_layer_set_text_alignment(wind_layer, GTextAlignmentCenter);
	text_layer_set_font(wind_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)wind_layer);

	// time_layer
	time_layer = text_layer_create(GRect(0, 138, 144, 24));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)time_layer);

}

static void destroy_ui(void) {
	text_layer_destroy(weather_layer);
	text_layer_destroy(snowfall_layer);
	text_layer_destroy(temps_layer);
	text_layer_destroy(wind_layer);
	text_layer_destroy(time_layer);
	text_layer_destroy(area_name_layer);		
	
	gbitmap_destroy(s_res_snowing);
	bitmap_layer_destroy(snow_bitmap_layer);
	
	window_destroy(s_window);	
}

static void handle_window_unload(Window* window) {
	destroy_ui();
}

void show_details(int index) {
	initialise_ui();
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	window_set_click_config_provider(s_window, click_config_provider_callback);
	details_layer_set_text(index);
	window_stack_push(s_window, true);
}

void hide_details(void *data) {
	window_stack_remove(s_window, true);
}


void details_layer_set_text(int index)
{	
	text_layer_set_text(area_name_layer, ski_data[index].Name);	
	text_layer_set_text(snowfall_layer, ski_data[index].Snowfall);	
	text_layer_set_text(weather_layer, ski_data[index].Weather);	
	text_layer_set_text(wind_layer, ski_data[index].Wind);	
	text_layer_set_text(temps_layer, ski_data[index].Temps);	
	text_layer_set_text(time_layer, ski_data[index].Time);

	text_layer_set_text_color(area_name_layer, GColorOrange);
	text_layer_set_text_color(weather_layer, GColorOrange);	
	text_layer_set_text_color(snowfall_layer, GColorPastelYellow);	
	text_layer_set_text_color(temps_layer, GColorIcterine);	
	text_layer_set_text_color(wind_layer, GColorBlack);	
	text_layer_set_text_color(time_layer, GColorDarkGray);
}