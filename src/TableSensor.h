#ifndef TABLESENSOR_H_INCLUDED
#define TABLESENSOR_H_INCLUDED

#include <stdint.h>

typedef struct TableSensor_s TableSensor_t;
struct TableSensor_s
{
    int16_t t_max;
    int16_t t_min;
    uint16_t t_step;
    int16_t t_under;
    int16_t t_over;
    uint16_t table_len;
    const uint16_t *t_table;
};

#define TABLE_SENSOR_DEF(instance_name, \
                         temperature_table_min, \
                         temperature_table_max, \
                         temperature_table_step, \
                         temperature_if_under, \
                         temperature_if_over, \
                         table_buf_data) \
    static const uint16_t instance_name##table[(temperature_table_max-(temperature_table_min))/temperature_table_step + 1] = table_buf_data; \
    static const TableSensor_t (instance_name) = { \
        .table_len = (temperature_table_max-(temperature_table_min))/temperature_table_step, \
        .t_min = temperature_table_min, \
        .t_max = temperature_table_max, \
        .t_step = temperature_table_step, \
        .t_under = temperature_if_under, \
        .t_over = temperature_if_over, \
        .t_table = instance_name##table, \
    };

int16_t calc_temperature_ntc(TableSensor_t sensor_inst, uint16_t adc_value);
int16_t calc_temperature_ptc(TableSensor_t sensor_inst, uint16_t adc_value);

#endif /* TABLESENSOR_H_INCLUDED */
