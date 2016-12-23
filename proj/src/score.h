#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
//#define SCORESFILE "/home/lcom/lcom1617-t4g11/proj/src/highscores.txt"
#define SCORESFILE "/home/lcom/casa2/lcom1617-t4g11/proj/src/highscores.txt"

#define MAX_SCORES_READ 15
#define NAME_LENGTH 4
#define DATE_LENGTH 3
#define TIME_LENGTH 3
#define MAX_SCORES_ON_SCREEN 5
#define SCORE_MINUTES_SIZE 2
#define SCORE_SECONDS_SIZE 2

#define SESSION_SCORE_X_START 332
#define SESSION_SCORE_Y 145

#define HIGHSCORE_GAP 30

typedef struct{
	unsigned int points_minutes;
	unsigned int points_seconds;
	unsigned long time[DATE_LENGTH];
	unsigned long date[TIME_LENGTH];
	char name[NAME_LENGTH];
}Score;

Score* create_score(unsigned int new_points_minutes, unsigned int new_points_seconds, unsigned long* new_time, unsigned long* new_date, char* new_name);
void set_score_name(Score* score, char* new_name);
void draw_score(Score* score, int x, int y, char* buffer); //x,y represents the top left corner of the first bitmap to be drawn (rest is automatically calculated)
void draw_scores(Score** scores, int x, int y, char* buffer); //x,y represents the top left corner of the first bitmap to be drawn (rest is automatically calculated)
void write_score_to_file(Score* score);
Score** read_scores_from_file();
int comp_score(const void* s1, const void* s2);
void delete_score(Score* score);

