#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <queue>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cassert>
#include <map>
#include <iterator>
#include "Instance.h"
#include "Caminho.h"
#include "Sol.h"
#include "Utils.h"
#include "Perturbacao.h"
#include "Busca_local.h"

#define NDEBUG

using namespace std;

void Construtivo(Instance &grafo, Sol &s0, mt19937 &gen)
{
    // Variavel de parada do while, todas as rotas estao prontas
    priority_queue<Caminho> rotas_prontas;
    while (!s0.rotas.empty())
    {
        Caminho rota = s0.rotas.top();
        s0.rotas.pop();

        // decidindo se vai parar ou passar
        std::uniform_int_distribution<int> dis(1, 10);
        int rand_num = dis(gen);
        rota.plus_parada = (rand_num <= 7) ? grafo.t_parada : 0;

        // Lista de vertices possiveis
        vector<int> lista_candidatos = Utils::make_lista(grafo, rota, s0.visited_vertices);
        if (lista_candidatos.empty())
        {
            rota.custo += grafo.distancia_matriz[rota.route.back()][0];
            rota.route.push_back(0);
            rota.paradas.push_back(0);
            rota.visita_custo.push_back(rota.custo);
            rota.push_hotspots.push_back({999, 999});

            rota.atualizar_push_hotspots(s0.visited_vertices);
            rotas_prontas.push(rota);
            s0.custo += rota.custo;
            s0.score += rota.score;
            // rota.print_push();
            continue;
        }
        // - (20%) Aleatório,
        // - (20%) menor custo,
        // - (20%) maior pontuação,
        // - (40%) melhor custo-benefício (pontuação/tempo).
        // cout << "aleatorio: " << rand_num << endl;
        int index; string chamou;
        rand_num = dis(gen);
        // cout << "rand_num: "<<rand_num<<endl;
        if(rand_num >=1 && rand_num < 3){
            shuffle(lista_candidatos.begin(), lista_candidatos.end(), gen);
            index = lista_candidatos[0];
            // cout << "Random: " << index << endl;
            chamou = "Construtivo - Random";
        }else if(rand_num >= 3 && rand_num < 5){
            // cout << rota <<endl<<rota.route.back()<<endl;
            index = Utils::min_custo(lista_candidatos, grafo.distancia_matriz, rota.route.back());
            // cout << "Min_custo: " << index << endl;
            chamou = "Construtivo - Min_custo";
        }else if(rand_num >= 5 && rand_num < 7){
            index = Utils::max_score(lista_candidatos, grafo.score_vertices);
            // cout << "Max_score: " << index << endl;
            chamou = "Construtivo - Max_scores";
        }else if(rand_num >= 7 && rand_num <=10){
            index = Utils::cost_benefit(lista_candidatos, grafo.score_vertices, grafo.distancia_matriz, rota.route.back());
            // cout << "Cost_benefit: " << index << endl;
            chamou = "Construtivo - Cost_benefit";
        }


        rota.custo += grafo.distancia_matriz[rota.route.back()][index] + rota.plus_parada;
        double s = grafo.score_vertices[index];

        if (rota.plus_parada == grafo.t_parada)
        {
            s = grafo.score_vertices[index];
            rota.score += s;
            rota.paradas.push_back(1);
        }
        else
        {
            s = grafo.score_vertices[index] / 3;
            rota.score += s;
            rota.paradas.push_back(0);
        }
        rota.visita_custo.push_back(rota.custo);
        rota.route.push_back(index);
        rota.push_hotspots.push_back({999, 999});

        s0.visited_vertices[index][rota.custo + grafo.t_prot] = rota.id;

        s0.rotas.push(rota);
        s0.atualiza_push(grafo);
        s0.checa_solucao(grafo, chamou);
    }
    s0.rotas = rotas_prontas;
}

Sol ILS(Sol &s0, Instance &grafo, mt19937 gen, double tempo_maximo)
{
    // cout << "ILS" << endl;
    Sol s = s0; Sol s1 = s0;
    Sol best_s = s0;
    string chamou;

    auto inicio = std::chrono::high_resolution_clock::now();
    
    while(true) {
        s1 = s;
        s1 = Perturbacao::perturbacao(grafo, s1, gen);
        s1.atualiza_push(grafo);
        chamou = "Pertubação";
        s1.checa_solucao(grafo, chamou);
        cout << "|||||||||||Pertubação " << grafo.iteracoes << endl;
        cout << "Score: " << s1.score << ", Custo: " << s1.custo << endl;

        s1 = Busca_local::busca_local(grafo, s1, gen);
        s1.atualiza_push(grafo);

        chamou = "Busca Local";
        s1.checa_solucao(grafo, chamou);
        cout << "||||||||||||Busca Local " << grafo.iteracoes << endl;
        cout << "Score: " << s1.score << ", Custo: " << s1.custo << endl;
        if (!Utils::doubleGreaterOrEqual(best_s.score, s1.score))
        {

            cout << "Melhor solucão encontrada ***** " << grafo.iteracoes << endl;
            cout << "Score: " << s1.score << ", Custo: " << s1.custo << endl;
            best_s = s1;
            s = best_s;
        }
        // Verifica se o tempo máximo foi atingido
        auto agora = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duracao = agora - inicio;

        if (duracao.count() >= tempo_maximo)
        {
            // std::cout << "Tempo máximo de execução atingido: " << duracao.count() << " segundos.\n";
            std::cout << "Iteração: " << grafo.iteracoes << "\n";
            break;
        }

        grafo.iteracoes++;
    }

    return best_s;
}

int main(int argc, char *argv[])
{
    unsigned int seed_value;
    std::random_device rd;

    if (argc > 2)
    {
        seed_value = stoul(argv[2]);
        // cout << "Seed fornecida: " << seed_value << std::endl;
    }
    else
    {
        random_device rd;
        seed_value = rd();
        // cout << "Seed aleatória gerada: " << seed_value << std::endl;
    }
    mt19937 gen(seed_value);

    // Lendo grafo
    string instancia = "Instancias/"+string(argv[1]);

    // Instance grafo("C:/Users/athus/Faculdade/6 periodo/PIBIT/solucao/pibit-rotas-pm/misc/ILS-algoritm/" + instancia, t_prot, t_parada, velocidade);
    Instance grafo(instancia);

    // Solução inicial
    Sol s0(grafo);

    Construtivo(grafo, s0, gen);
    s0.atualiza_push(grafo);


    string chamou = "Construtivo";
    s0.checa_solucao(grafo, chamou);

    double tempo_maximo = 60.0 * 3; //Em segundos
    Sol s1 = ILS(s0, grafo, gen, tempo_maximo);

    double percentual_melhora = (static_cast<double>(s1.score - s0.score) / s0.score) * 100;
    std::cout << "Tempo Máximo: " << grafo.t_max * grafo.veiculos << std::endl;
    cout << "Seed: "<<seed_value<< " | Tempo de Execução: "<<tempo_maximo<<"s"<<endl;
    cout << "T_prot: "<< grafo.t_prot/60 << "min | T_parada: " <<grafo.t_parada/60<< "min | Velocidade: "<<grafo.velocidade<<"Km/h"<< endl;
    cout << "Instância: " << instancia << " | Vértices: " <<grafo.qt_vertices << " | Veículos: " <<grafo.veiculos<<endl;
    cout << "Solução Construtivo - Score: " << s0.score << " | Custo: " << s0.custo << endl;
    cout << "Solução ILS - Score: " << s1.score << " | Custo: "<<s1.custo<<endl;
    cout << "Melhoria de " << percentual_melhora << "%"<<endl<<endl;
    cout << "Contagem Estruturas de Vizinhança: " <<endl;
    cout << "Best Incert = " << s1.cont_vizinhanca["best_incert"]<<endl;
    cout << "Swap Inter = " << s1.cont_vizinhanca["swap_inter"]<<endl;
    cout << "Swap Intra = " << s1.cont_vizinhanca["swap_intra"] << endl;
    cout << "Swap Out = " << s1.cont_vizinhanca["swap_out"] << endl;
    cout << "Para = " << s1.cont_vizinhanca["para"] << endl;
    // s1.print_solucao(grafo);
    // exportar um .TXT
}