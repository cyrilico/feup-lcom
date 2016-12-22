#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#define SCORESFILE fullPath("highscores.txt")
#define MAX_SCORES_READ 15
#define NAME_LENGTH 4
#define DATE_LENGTH 3
#define TIME_LENGTH 3

typedef struct{
	unsigned int points_minutes;
	unsigned int points_seconds;
	unsigned long time[DATE_LENGTH];
	unsigned long date[TIME_LENGTH];
	char name[NAME_LENGTH];
}Score;

Score* create_score(unsigned int new_points_minutes, unsigned int new_points_seconds, unsigned long* new_time, unsigned long* new_date, char* new_name);
void set_score_name(Score* score, char* new_name);
void write_score_to_file(Score* score);
Score** read_scores_from_file();
void delete_score(Score* score);
