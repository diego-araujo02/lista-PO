#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>

using namespace std;
using namespace std::chrono;

// Representação da Solução
struct Solution {
    vector<int> assignment;  // Atribuição de itens para bins
    vector<double> bin_totals; // Capacidade ocupada em cada bin
    int k;                   // Número total de bins
    
    Solution(const vector<double>& items) {
        assignment.resize(items.size(), -1);
        bin_totals.clear();
        k = 0;
    }
};

// Função de Avaliação
int evaluate(const vector<double>& bin_totals) {
    return bin_totals.size();
}

// solução inicial com First Fit Decreasing
Solution first_fit_decreasing(const vector<double>& items) {
    Solution sol(items);
    
    // ordem decrescente
    vector<pair<double, int>> sorted_items;
    for(int i = 0; i < items.size(); i++) {
        sorted_items.emplace_back(items[i], i);
    }
    sort(sorted_items.rbegin(), sorted_items.rend());

    // First Fit
    for(const auto& [size, idx] : sorted_items) {
        bool placed = false;
        for(int b = 0; b < sol.bin_totals.size(); b++) {
            if(sol.bin_totals[b] + size <= 1.0 + 1e-6) { 
                sol.assignment[idx] = b;
                sol.bin_totals[b] += size;
                placed = true;
                break;
            }
        }
        if(!placed) {
            sol.assignment[idx] = sol.bin_totals.size();
            sol.bin_totals.push_back(size);
        }
    }
    sol.k = sol.bin_totals.size();
    return sol;
}

// First Improvement
Solution local_search(Solution sol, const vector<double>& items, double time_limit) {
    auto start = high_resolution_clock::now();
    int best_k = sol.k;
    
    // ordem decrescente
    vector<int> item_order(items.size());
    for(int i = 0; i < items.size(); i++) item_order[i] = i;
    sort(item_order.begin(), item_order.end(), 
        [&](int a, int b){ return items[a] > items[b]; });

    while(true) {
        bool improved = false;
        
        // tenta mover cada item para outro bin
        for(int item_idx : item_order) {
            double size = items[item_idx];
            int current_bin = sol.assignment[item_idx];
            
            // Tenta encontrar um bin melhor
            for(int target_bin = 0; target_bin < sol.bin_totals.size(); target_bin++) {
                if(target_bin == current_bin) continue;
                
                // Verifica se o item cabe no bin alvo
                if(sol.bin_totals[target_bin] + size <= 1.0 + 1e-6) {
                    
                    // calcula novos valores temporários
                    double new_current = sol.bin_totals[current_bin] - size;
                    double new_target = sol.bin_totals[target_bin] + size;
                    
                    // Verifica se melhora a solução
                    int potential_k = sol.k - (new_current < 1e-6 ? 1 : 0);
                    
                    if(potential_k < sol.k) {
                        // atualiza a solução
                        sol.bin_totals[current_bin] = new_current;
                        sol.bin_totals[target_bin] = new_target;
                        sol.assignment[item_idx] = target_bin;
                        
                        // Remove bin vazio se necessário
                        if(new_current < 1e-6) {
                            sol.bin_totals.erase(sol.bin_totals.begin() + current_bin);
                            for(int& bin : sol.assignment) {
                                if(bin > current_bin) bin--;
                            }
                        }
                        
                        sol.k = sol.bin_totals.size();
                        improved = true;
                        goto improvement_found; // First improvement
                    }
                }
            }
        }
        improvement_found:
        
        auto now = high_resolution_clock::now();
        double elapsed = duration_cast<duration<double>>(now - start).count();
        
        if(!improved || elapsed >= time_limit || sol.k == 1) break;
    }
    return sol;
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        cout << "Use: " << argv[0] << " <tempo_limite> <input_arquivo>\n";
        return 1;
    }

    double time_limit = stod(argv[1]);
    vector<double> items;
    
    ifstream file(argv[2]);
    double item;
    while(file >> item) {
        items.push_back(item);
    }

    // Gera solução inicial
    Solution sol = first_fit_decreasing(items);
    
    // Busca local
    sol = local_search(move(sol), items, time_limit);
    
    cout << "Número mínimo de bins encontrado: " << sol.k << endl;
    
    return 0;
}