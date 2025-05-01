#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        const int numF = 3;   // número de fazendas
        const int numC = 3;   // número de culturas
        int areaF[numF]   = {400, 650, 350};      // área disponível (acres)
        int waterF[numF]  = {1800, 2200, 950};    // água disponível (litros)
        int maxAreaC[numC]   = {660, 880, 400};   // área máxima total por cultura
        double waterC[numC]  = {5.5, 4.0, 3.5};   // água por acre
        double profitC[numC] = {5000, 4000, 1800}; // lucro por acre

        IloModel model(env);

        // Variáveis x[i][m]: acres de cultura m na fazenda i
        IloArray<IloIntVarArray> x(env, numF);
        for(int i = 0; i < numF; ++i) {
            x[i] = IloIntVarArray(env, numC);
            for(int m = 0; m < numC; ++m) {
                std::ostringstream name;
                name << "x_" << (i+1) << "_" << m;
                // limite superior por cultura: não ultrapassa maxAreaC[m]
                x[i][m] = IloIntVar(env, 0, maxAreaC[m], name.str().c_str());
            }
        }

        IloExpr obj(env);
        for(int i = 0; i < numF; ++i)
            for(int m = 0; m < numC; ++m)
                obj += profitC[m] * x[i][m];
        model.add(IloMaximize(env, obj));
        obj.end();

        // RESTRIÇÕES DE CAPACIDADE POR FAZENDA 
        for(int i = 0; i < numF; ++i) {
            IloExpr sumArea(env), sumWater(env);
            for(int m = 0; m < numC; ++m) {
                sumArea  += x[i][m];
                sumWater += waterC[m] * x[i][m];
            }
            model.add(sumArea  <= areaF[i]);
            model.add(sumWater <= waterF[i]);
            sumArea.end(); sumWater.end();
        }

        // LIMITES GLOBAIS DE ÁREA POR CULTURA
        for(int m = 0; m < numC; ++m) {
            IloExpr tot(env);
            for(int i = 0; i < numF; ++i) tot += x[i][m];
            model.add(tot <= maxAreaC[m]);
            tot.end();
        }

        // PROPORÇÃO IGUAL ENTRE FAZENDAS
        for(int m = 0; m < numC; ++m) {
            model.add(x[1][m] * areaF[0] == x[0][m] * areaF[1]);
            model.add(x[2][m] * areaF[0] == x[0][m] * areaF[2]);
        }

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::Threads, 1);
        if(cplex.solve()) {
            env.out() << "Lucro ótimo = " << cplex.getObjValue() << std::endl;
            for(int i = 0; i < numF; ++i) {
                env.out() << "Fazenda " << (i+1) << ": ";
                env.out() << cplex.getValue(x[i][0]) << " milho, ";
                env.out() << cplex.getValue(x[i][1]) << " arroz, ";
                env.out() << cplex.getValue(x[i][2]) << " feijão\n";
            }
        } else {
            env.out() << "Modelo não solucionado\n";
        }
    }
    catch(IloException &e) {
        std::cerr << "Erro CPLEX: " << e << std::endl;
    }
    catch(...) {
        std::cerr << "Erro desconhecido" << std::endl;
    }
    env.end();
    return 0;
}