#ifndef NTC_SENSOR_H_INCLUDED
#define NTC_SENSOR_H_INCLUDED

#include <stdio.h>

#define TEMPERATURE_UNDER 8888
#define TEMPERATURE_OVER 9999
#define TEMPERATURE_TABLE_START -10
#define TEMPERATURE_TABLE_STEP 1


typedef uint32_t temperature_table_entry_type;
typedef uint8_t temperature_table_index_type;

int16_t calc_temperature(temperature_table_entry_type adcsum) ;

#endif /* NTC_SENSOR_H_INCLUDED */
