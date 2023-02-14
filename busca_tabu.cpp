#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <algorithm>

int **lista_tabu;

struct Maquina {
public:
    int *tarefas;
    int pos;
    int n;

    Maquina() : tarefas(), pos() {}

    explicit Maquina(int n) {
        this->pos = -1;
        this->tarefas = new int[n];
        this->n = n;
    }

    Maquina(const Maquina &toCopy) {
        this->tarefas = new int[toCopy.n];
        this->pos = toCopy.pos;
        this->n = toCopy.n;
        std::copy(toCopy.tarefas, toCopy.tarefas + toCopy.n, this->tarefas);
    }
};

void GravaArquivo(std::string heuristicaAnalisada, int quantidadeDeTarefasAlocadas, int quantidadesDeMaquinasAlocadas,
                  float valorDeReplicacao, float tempoDeExecucao, int totalDeIteracoes, int makespanInicial,
                  int makespanFinal, int numeroDaExecucao = 0) {
    std::string nome_arquivo =
            heuristicaAnalisada + "_" +
            std::to_string(quantidadesDeMaquinasAlocadas) + "_" +
            std::to_string(valorDeReplicacao) +
            ".txt";
    std::ofstream arquivo(nome_arquivo, std::ios::app);

    if (!arquivo.is_open()) {
        std::cout << "Não foi possível abrir o arquivo.";

        return;
    }

    arquivo << "Execucao: " << numeroDaExecucao << std::endl;
    arquivo << "Heuristica: " << heuristicaAnalisada << std::endl;
    arquivo << "Quantidade de tarefas (Valor de N): " << quantidadeDeTarefasAlocadas << std::endl;
    arquivo << "Quantidade de Maquinas (Valor de M): " << quantidadesDeMaquinasAlocadas << std::endl;
    arquivo << "Valor de Replicacao: " << valorDeReplicacao << std::endl;
    arquivo << "Tempo de execução (ms): " << tempoDeExecucao << std::endl;
    arquivo << "Total de iteracoes: " << totalDeIteracoes << std::endl;
    arquivo << "Makespan inicial: " << makespanInicial << std::endl;
    arquivo << "Makespan final: " << makespanFinal << std::endl;
    arquivo << std::endl << std::endl;

    arquivo.close();

    std::cout << "Texto gravado com sucesso no arquivo." << std::endl;
}

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

bool localiza_tabu(Maquina *maquinas, int tam_m, int interacoes) {
    bool found = false;

    for (int i = 0; i < interacoes; i++) {
        for (int j = 0; j < tam_m; j++) {
            if (ms_maquina(maquinas[j]) != lista_tabu[i][j]) {
                break;
            }

            found = true;
        }
    }

    return found;
}

bool next_move(
        int &moveFrom,
        int &moveTo,
        Maquina *maquinas,
        int tam_m,
        bool busca_tabu,
        int interacoes,
        int &moves
) {
    bool found = false;
    int ms_min, ms, min_k, min_l;
    Maquina clone_maquinas[tam_m];

    for (int z = 0; z < tam_m; z++) {
        clone_maquinas[z] = Maquina(maquinas[z]);
    }

    if (busca_tabu) {
        ms_min = -1;
    } else {
        ms_min = ms_total(clone_maquinas, tam_m);
    }

    for (int k = 0; k < tam_m; k++) {
        for (int l = 0; l < tam_m; l++) {
            if ((k == l) || (clone_maquinas[k].pos == 1)) {
                continue;
            }

            clone_maquinas[l].tarefas[++clone_maquinas[l].pos] = clone_maquinas[k].tarefas[clone_maquinas[k].pos--];
            moves++;
            ms = ms_total(clone_maquinas, tam_m);

            if ((ms < ms_min) || (ms_min == -1)) {
                if (busca_tabu) {
                    if (!localiza_tabu(clone_maquinas, tam_m, interacoes)) {
                        found = true;
                        ms_min = ms;
                        min_k = k;
                        min_l = l;
                    }
                } else {
                    found = true;

                    ms_min = ms;
                    min_k = k;
                    min_l = l;
                }
            }

            clone_maquinas[k].tarefas[++clone_maquinas[k].pos] = clone_maquinas[l].tarefas[clone_maquinas[l].pos--];
            moves++;
        }
    }

    if (found) {
        moveFrom = min_k;
        moveTo = min_l;
    }

    // delete []clone_maquinas;

    return found;
}

void busca_tabu(
        Maquina *maquinas,
        int tam_m,
        int tam_n,
        float tam_r,
        int qtd
) {
    int ms, ms_s, ms_f, ms_t, value, pos_min, pos_max, pos_max_value, moves = 0;

    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;
    static clock::time_point tempo_s;
    duration tempo_exec;

    for (int i = 0; i < tam_m; i++) {
        maquinas[i] = Maquina(tam_n);
    }

    for (int i = 0; i < tam_n; i++) {
        value = (rand() % 100);
        maquinas[0].tarefas[i] = (value > 0) ? value : 1;
        maquinas[0].pos++;
    }

    ms_s = ms_total(maquinas, tam_m);
    tempo_s = clock::now();

    int interacoes = 0;
    // TODO : Depois programar o delta certinho
    int max_interacoes = tam_n * 0.09;

    Maquina clone_maquinas[tam_m];

    if (max_interacoes < 10) {
        max_interacoes = 10;
    }

    lista_tabu = new int *[max_interacoes];

    for (int i = 0; i < max_interacoes; i++) {
        lista_tabu[i] = new int[tam_n];

        for (int j = 0; j < tam_m; j++) {
            lista_tabu[i][j] = -2;
        }
    }

    int moveFrom;
    int moveTo;
    bool run = true;

    while (run) {
        ms = ms_total(maquinas, tam_m);

        if (next_move(moveFrom, moveTo, maquinas, tam_m, false, interacoes, moves)) {
            maquinas[moveTo].tarefas[++maquinas[moveTo].pos] = maquinas[moveFrom].tarefas[maquinas[moveFrom].pos--];

            continue;
        }

        for (int z = 0; z < tam_m; z++) {
            clone_maquinas[z] = Maquina(maquinas[z]);
        }

        while (interacoes < max_interacoes) {
            for (int m = 0; m < tam_m; m++) {
                lista_tabu[interacoes][m] = ms_maquina(clone_maquinas[m]);
            }
            interacoes++;

            if (!next_move(moveFrom, moveTo, clone_maquinas, tam_m, true, interacoes, moves)) {
                run = false;

                break;
            }

            clone_maquinas[moveTo].tarefas[++clone_maquinas[moveTo].pos] = clone_maquinas[moveFrom].tarefas[clone_maquinas[moveFrom].pos--];
            moves++;
            ms_t = ms_total(clone_maquinas, tam_m);

            if (ms_t < ms) {
                interacoes = 0;

                for (int z = 0; z < tam_m; z++) {
                    maquinas[z] = Maquina(clone_maquinas[z]);
                }

                for (int i = 0; i < max_interacoes; i++) {
                    lista_tabu[i] = new int[tam_n];

                    for (int j = 0; j < tam_m; j++) {
                        lista_tabu[i][j] = -2;
                    }
                }

                break;
            }

            if(interacoes == max_interacoes) {
                run = false;
            }
        }


    }

    ms_f = ms_total(maquinas, tam_m);
    tempo_exec = clock::now() - tempo_s;

    GravaArquivo(
            "Busca Tabu",
            tam_n,
            tam_m,
            tam_r,
            tempo_exec.count(),
            moves,
            ms_s,
            ms_f,
            qtd
    );
}

int main() {

    float const r_x[] = {1.5, 2.0};
    int const m_x[] = {10, 20, 50};

    srand(time(nullptr));

    for (int qtdParaExecutar = 1; qtdParaExecutar <= 10; qtdParaExecutar++) {
        for (float i: r_x) {
            for (int j: m_x) {
                auto *maquinas = new Maquina[j];
                busca_tabu(maquinas, j, pow(j, i), i, qtdParaExecutar);
            }
        }
    }

    return 0;
}