#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    IloEnv env;
    try {
        const int n = 4;           
        const int W = 10;          
        int valores[] = {40, 50, 100, 95}; 
        int pesos[] = {2, 3, 5, 7};         

        IloModel model(env);
        IloCplex cplex(model);
        
        IloBoolVarArray x(env, n); 
        
        IloExpr obj(env);
        for(int i = 0; i < n; i++) {
            obj += valores[i] * x[i];
        }
        model.add(IloMaximize(env, obj));
        obj.end();
        
        // Restrição de capacidade
        IloExpr pesoTotal(env);
        for(int i = 0; i < n; i++) {
            pesoTotal += pesos[i] * x[i];
        }
        model.add(pesoTotal <= W);
        pesoTotal.end();
        
        if(cplex.solve()) {
            cout << "Solucao otima encontrada\n";
            cout << "Valor total: " << cplex.getObjValue() << endl;
            cout << "Peso utilizado: ";
            
            double pesoUsado = 0;
            for(int i = 0; i < n; i++) {
                pesoUsado += pesos[i] * cplex.getValue(x[i]);
            }
            cout << pesoUsado << "/" << W << "\n\n";
            
            cout << "Itens na mochila:\n";
            for(int i = 0; i < n; i++) {
                if(cplex.getValue(x[i]) > 0.9) {
                    cout << "Item " << (i+1) 
                         << " (Valor: " << valores[i] 
                         << ", Peso: " << pesos[i] << ")\n";
                }
            }
        }
        else {
            cout << "Nenhuma solucao viavel" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    env.end();
    return 0;
}