#pragma once

#define CURRENT_TIME 0
#define CURRENT_DATE 1

extern unsigned long* result_date;
extern unsigned long* result_time;
extern unsigned long* rtc_get();

int rtc_subscribe_int(); //does it need to return the BIT of the hookid? not sure until I integrate it in the dispatcher class (may need it even if it's only to clear interrupt from register C)
int rtc_unsubscribe_int();
/*unsigned long* rtc_get(int time_or_date);*/
