#ifndef NTC_SENSOR_H_INCLUDED
#define NTC_SENSOR_H_INCLUDED

#include <stdio.h>

#define TEMPERATURE_UNDER 150
#define TEMPERATURE_OVER 150
#define TEMPERATURE_TABLE_START -40
#define TEMPERATURE_TABLE_STEP 1
#define TEMPERATURE_TABLE_SIZE 166

typedef uint32_t temperature_table_entry_type;
typedef uint8_t temperature_table_index_type;

int16_t calc_temperature(temperature_table_entry_type adcsum) ;

#endif /* NTC_SENSOR_H_INCLUDED */
