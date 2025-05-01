#include <ilcplex/ilocplex.h>
#include <iostream>

using namespace std;

int main() {
    IloEnv env;
    try {
        const int numPadroes = 4;
        int folhasTam1 = 200, folhasTam2 = 90;
        
        int corpos[] = {1, 2, 1, 1};
        int tampas[] = {7, 3, 9, 4};
        
        IloModel model(env);
        IloCplex cplex(model);
        IloIntVarArray x(env, numPadroes, 0, IloIntMax);
        IloIntVar y(env, 0, IloIntMax);
        
        IloExpr objExpr(env);
        objExpr = 103*y 
                - 50*(corpos[0]*x[0] + corpos[1]*x[1] + corpos[2]*x[2] + corpos[3]*x[3])
                - 3*(tampas[0]*x[0] + tampas[1]*x[1] + tampas[2]*x[2] + tampas[3]*x[3]);
        model.add(IloMaximize(env, objExpr));
        objExpr.end();
        
        // Restrições de folhas
        model.add(x[0] + x[2] + x[3] <= folhasTam1); // Tam1: padrões 1,3,4
        model.add(x[1] <= folhasTam2);               // Tam2: padrão 2
        
        // Restrições de balanço
        model.add(y <= corpos[0]*x[0] + corpos[1]*x[1] + corpos[2]*x[2] + corpos[3]*x[3]);
        model.add(y <= tampas[0]*x[0] + tampas[1]*x[1] + tampas[2]*x[2] + tampas[3]*x[3]);
        
        if (cplex.solve()) {
            cout << "Lucro Máximo: " << cplex.getObjValue() << endl;
            cout << "Impressões por padrão:\n";
            cout << "Padrão 1: " << cplex.getValue(x[0]) << endl;
            cout << "Padrão 2: " << cplex.getValue(x[1]) << endl;
            cout << "Padrão 3: " << cplex.getValue(x[2]) << endl;
            cout << "Padrão 4: " << cplex.getValue(x[3]) << endl;
            cout << "Latinhas vendidas: " << cplex.getValue(y) << endl;
        } else {
            cout << "Solução não encontrada" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro: " << e << endl;
    }
    env.end();
    return 0;
}