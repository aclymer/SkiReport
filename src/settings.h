#pragma once
#include <pebble.h>

extern bool units;
extern void send_cmd(Tuplet);

bool unit_flag;
char *unit_text[2];

void show_settings(void);
void hide_settings(void);
void settings_click_config_provider(void *);
void settings_up_single_click_handler(ClickRecognizerRef, void *);
void settings_down_single_click_handler(ClickRecognizerRef, void *);
void settings_select_single_click_handler(ClickRecognizerRef, void *);