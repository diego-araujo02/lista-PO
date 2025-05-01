#include <ilcplex/ilocplex.h>
#include <iostream>

using namespace std;

int main() {
    IloEnv env;
    try {
        const int N = 3;  
        const int M = 5;  
        
        double f[] = {100.0, 200.0, 150.0};
        
        double c[N][M] = {
            {50.0, 60.0, 70.0, 80.0, 90.0},  
            {30.0, 40.0, 50.0, 60.0, 70.0},  
            {20.0, 25.0, 30.0, 35.0, 40.0}  
        };

        IloModel model(env);
        IloCplex cplex(model);

        // Variáveis de decisão
        IloBoolVarArray x(env, N);  // x[i]: instalação i está aberta
        IloArray<IloBoolVarArray> y(env, N); // y[i][j]: cliente j na instalação i
        for (int i = 0; i < N; i++) {
            y[i] = IloBoolVarArray(env, M);
        }

        IloExpr obj(env);
        for (int i = 0; i < N; i++) {
            obj += f[i] * x[i];  // Custo de instalação
            for (int j = 0; j < M; j++) {
                obj += c[i][j] * y[i][j];  // Custo de atendimento
            }
        }
        model.add(IloMinimize(env, obj));
        obj.end();

        // Restrições
        // Cada cliente é atendido por exatamente uma instalação
        for (int j = 0; j < M; j++) {
            IloExpr sum(env);
            for (int i = 0; i < N; i++) {
                sum += y[i][j];
            }
            model.add(sum == 1);
            sum.end();
        }

        // Clientes só podem ser alocados a instalações abertas
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                model.add(y[i][j] <= x[i]);
            }
        }

        if (cplex.solve()) {
            cout << "Custo total minimo: " << cplex.getObjValue() << endl << endl;

            cout << "Instalacoes abertas:" << endl;
            for (int i = 0; i < N; i++) {
                if (cplex.getValue(x[i]) > 0.9) {
                    cout << "Instalacao " << (i+1) << " (Custo: " << f[i] << ")" << endl;
                }
            }

            cout << "\nAtribuicao de clientes:" << endl;
            for (int j = 0; j < M; j++) {
                for (int i = 0; i < N; i++) {
                    if (cplex.getValue(y[i][j]) > 0.9) {
                        cout << "Cliente " << (j+1) << " -> Instalacao " << (i+1) 
                             << " (Custo: " << c[i][j] << ")" << endl;
                    }
                }
            }
        } else {
            cout << "Nenhuma solucao viavel encontrada" << endl;
        }
    }
    catch(IloException& e) {
        cerr << "Erro: " << e << endl;
    }
    env.end();
    return 0;
}