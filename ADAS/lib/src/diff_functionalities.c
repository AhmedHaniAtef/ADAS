#include "../inc/diff_functionalities.h"



bool value_in_range (float_t p_Value, float_t p_Min, float_t p_Max)
{
    if ((p_Min <= p_Value) && (p_Value <= p_Max))
        return true;
    else
        return false;
}

float_t max(float_t p_Value_1, float_t p_Value_2)
{
    if (p_Value_1 > p_Value_2)
        return p_Value_1;
    else
        return p_Value_2;
}

float_t min(float_t p_Value_1, float_t p_Value_2)
{
    if (p_Value_1 < p_Value_2)
        return p_Value_1;
    else
        return p_Value_2;
}
