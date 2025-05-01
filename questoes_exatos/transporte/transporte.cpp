#include <ilcplex/ilocplex.h>
#include <iostream>

using namespace std;

int main() {
    IloEnv env;
    try {
        const int numFab = 3, numDep = 3;
        
        int custos[numFab][numDep] = {
            {8, 5, 6},
            {15, 10, 12},
            {3, 9, 10}
        };
        
        int oferta[numFab] = {120, 80, 80};
        int demanda[numDep] = {150, 70, 60};
        
        IloModel model(env);
        IloCplex cplex(model);
        
        // Variáveis de decisão: x[i][j] = quantidade transportada de i para j
        IloArray<IloIntVarArray> x(env, numFab);
        for(int i = 0; i < numFab; i++) {
            x[i] = IloIntVarArray(env, numDep, 0, IloIntMax);
        }
        
        // Função objetivo: Minimizar custo total
        IloExpr objExpr(env);
        for(int i = 0; i < numFab; i++) {
            for(int j = 0; j < numDep; j++) {
                objExpr += custos[i][j] * x[i][j];
            }
        }
        model.add(IloMinimize(env, objExpr));
        objExpr.end();
        
        // Restrições de oferta
        for(int i = 0; i < numFab; i++) {
            IloExpr sum(env);
            for(int j = 0; j < numDep; j++) {
                sum += x[i][j];
            }
            model.add(sum == oferta[i]);
            sum.end();
        }
        
        // Restrições de demanda
        for(int j = 0; j < numDep; j++) {
            IloExpr sum(env);
            for(int i = 0; i < numFab; i++) {
                sum += x[i][j];
            }
            model.add(sum == demanda[j]);
            sum.end();
        }
        
        if(cplex.solve()) {
            cout << "Status: " << cplex.getStatus() << endl;
            cout << "Custo total minimo: " << cplex.getObjValue() << endl << endl;
            
            for(int i = 0; i < numFab; i++) {
                for(int j = 0; j < numDep; j++) {
                    int qtd = cplex.getValue(x[i][j]);
                    if(qtd > 0) {
                        cout << "Fabrica " << (i+1) << " -> Deposito " << (j+1)
                             << ": " << qtd << " unidades" << endl;
                    }
                }
            }
        }
        else {
            cout << "Nenhuma solução encontrada" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    catch(...) {
        cerr << "Erro desconhecido" << endl;
    }
    env.end();
    return 0;
}