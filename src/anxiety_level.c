#include "anxiety_level.h"
#include "config.h"

void determine_anxiety_level(void) {
    if (btn_a_ativo && btn_b_ativo) {
        current_level = 3; 
    } else if (btn_a_ativo && !btn_b_ativo) {
        current_level = 1; 
    } else if (btn_b_ativo && !btn_a_ativo) {
        current_level = 2; 
    } else {
        current_level = 0; 
    }
    btn_a_ativo = false;
    btn_b_ativo = false;
}
