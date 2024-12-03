#ifndef Instance_H
#define Instance_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class Instance {
public:
    int qt_vertices;
    int veiculos;
    double t_max;
    int t_prot;
    int t_parada;
    int velocidade;
    int iteracoes = 1;
    vector<double> score_vertices;
    vector<vector<double> > distancia_matriz;

    Instance(const string &filename);
    friend ostream &operator<<(ostream &os, const Instance &instance);
};

#endif // Instance_H
