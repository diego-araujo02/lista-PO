#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    IloEnv env;
    try {
        // 3 antenas formando um triângulo
        const int N = 3; 
        vector<vector<int>> adjacencia = {
            {1, 2},   
            {0, 2},   
            {0, 1}  
        };
        const int K = N;  

        IloModel model(env);
        IloCplex cplex(model);

        IloArray<IloBoolVarArray> x(env, N);
        for (int i = 0; i < N; i++) {
            x[i] = IloBoolVarArray(env, K);
        }
        IloBoolVarArray y(env, K); 

        IloExpr obj(env);
        for (int k = 0; k < K; k++) {
            obj += y[k];
        }
        model.add(IloMinimize(env, obj));
        obj.end();

        // Restrições
        // Cada antena usa exatamente uma frequência
        for (int i = 0; i < N; i++) {
            IloExpr sum(env);
            for (int k = 0; k < K; k++) {
                sum += x[i][k];
            }
            model.add(sum == 1);
            sum.end();
        }

        // Antenas adjacentes não compartilham frequência
        for (int k = 0; k < K; k++) {
            for (int i = 0; i < N; i++) {
                for (int j : adjacencia[i]) {
                    if (j > i) {  // Evitar duplicatas
                        model.add(x[i][k] + x[j][k] <= 1);
                    }
                }
            }
        }

        // Vinculação x e y
        for (int k = 0; k < K; k++) {
            for (int i = 0; i < N; i++) {
                model.add(x[i][k] <= y[k]);
            }
        }

        if (cplex.solve()) {
            cout << "Numero minimo de frequencias: " << cplex.getObjValue() << endl;
            cout << "Atribuicao:\n";
            for (int i = 0; i < N; i++) {
                for (int k = 0; k < K; k++) {
                    if (cplex.getValue(x[i][k])) {
                        cout << "Antena " << (i+1) << " -> Frequencia " << (k+1) << endl;
                    }
                }
            }
        } else {
            cout << "Nenhuma solucao viavel" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro: " << e << endl;
    }
    env.end();
    return 0;
}