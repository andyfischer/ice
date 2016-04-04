
#pragma once

Value ndet_init();
int ndet_next_choice(Value* ndet, int choices);
Value ndet_advance(Value ndet /*consumed*/);
bool ndet_done(Value ndet);

void reset_test_params();
bool test_params_advance();
bool test_params_done();
int next_param(int choices);
Value next_value_any();
