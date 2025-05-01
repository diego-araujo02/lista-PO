#include <ilcplex/ilocplex.h>  
#include <ilconcert/ilomodel.h>
#include <iostream>

using namespace std;

int main() {
    IloEnv env;

    try {
        const int n = 2;  // AMGS e RE
        const int m = 2;  // Carne e cereais

        string produtos[] = {"AMGS", "RE"};
        string recursos[] = {"Carne", "Cereais"};

        double lucro[] = {11.0, 12.0};
        double a[][n] = {{1.0, 4.0}, {5.0, 2.0}};
        double b[] = {10000.0, 30000.0};

        IloModel modelo(env, "Problema da Racao");

        
        IloIntVarArray x(env, n, 0, IloIntMax); 

        IloExpr lucroTotal(env);
        for (int j = 0; j < n; j++) {
            lucroTotal += lucro[j] * x[j];
        }
        modelo.add(IloMaximize(env, lucroTotal));
        lucroTotal.end();

        for (int i = 0; i < m; i++) {
            IloExpr restricao(env);
            for (int j = 0; j < n; j++) {
                restricao += a[i][j] * x[j];
            }
            modelo.add(restricao <= b[i]);
            restricao.end();
        }

        IloCplex cplex(env);    
        cplex.extract(modelo);  
        cplex.solve();

        cout << "Lucro maximo (inteiro): " << cplex.getObjValue() << " reais" << endl;
        for (int j = 0; j < n; j++) {
            cout << produtos[j] << ": " << cplex.getValue(x[j]) << " unidades" << endl;
        }

    } catch (IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }

    env.end();
    return 0;
}