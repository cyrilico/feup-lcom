#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "score.h"

Score* create_score(unsigned int new_points_minutes, unsigned int new_points_seconds, unsigned long* new_time, unsigned long* new_date, char* new_name){
	Score* new_score = (Score*)(malloc(sizeof(Score)));
	new_score->points_minutes = new_points_minutes;
	new_score->points_seconds = new_points_seconds;
	int i;
	for(i = 0; i < TIME_LENGTH; i++)
		new_score->time[i] = new_time[i];
	for(i = 0; i < DATE_LENGTH; i++)
		new_score->date[i] = new_date[i];
	for(i = 0; i < NAME_LENGTH; i++)
		new_score->name[i] = new_name[i];

	return new_score;
}

void set_score_name(Score* score, char* new_name){
	int i;
	for(i = 0; i < NAME_LENGTH; i++)
		score->name[i] = new_name[i];
}

void draw_score(Score* score, int x, int y, char* buffer){
	int position_x = x;
	int i;
	//Draw score name
	for(i = 0; i < NAME_LENGTH; i++){
		draw_letter(score->name[i],position_x,y,buffer);
		position_x += SCORE_BITMAP_WIDTH;
	}
	position_x += SCORE_BITMAP_WIDTH;

	//Draw score date
	for(i = 0; i < 3; i++){
		draw_score_number(score->date[i],position_x,y,buffer);
		position_x += 2*SCORE_BITMAP_WIDTH; //2 numbers were drawn in above call
		if(i != 2){ //Hasn't drawn year yet
			draw_slash(position_x, y, buffer);
			position_x += SCORE_BITMAP_WIDTH;
		}
	}
	position_x += SCORE_BITMAP_WIDTH;

	//Draw score time (SECONDS ARE NOT DRAWN TO ALLOW MORE SPACE FOR SCORE)
	for(i = 0; i < 2; i++){
		draw_score_number(score->time[i],position_x,y,buffer);
		position_x += 2*SCORE_BITMAP_WIDTH; //2 numbers were drawn in above call
		if(i == 0){ //Drawing hours so need a colon after it
			draw_colon(position_x, y, buffer);
			position_x += SCORE_BITMAP_WIDTH;
		}
	}
	position_x += SCORE_BITMAP_WIDTH;

	//Draw score... score
	draw_score_number(score->points_minutes,position_x,y,buffer);
	position_x += 2*SCORE_BITMAP_WIDTH; //2 numbers were drawn in above call
	draw_colon(position_x,y,buffer);
	position_x += SCORE_BITMAP_WIDTH;
	draw_score_number(score->points_seconds,position_x,y,buffer);
}

void draw_scores(Score** scores, int x, int y, char* buffer){
	int i = 0;
	while(scores[i] != NULL){
		draw_score(scores[i],x,y + i*HIGHSCORE_GAP,buffer);
		if(++i == MAX_SCORES_ON_SCREEN)
			break;
	}
}

void write_score_to_file(Score* score){
	FILE* scoresfile = fopen(SCORESFILE,"a");
	fprintf(scoresfile,"%s %d %d %d %d %d %d %d %d\n", score->name, score->date[0], score->date[1], score->date[2], score->time[0], score->time[1], score->time[2], score->points_minutes, score->points_seconds); /*Order: name day month year hour minutes seconds score_minutes score_seconds*/
	fclose(scoresfile);
}

Score** read_scores_from_file(){
	Score** result = (Score**)(malloc(MAX_SCORES_READ*sizeof(Score*)));
	FILE* scoresfile = fopen(SCORESFILE, "r");
	unsigned long* time = (unsigned long*)malloc(3*sizeof(unsigned long));
	unsigned long* date = (unsigned long*)malloc(3*sizeof(unsigned long));
	unsigned int* score_minutes = (unsigned int*)malloc(sizeof(unsigned int));
	unsigned int* score_seconds = (unsigned int*)malloc(sizeof(unsigned int));
	char name[NAME_LENGTH];
	int i = 0;
	while (fscanf(scoresfile,"%s %d %d %d %d %d %d %d %d\n", name, &date[0], &date[1], &date[2], &time[0], &time[1], &time[2], score_minutes, score_seconds) != EOF) {
		result[i++] = create_score(*score_minutes,*score_seconds,time,date,name);
		if(feof(scoresfile))
			break;
	}
	result[i] = NULL;
	qsort(result,i,sizeof(Score*),comp_score);
	return result;
}

int comp_score(const void* s1, const void* s2) {
	Score* ss1 = *(Score**)s1;
	Score* ss2 = *(Score**)s2;
	if( (60*ss1->points_minutes + ss1->points_seconds) > (60*ss2->points_minutes + ss2->points_seconds))
		return -1;
	if( (60*ss1->points_minutes + ss1->points_seconds) == (60*ss2->points_minutes + ss2->points_seconds))
		return 0;
	return 1;
}


void delete_score(Score* score){
	free(score);
}
