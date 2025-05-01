#include <ilcplex/ilocplex.h> 
#include <iostream>

using namespace std;

int main(){
    IloEnv env;
    try {
        int n = 6;
        int m = 2;

        int vitA[] = {1,0,2,2,1,2};
        int vitC[] = {0,1,3,1,3,2};
        int quantMin[] = {9,19};
        int preco[] = {35,30,60,50,27,22};

        IloModel modelo(env, "Problema das vitaminas");

        IloIntVarArray x(env, n, 0, IloIntMax);

        IloExpr custoTotal(env);
        for (int i = 0; i < n; i++) {
            custoTotal += preco[i] * x[i];
        }
        modelo.add(IloMinimize(env, custoTotal));
        custoTotal.end();

        for (int i = 0; i < m; i++) {
            IloExpr restricao(env);
            for (int j = 0; j < n; j++) {
                if (i == 0) {
                    restricao += vitA[j] * x[j]; // Vitamina A
                } else {
                    restricao += vitC[j] * x[j]; // Vitamina C
                }
            }
            modelo.add(restricao >= quantMin[i]);
            restricao.end();
        }

        IloCplex cplex(modelo);
        cplex.solve();

        cout << "Custo mínimo: " << cplex.getObjValue() << " reais" << endl;
        for (int j = 0; j < n; j++) {
            cout << "Ingrediente " << j+1 << ": " << cplex.getValue(x[j]) << " unidades" << endl;
        }

    } catch (IloException& e) {
        cerr << "Erro: " << e << endl;
    }
    env.end();
    return 0;
}
    

