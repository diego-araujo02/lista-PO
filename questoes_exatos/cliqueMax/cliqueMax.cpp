#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    IloEnv env;
    try {
        const int N = 3; 
        vector<vector<int>> adjacencia = {
            {1, 1, 1}, // Vértice 0 conectado a 1 e 2
            {1, 1, 1}, // Vértice 1 conectado a 0 e 2
            {1, 1, 1}  // Vértice 2 conectado a 0 e 1
        };

        IloModel model(env);
        IloCplex cplex(model);

        IloBoolVarArray x(env, N); 

        IloExpr obj(env);
        for (int i = 0; i < N; i++) {
            obj += x[i];
        }
        model.add(IloMaximize(env, obj));
        obj.end();

        // Restrições: Pares não adjacentes não podem estar na clique
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (!adjacencia[i][j]) { 
                    model.add(x[i] + x[j] <= 1);
                }
            }
        }

        if (cplex.solve()) {
            cout << "Tamanho da Clique Maxima: " << cplex.getObjValue() << endl;
            cout << "Vertices na clique:\n";
            for (int i = 0; i < N; i++) {
                if (cplex.getValue(x[i]) > 0.9) {
                    cout << "Vertice " << i << endl;
                }
            }
        } else {
            cout << "Nenhuma solucao encontrada" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro: " << e << endl;
    }
    env.end();
    return 0;
}