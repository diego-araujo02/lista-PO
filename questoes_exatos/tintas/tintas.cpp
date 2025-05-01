#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        const int demandSR = 1000;
        const int demandSN = 250;
        const double priceA = 1.5, priceB = 1.0, priceSEC = 4.0, priceCOR = 6.0;
        const double secA = 0.30, corA = 0.70;
        const double secB = 0.60, corB = 0.40;

        IloModel model(env);

        
        IloIntVar aA(env, 0, demandSR, "aA");
        IloIntVar aB(env, 0, demandSR, "aB");
        IloIntVar aSEC(env,0, demandSR,"aSEC");
        IloIntVar aCOR(env,0, demandSR,"aCOR");

        IloIntVar bA(env, 0, demandSN, "bA");
        IloIntVar bB(env, 0, demandSN, "bB");
        IloIntVar bSEC(env,0, demandSN,"bSEC");
        IloIntVar bCOR(env,0, demandSN,"bCOR");


        IloExpr cost(env);
        cost += priceA*(aA + bA) + priceB*(aB + bB)
              + priceSEC*(aSEC + bSEC) + priceCOR*(aCOR + bCOR);
        model.add(IloMinimize(env, cost));
        cost.end();

        // --- DEMANDAS ---
        model.add(aA + aB + aSEC + aCOR == demandSR);
        model.add(bA + bB + bSEC + bCOR == demandSN);

        // --- COMPOSIÇÃO SR ---
        model.add(secA*aA + secB*aB + aSEC >= secA*demandSR);
        model.add(corA*aA + corB*aB + aCOR >= corA*demandSR);

        // --- COMPOSIÇÃO SN ---
        model.add(secA*bA + secB*bB + bSEC >= secA*demandSN);
        model.add(corA*bA + corB*bB + bCOR >= corA*demandSN);

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::Threads, 1);
        if(cplex.solve()) {
            env.out() << "Custo mínimo = R$ " << cplex.getObjValue() << std::endl;
            env.out() << "SR (1000 L): SolA=" << cplex.getValue(aA)
                      << ", SolB=" << cplex.getValue(aB)
                      << ", SEC="  << cplex.getValue(aSEC)
                      << ", COR="  << cplex.getValue(aCOR) << std::endl;
            env.out() << "SN (250 L): SolA=" << cplex.getValue(bA)
                      << ", SolB=" << cplex.getValue(bB)
                      << ", SEC="  << cplex.getValue(bSEC)
                      << ", COR="  << cplex.getValue(bCOR) << std::endl;
        } else {
            env.out() << "Modelo não solucionado" << std::endl;
        }
    } catch(IloException &e) {
        std::cerr << "Erro CPLEX: " << e << std::endl;
    } catch(...) {
        std::cerr << "Erro desconhecido." << std::endl;
    }
    env.end();
    return 0;
}
