
#include "ice_internal_headers.h"


Value g_ndet;

/*
ndet: {
  int currentStep
  step[] steps
}

step: {
    int current
    int max
    [optional] Value repr
}
*/

Value ndet_init()
{
    return list2(int_value(0), empty_list());
}

int ndet_next_choice(Value* ndet, int choices)
{
    int stepIndex = nth(*ndet, 0).i;
    Value steps = nth(*ndet, 1);

    // Add another step if needed
    if (stepIndex >= length(steps)) {

        Value newStep = list2(int_value(0), int_value(choices));
        *ndet = set_nth(*ndet, 1, append(take_nth(*ndet, 1), newStep));
    }

    int result = nth(nth(steps, stepIndex), 0).i;
    *ndet = apply_nth(*ndet, 0, increment);
    return result;
}

Value ndet_advance(Value ndet /*consumed*/)
{
    Value steps = take_nth(ndet, 1);

    int index = length(steps) - 1;
    bool loop = true;

    while (loop && index >= 0) {
        Value step = take_nth(steps, index);

        step = apply_nth(step, 0, increment);
        if (nth(step, 0).i >= nth(step, 1).i) {
            // Overflow
            step = set_nth(step, 0, int_value(0));
            loop = true;
        } else {
            loop = false;
        }
        steps = set_nth(steps, index, step);
        if (loop)
            index--;
    }

    if (index < 0) {
        decref2(ndet, steps);
        return nil_value();
    }

    ndet = set_nth(ndet, 0, int_value(0));
    ndet = set_nth(ndet, 1, steps);
    return ndet;
}

bool ndet_done(Value ndet)
{
    return is_nil(ndet);
}

void reset_test_params()
{
    if (g_ndet.raw == 0)
        g_ndet = nil_value();

    nullify(&g_ndet);
}

void test_params_advance()
{
    g_ndet = ndet_advance(g_ndet);
}

bool test_params_done()
{
    return ndet_done(g_ndet);
}

int next_param(int choices)
{
    if (is_nil(g_ndet))
        g_ndet = ndet_init();

    return ndet_next_choice(&g_ndet, choices);
}

Value next_value_any()
{
    switch (next_param(10)) {
    case 0:
        return nil_value();
    case 1:
        return int_value(1);
    case 2:
        return float_value(1);
    case 3:
        return bool_value(true);
    case 4:
        return bool_value(false);
    case 5:
        return empty_list();
    case 6:
        return list1(int_value(1));
    case 7:
        return table1(int_value(1), int_value(2));
    case 8:
        return symbol("sym");
    case 9:
        return from_str("123");
    }
    return nil_value();
}

void param_set_repr(Value repr)
{
    int index = nth(g_ndet, 0).i - 1;




}
