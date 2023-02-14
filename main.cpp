#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>

struct Maquina {
    int *tarefas;
    int pos;

    Maquina() : tarefas(), pos() {}

    explicit Maquina(int n) {
        this->pos = -1;
        this->tarefas = new int[n];
    }
};

void GravaArquivo(std::string heuristicaAnalisada, int quantidadeDeTarefasAlocadas, int quantidadesDeMaquinasAlocadas,
                  float valorDeReplicacao, float tempoDeExecucao, int totalDeIteracoes, int makespanInicial,
                  int makespanFinal, int numeroDaExecucao = 0) {
    std::string nome_arquivo =
            heuristicaAnalisada + "_" +
            std::to_string(quantidadesDeMaquinasAlocadas) + "_" +
            std::to_string(valorDeReplicacao) +
            ".csv";
    std::ofstream arquivo(nome_arquivo, std::ios::app);

    if (!arquivo.is_open()) {
        std::cout << "Não foi possível abrir o arquivo.";

        return;
    }

    arquivo << numeroDaExecucao << "," <<
    heuristicaAnalisada << "," <<
    quantidadeDeTarefasAlocadas << "," <<
    quantidadesDeMaquinasAlocadas << "," <<
    valorDeReplicacao << "," <<
    tempoDeExecucao << "," <<
    totalDeIteracoes << "," <<
    makespanInicial << "," <<
    makespanFinal << std::endl;


//    arquivo << "Execucao: " << numeroDaExecucao << std::endl;
//    arquivo << "Heuristica: " << heuristicaAnalisada << std::endl;
//    arquivo << "Quantidade de tarefas (Valor de N): " << quantidadeDeTarefasAlocadas << std::endl;
//    arquivo << "Quantidade de Maquinas (Valor de M): " << quantidadesDeMaquinasAlocadas << std::endl;
//    arquivo << "Valor de Replicacao: " << valorDeReplicacao << std::endl;
//    arquivo << "Tempo de execução (ms): " << tempoDeExecucao << std::endl;
//    arquivo << "Total de iteracoes: " << totalDeIteracoes << std::endl;
//    arquivo << "Makespan inicial: " << makespanInicial << std::endl;
//    arquivo << "Makespan final: " << makespanFinal << std::endl;
//    arquivo << std::endl << std::endl;

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

void melhor_melhora(
        Maquina *maquinas,
        int tam_m,
        int tam_n,
        float tam_r,
        int qtd
) {
    int ms, ms_s, ms_f, ms_n, value, pos_min, pos_max_value, moves = 0;

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

    while (true) {
        ms = ms_total(maquinas, tam_m);
        pos_min = pos_ms_min(maquinas, tam_m);

        if (pos_min == 0) {
            break;
        }

        ms_n = ms_maquina(maquinas[pos_min]);
        pos_max_value = search_max_value(maquinas[0]);

        if ((ms_n + maquinas[0].tarefas[pos_max_value] > ms) &&
            !find_next_max_value(maquinas[0], pos_max_value, ms, ms_n)) {
            break;
        }

        maquinas[pos_min].tarefas[++maquinas[pos_min].pos] = maquinas[0].tarefas[pos_max_value];

        for (int i = pos_max_value; i <= maquinas[0].pos; i++) {
            maquinas[0].tarefas[i] = maquinas[0].tarefas[i + 1];
            maquinas[0].tarefas[i + 1] = 0;
        }

        maquinas[0].pos--;
        moves++;
    }

    ms_f = ms_total(maquinas, tam_m);
    tempo_exec = clock::now() - tempo_s;

    GravaArquivo(
            "Busca Local - Melhor melhora",
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
                melhor_melhora(maquinas, j, pow(j, i), i, qtdParaExecutar);
            }
        }
    }

    return 0;
}