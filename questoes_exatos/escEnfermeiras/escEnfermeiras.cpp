#include <ilcplex/ilocplex.h>
#include <iostream>

using namespace std;

int main() {
    IloEnv env;
    try {
        // Demanda diária (dia 1 a 7)
        int demanda[] = {3, 5, 4, 2, 6, 7, 2};
        
        IloModel model(env);
        IloCplex cplex(model);
        
        // Variáveis de decisão (enfermeiras que começam no dia i+1)
        IloIntVarArray X(env, 7, 0, IloIntMax);  // X[0]=dia1, X[1]=dia2,...X[6]=dia7
        
        // Função objetivo: Minimizar total de enfermeiras
        model.add(IloMinimize(env, IloSum(X)));
        
        // Restrições de cobertura
        for(int j = 0; j < 7; j++) {  // Para cada dia j+1
            IloExpr soma(env);
            for(int k = 0; k < 5; k++) {  // 5 dias consecutivos
                int dia_inicio = j - 4 + k;
                if(dia_inicio < 0) dia_inicio += 7;
                soma += X[dia_inicio];
            }
            model.add(soma >= demanda[j]);
            soma.end();
        }
        
        if(cplex.solve()) {
            cout << "Solucao otima encontrada\n";
            cout << "Numero minimo de enfermeiras: " << cplex.getObjValue() << "\n\n";
            
            cout << "Alocacao por dia de inicio:\n";
            for(int i = 0; i < 7; i++) {
                cout << "Dia " << (i+1) << ": " << cplex.getValue(X[i]) << " enfermeiras\n";
            }
            
            // Verificação da cobertura
            cout << "\nCobertura efetiva:\n";
            for(int j = 0; j < 7; j++) {
                int cobertura = 0;
                for(int k = 0; k < 5; k++) {
                    int dia_inicio = j - 4 + k;
                    if(dia_inicio < 0) dia_inicio += 7;
                    cobertura += cplex.getValue(X[dia_inicio]);
                }
                cout << "Dia " << (j+1) << ": " << cobertura 
                     << "/" << demanda[j] << endl;
            }
        }
        else {
            cout << "Nao foi encontrada uma solucao viavel" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    env.end();
    return 0;
}