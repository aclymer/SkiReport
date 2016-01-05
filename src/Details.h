#pragma once

// Structure type definition for recieved data
typedef struct {
	char* Name;
	char* Snowfall;
	char* Wind;
	char* Temps;
	char* Time;
	char* Weather;
}SKI_DATA;

// Declare recieved data Structure array
SKI_DATA ski_data[10];

void show_details(int);
void hide_details(void *);
void details_layer_set_text(int);