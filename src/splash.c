#include <pebble.h>
#include "splash.h"

static Window *s_window;
static GFont s_res_gothic_24;

static GBitmap *s_res_snowing;
static GBitmapSequence *s_res_snowing_sequence;
static AppTimer *frame_timer;
static BitmapLayer *snow_bitmap_layer;
	
static TextLayer *weather_layer;
static TextLayer *area_name_layer;
static TextLayer *time_layer;

static void initialise_ui(void) {
	s_window = window_create();
	
#ifdef PBL_PLATFORM_BASALT
	window_set_background_color(s_window, GColorCobaltBlue);
#endif

	s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

	// snow_bitmap_layer
	snow_bitmap_layer = bitmap_layer_create(GRect(-6, 58, 150, 120));
	s_res_snowing_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_SPLASH);
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

	// time_layer
	time_layer = text_layer_create(GRect(0, 138, 144, 24));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_text(time_layer, "Loading...");
	text_layer_set_font(time_layer, s_res_gothic_24);
	layer_add_child(window_get_root_layer(s_window), (Layer *)time_layer);

}

static void destroy_ui(void) {
	text_layer_destroy(weather_layer);
	text_layer_destroy(time_layer);
	text_layer_destroy(area_name_layer);
	
	app_timer_cancel(frame_timer);
	gbitmap_sequence_destroy(s_res_snowing_sequence);
	gbitmap_destroy(s_res_snowing);
	bitmap_layer_destroy(snow_bitmap_layer);
	
	window_destroy(s_window);
}

static void next_frame(void *data) {
	uint32_t next_delay;

	if (gbitmap_sequence_update_bitmap_next_frame(s_res_snowing_sequence, s_res_snowing, &next_delay))
	{
		layer_mark_dirty(bitmap_layer_get_layer(snow_bitmap_layer));
		bitmap_layer_set_bitmap(snow_bitmap_layer, s_res_snowing);
	}
	else
		gbitmap_sequence_restart(s_res_snowing_sequence);

	frame_timer = app_timer_register(100, next_frame, NULL);
}

static void handle_window_unload(Window* window) {
	destroy_ui();
}

void show_splash(void) {
	initialise_ui();
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	
	window_stack_push(s_window, true);
	
	frame_timer = app_timer_register(10, next_frame, NULL);
}

void hide_splash(void) {
	window_stack_remove(s_window, true);
}
