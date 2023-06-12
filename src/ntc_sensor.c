#include <ntc_sensor.h>

#define TEMPERATURE_TABLE_READ(i) (termo_table[i])

const temperature_table_entry_type termo_table[]  =
{
    223578, 221654, 219668, 217621, 215511, 213341, 211110, 208821,
    206473, 204069, 201611, 199098, 196535, 193922, 191263, 188559,
    185813, 183028, 180206, 177351, 174465, 171553, 168616, 165659,
    162684, 159696, 156696, 153690, 150680, 147669, 144661, 141659,
    138666, 135686, 132721, 129774, 126848, 123946, 121070, 118223,
    115406, 112623, 109874, 107162, 104489, 101856, 99264, 96714,
    94209, 91748, 89332, 86963, 84641, 82365, 80138, 77958,
    75826, 73742, 71706, 69718, 67777, 65883, 64037, 62236,
    60481, 58772, 57108, 55487, 53910, 52375, 50883, 49431,
    48020, 46649, 45316, 44021, 42763, 41542, 40356, 39204,
    38086, 37001, 35948, 34926, 33935, 32973, 32040, 31134,
    30256, 29405, 28579, 27778, 27001, 26248, 25517, 24809,
    24122, 23455, 22809, 22183, 21576, 20987, 20415, 19861,
    19324, 18803, 18298, 17808, 17333, 16872, 16424, 15990,
    15570, 15161, 14765, 14381, 14008, 13646, 13294, 12953,
    12622, 12301, 11989, 11687, 11393, 11107, 10830, 10561,
    10299, 10045, 9799, 9559, 9326, 9100, 8880, 8667
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
