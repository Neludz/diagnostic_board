#include <ntc_sensor.h>

#define TEMPERATURE_TABLE_READ(i) (termo_table[i])

const temperature_table_entry_type termo_table[TEMPERATURE_TABLE_SIZE]  =
{
    934, 928, 921, 914, 907, 900, 892, 884,
    875, 866, 857, 848, 838, 827, 817, 806,
    794, 783, 771, 759, 746, 734, 721, 708,
    694, 681, 667, 653, 639, 625, 611, 597,
    582, 568, 554, 540, 526, 512, 498, 484,
    470, 457, 443, 430, 417, 405, 392, 380,
    368, 356, 345, 333, 323, 312, 301, 291,
    281, 272, 263, 253, 245, 236, 228, 220,
    212, 205, 198, 191, 184, 177, 171, 165,
    159, 153, 148, 143, 138, 133, 128, 124,
    119, 115, 111, 107, 103, 100, 96, 93,
    90, 87, 84, 81, 78, 75, 73, 70,
    68, 66, 63, 61, 59, 57, 55, 54,
    52, 50, 48, 47, 45, 44, 43, 41,
    40, 39, 37, 36, 35, 34, 33, 32,
    31, 30, 29, 28, 27, 27, 26, 25,
    24, 24, 23, 22, 22, 21, 20, 20,
    19, 19, 18, 18, 17, 17, 16, 16,
    15, 15, 15, 14, 14, 13, 13, 13,
    12, 12, 12, 11, 11, 11, 11, 10,
    10, 10, 10, 9, 9, 9
};

int16_t calc_temperature(temperature_table_entry_type adcsum)
{
    temperature_table_index_type l = 0;
    temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
    temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
    int32_t TEMPERATURE_TABLE_START_RATE;
    int32_t TEMPERATURE_TABLE_STEP_RATE;

    TEMPERATURE_TABLE_START_RATE = TEMPERATURE_TABLE_START;
    TEMPERATURE_TABLE_STEP_RATE = TEMPERATURE_TABLE_STEP;

    if (adcsum <= thigh)
    {
#ifdef TEMPERATURE_UNDER
        if (adcsum < thigh)
            return TEMPERATURE_OVER;
#endif
        return TEMPERATURE_TABLE_STEP_RATE * r + TEMPERATURE_TABLE_START_RATE;
    }
    temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
    if (adcsum >= tlow)
    {
#ifdef TEMPERATURE_OVER
        if (adcsum > tlow)
            return TEMPERATURE_UNDER;
#endif
        return TEMPERATURE_TABLE_START_RATE;
    }

    while ((r - l) > 1)
    {
        temperature_table_index_type m = (l + r) >> 1;
        temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
        if (adcsum > mid)
        {
            r = m;
        }
        else
        {
            l = m;
        }
    }
    temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
    if (adcsum >= vl)
    {
        return l * TEMPERATURE_TABLE_STEP_RATE + TEMPERATURE_TABLE_START_RATE;
    }
    temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
    temperature_table_entry_type vd = vl - vr;
    int16_t res = TEMPERATURE_TABLE_START_RATE + r * TEMPERATURE_TABLE_STEP_RATE;
    if (vd)
    {
        res -= ((TEMPERATURE_TABLE_STEP_RATE * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
    }
    return res;
}
