#include <pebble.h>
#include "settings.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_icon_up_arrow;
static GBitmap *s_res_icon_ok;
static GBitmap *s_res_icon_down_arrow;
static GFont s_res_gothic_24_bold;
static ActionBarLayer *settings_action_bar;
static TextLayer *settings_text_layer;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorClear);
  
  s_res_icon_up_arrow = gbitmap_create_with_resource(RESOURCE_ID_ICON_UP_ARROW);
  s_res_icon_ok = gbitmap_create_with_resource(RESOURCE_ID_ICON_OK);
  s_res_icon_down_arrow = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN_ARROW);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
  // settings_action_bar
  settings_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(settings_action_bar, s_window);
  action_bar_layer_set_background_color(settings_action_bar, GColorClear);
  action_bar_layer_set_icon(settings_action_bar, BUTTON_ID_UP, s_res_icon_up_arrow);
  action_bar_layer_set_icon(settings_action_bar, BUTTON_ID_SELECT, s_res_icon_ok);
  action_bar_layer_set_icon(settings_action_bar, BUTTON_ID_DOWN, s_res_icon_down_arrow);
  layer_add_child(window_get_root_layer(s_window), (Layer *)settings_action_bar);
  
  // settings_text_layer
  settings_text_layer = text_layer_create(GRect(24, 64, 80, 30));
  text_layer_set_background_color(settings_text_layer, GColorBlack);
#ifdef PBL_PLATFORM_BASALT
	text_layer_set_text_color(settings_text_layer, GColorYellow);
#else
  text_layer_set_text_color(settings_text_layer, GColorWhite);
#endif
  text_layer_set_text(settings_text_layer, unit_text[units]);
  text_layer_set_text_alignment(settings_text_layer, GTextAlignmentCenter);
  text_layer_set_font(settings_text_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)settings_text_layer);
}

static void destroy_ui(void) {
	free(unit_text[0]);
	free(unit_text[1]);
  window_destroy(s_window);
  action_bar_layer_destroy(settings_action_bar);
  text_layer_destroy(settings_text_layer);
  gbitmap_destroy(s_res_icon_up_arrow);
  gbitmap_destroy(s_res_icon_ok);
  gbitmap_destroy(s_res_icon_down_arrow);
}
// END AUTO-GENERATED UI CODE

void settings_up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (units != 1)
		units = 1;
	else if (units != 0)
		units = 0;

	text_layer_set_text(settings_text_layer, unit_text[units]);
}

void settings_down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	if (units != 1)
		units = 1;
	else if (units != 0)
		units = 0;

	text_layer_set_text(settings_text_layer, unit_text[units]);
}

void settings_select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
	// For Select Feature - Set Settings & return to main Window
	APP_LOG(APP_LOG_LEVEL_INFO, "Units changed to: %s", unit_text[units]);
	if (units != unit_flag)
	{
		send_cmd(TupletInteger(7, units));
		persist_write_int(0, units);
	}
	hide_settings();
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

static void handle_window_unload(Window* window)
{
	destroy_ui();
}

void show_settings(void)
{	
	unit_text[0]		= malloc(7 * sizeof(char));
	unit_text[1]		= malloc(7 * sizeof(char));

	strcpy(unit_text[0], "  US  ");
	strcpy(unit_text[1], "METRIC");
	unit_flag = units;
	initialise_ui();
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	action_bar_layer_set_click_config_provider(settings_action_bar, settings_click_config_provider);
	window_stack_push(s_window, true);
}

void hide_settings(void)
{
	window_stack_remove(s_window, true);
}
