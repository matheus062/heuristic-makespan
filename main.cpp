#include <iostream>
#include <math.h>

float const r_15 = 1.5;
float const r_20 = 2.0;

int const m_10 = 10;
int const m_20 = 20;
int const m_50 = 50;

struct Maquina {
    int tarefas[35] = {0};
    int pos = -1;
};

int ms_maquina(Maquina maquina) {
    if (maquina.pos == -1) {
        return 0;
    }

    int makespan = 0;

    for (int i = 0; i <= maquina.pos; i++) {
        makespan += maquina.tarefas[i];
    }

    return makespan;
}

int ms_total(Maquina *maquinas, int tam) {
    int makespan;
    int makespan_total = 0;

    for (int i = 0; i < tam; i++) {
        makespan = ms_maquina(maquinas[i]);
        if (makespan > makespan_total) makespan_total = makespan;
    }

    return makespan_total;
}

int pos_ms_min(Maquina *maquinas, int tam) {
    int makespan;
    int pos = 0;
    int makespan_min = ms_maquina(maquinas[pos]);

    for (int i = 1; i < tam; i++) {
        makespan = ms_maquina(maquinas[i]);
        if (makespan < makespan_min) {
            pos = i;
            makespan_min = makespan;
        }
    }

    return pos;
}

int search_max_value(Maquina maquina, int filtrar_menor = 0) {
    bool filtrar = false;
    int pos = -1;
    int valor = 0;

    if (filtrar_menor > 0) {
        filtrar = true;
    }

    for (int i = 0; i <= maquina.pos; i++) {
        if ((maquina.tarefas[i] > valor)) {
            if (!filtrar) {
                valor = maquina.tarefas[i];
                pos = i;

                continue;
            } else if (maquina.tarefas[i] < filtrar_menor) {
                valor = maquina.tarefas[i];
                pos = i;
            }
        }

    }

    return pos;
}

bool find_next_max_value(Maquina maquina, int &pos, int ms, int ms_n) {
    while (true) {
        pos = search_max_value(maquina, maquina.tarefas[pos]);

        if (pos == -1) {
            return false;
        } else if (ms_n + maquina.tarefas[pos] < ms) {
            break;
        }
    }

    return true;
}

int main() {

    int tam_m = m_10;
    float tam_r = r_15;

    int n = pow(tam_m, tam_r);
    Maquina maquinas[tam_m];

    int ms;

    int pos_min;
    int pos_max_value;

    bool run = true;

    int value;
    srand(time(NULL));

    int moves = 0;

    for (int i = 0; i < n; i++) {
        value = (rand() % 100);
        maquinas[0].tarefas[i] = (value > 0) ? value : 1;
        maquinas[0].pos++;
    }

    int ms_n;

    do {
        ms = ms_total(maquinas, tam_m);
        pos_min = pos_ms_min(maquinas, tam_m);

        if (pos_min == 0) {
            break;
        }

        ms_n = ms_maquina(maquinas[pos_min]);
        pos_max_value = search_max_value(maquinas[0]);

        if ((ms_n + maquinas[0].tarefas[pos_max_value] > ms) && !find_next_max_value(maquinas[0], pos_max_value, ms, ms_n)) {
            break;
        }

        maquinas[pos_min].tarefas[++maquinas[pos_min].pos] = maquinas[0].tarefas[pos_max_value];

        for (int i = pos_max_value; i <= maquinas[0].pos; i++) {
            maquinas[0].tarefas[i] = maquinas[0].tarefas[i + 1];
            maquinas[0].tarefas[i + 1] = 0;
        }

        maquinas[0].pos--;
        moves++;

    } while (run);


    return 0;
}

/*    M1   M2  M3
 *  | 29 |   |   |
 *  | 17 |   |   |
 *  | 14 |   |   |
 *  | 10 |   |   |
 */