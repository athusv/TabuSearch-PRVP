#ifndef SOL_H
#define SOL_H

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <queue>
#include <cassert>
#include "Caminho.h"  // Inclui o cabeçalho da classe Caminho
#include "Instance.h"  // Inclui o cabeçalho da classe Instance
#include "Busca_local.h"

using namespace std;

class Sol {
private:
public:
    double score = 0;
    double custo = 0;
    vector<tuple<Busca_local::Operacao, int, int, int>> lista_tabu;
    // <movimentação, id da rota, vértice, iteração para ser excluído>
    priority_queue<Caminho> rotas;
    vector<map<double, int>> visited_vertices;  // numero_vertice - quando/quem visitou
    map<string, int> cont_vizinhanca = {{"best_incert", 0},{"swap_inter", 0}, {"swap_intra", 0}, {"swap_out", 0}, {"para", 0}};


    Sol(const Instance &grafo);
    void print_visited(int inicio, int final) const;
    bool checa_rota(Instance &grafo, string &chamou);
    bool checa_visited(const Instance &grafo, string &chamou) const;
    bool checa_score(Instance &grafo, string &chamou);
    bool checa_custo(Instance &grafo, string &chamou);
    bool checa_solucao(Instance &grafo, string &chamou);
    bool checa_push(Instance &grafo, string &chamou);
    bool checa_visita_custo(Instance &grafo, string &chamou);
    void atualiza_push(Instance &grafo);
    void print_solucao(Instance &grafo);
    bool operator<(const Sol &s) const;
    friend ostream &operator<<(ostream &os, const Sol &sol);
};

#endif // SOL_H
