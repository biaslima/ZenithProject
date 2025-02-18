#include "inc/anxiety_level.h"
#include "inc/config.h"

void determine_anxiety_level(void) {
    if (btn_a_ativo && btn_b_ativo) {
        current_level = 3; // Nível Grave
    } else if (btn_a_ativo && !btn_b_ativo) {
        current_level = 1; // Nível Leve
    } else if (btn_b_ativo && !btn_a_ativo) {
        current_level = 2; // Nível Médio
    } else {
        current_level = 0; // Nenhum nível selecionado
    }
    btn_a_ativo = false;
    btn_b_ativo = false;
}
