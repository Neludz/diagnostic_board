#include "TableSensor.h"
//---------------------------------------------------------------------------------
int16_t calc_temperature_ntc(TableSensor_t sensor_inst, uint16_t adc_value)
{
    uint16_t l = 0;
    uint16_t r = sensor_inst.table_len ;
    uint16_t temp = sensor_inst.t_table[r];

    if (adc_value <= temp)
    {
        if (adc_value < temp)
            return sensor_inst.t_over;
        return sensor_inst.t_max;
    }
    temp = sensor_inst.t_table[0];
    if (adc_value >= temp)
    {
        if (adc_value > temp)
            return sensor_inst.t_under;
        return sensor_inst.t_min;
    }
    while ((r - l) > 1)
    {
        uint16_t m = (l + r) >> 1;
        uint16_t mid = sensor_inst.t_table[m];
        if (adc_value > mid)
            r = m;
        else
            l = m;
    }
    uint16_t vl = sensor_inst.t_table[l];
    if (adc_value >= vl)
    {
        return l * sensor_inst.t_step + sensor_inst.t_min;
    }
    uint16_t vr = sensor_inst.t_table[r];
    uint16_t vd = vl - vr;
    int16_t res = sensor_inst.t_min + r * sensor_inst.t_step;
    if (vd)
    {
        res -= ((sensor_inst.t_step * (int32_t)(adc_value - vr) + (vd >> 1)) / vd);
    }
    return res;
}
//---------------------------------------------------------------------------------
int16_t calc_temperature_ptc(TableSensor_t sensor_inst, uint16_t adc_value)
{
    uint16_t l = 0;
    uint16_t r = sensor_inst.table_len;
    uint16_t temp = sensor_inst.t_table[r];

    if (adc_value >= temp)
    {
        if (adc_value > temp)
            return sensor_inst.t_over;
        return sensor_inst.t_max;
    }
    temp = sensor_inst.t_table[0];
    if (adc_value <= temp)
    {
        if (adc_value < temp)
            return sensor_inst.t_under;
        return sensor_inst.t_min;
    }
    while ((r - l) > 1)
    {
        uint16_t m = (l + r) >> 1;
        uint16_t mid = sensor_inst.t_table[m];
        if (adc_value < mid)
            r = m;
        else
            l = m;
    }
    uint16_t vl = sensor_inst.t_table[l];
    if (adc_value <= vl)
    {
        return l * sensor_inst.t_step + sensor_inst.t_min;
    }
    uint16_t vr = sensor_inst.t_table[r];
    uint16_t vd = vr - vl;
    int16_t res = sensor_inst.t_min + r * sensor_inst.t_step;
    if (vd)
    {
        res -= ((sensor_inst.t_step * (int32_t)(vr - adc_value) + (vd >> 1)) / vd);
    }
    return res;
}
//---------------------------------------------------------------------------------
