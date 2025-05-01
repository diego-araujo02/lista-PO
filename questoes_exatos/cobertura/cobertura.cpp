#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    IloEnv env;
    try {
        // bairros e adjacências
        const int numBairros = 6;
        vector<vector<int>> adjacencias = {
            {1, 2},      // Bairro 0 
            {0, 2, 3},   // Bairro 1
            {0, 1, 4},   // Bairro 2
            {1, 4, 5},   // Bairro 3
            {2, 3, 5},   // Bairro 4
            {3, 4}       // Bairro 5
        };

        IloModel model(env);
        IloCplex cplex(model);
        
        IloBoolVarArray X(env, numBairros);
        
        // Função objetivo: Minimizar escolas construídas
        model.add(IloMinimize(env, IloSum(X)));
        
        // Restrições de cobertura
        for(int v = 0; v < numBairros; v++) {
            IloExpr cobertura(env);
            cobertura += X[v];  // Escola no próprio bairro
            
            // Escolas nos bairros adjacentes
            for(int u : adjacencias[v]) {
                cobertura += X[u];
            }
            
            model.add(cobertura >= 1);
            cobertura.end();
        }
        
        if(cplex.solve()) {
            cout << "Solucao otima encontrada\n";
            cout << "Numero minimo de escolas: " << cplex.getObjValue() << "\n\n";
            
            cout << "Bairros selecionados:\n";
            for(int v = 0; v < numBairros; v++) {
                if(cplex.getValue(X[v]) > 0.9) {
                    cout << "Bairro " << (v+1) << endl;
                }
            }
            
            // Verificação da cobertura
            cout << "\nVerificacao de cobertura:\n";
            for(int v = 0; v < numBairros; v++) {
                int total = 0;
                total += cplex.getValue(X[v]);
                for(int u : adjacencias[v]) {
                    total += cplex.getValue(X[u]);
                }
                cout << "Bairro " << (v+1) << ": " 
                     << (total >= 1 ? "Coberto" : "NAO Coberto") << endl;
            }
        }
        else {
            cout << "Nenhuma solucao viavel encontrada" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    env.end();
    return 0;
}