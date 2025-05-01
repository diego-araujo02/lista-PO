#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

using namespace std;

struct Arco {
    int origem;
    int destino;
    int capacidade;
};

int main() {
    IloEnv env;
    try {
        // Configuração do grafo
        const int numVertices = 4;       
        const int s = 0;            
        const int t = 3;            
        
        // Lista de arcos com capacidades
        vector<Arco> arcos = {
            {0, 1, 16},  // s -> 1
            {0, 2, 13},  // s -> 2
            {1, 2, 10},  // 1 -> 2
            {1, 3, 12},  // 1 -> t
            {2, 1, 4},   // 2 -> 1
            {2, 3, 14}   // 2 -> t
        };

        IloModel model(env);
        IloCplex cplex(model);
        
        IloArray<IloIntVarArray> fluxo(env, numVertices);
        for(int i = 0; i < numVertices; i++) {
            fluxo[i] = IloIntVarArray(env, numVertices, 0, 0); // Inicializa com zeros
        }
        
        // variáveis apenas para arcos existentes
        for(auto& arco : arcos) {
            fluxo[arco.origem][arco.destino] = IloIntVar(env, 0, arco.capacidade);
        }

        // Função objetivo: Maximizar fluxo saindo da fonte
        IloExpr obj(env);
        for(auto& arco : arcos) {
            if(arco.origem == s) {
                obj += fluxo[arco.origem][arco.destino];
            }
        }
        model.add(IloMaximize(env, obj));
        obj.end();

        // Restrições de conservação de fluxo
        for(int v = 0; v < numVertices; v++) {
            if(v == s || v == t) continue;
            
            IloExpr entrada(env);
            IloExpr saida(env);
            
            for(auto& arco : arcos) {
                if(arco.destino == v) entrada += fluxo[arco.origem][arco.destino];
                if(arco.origem == v) saida += fluxo[arco.origem][arco.destino];
            }
            
            model.add(entrada == saida);
            entrada.end();
            saida.end();
        }

        // Restrições de capacidade já estão nas variáveis

        if(cplex.solve()) {
            cout << "Fluxo máximo: " << cplex.getObjValue() << endl << endl;
            
            cout << "Alocação de fluxo:" << endl;
            for(auto& arco : arcos) {
                int valor = cplex.getValue(fluxo[arco.origem][arco.destino]);
                if(valor > 0) {
                    cout << "No " << arco.origem << " -> No " << arco.destino
                         << ": " << valor << "/" << arco.capacidade << endl;
                }
            }
        } else {
            cout << "Nenhuma solução encontrada" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    env.end();
    return 0;
}