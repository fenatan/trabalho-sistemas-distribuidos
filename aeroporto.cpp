#include<bits/stdc++.h>
#include <unistd.h> // função "usleep"
#include <ctime> // funções "rand" e "srand"
//#include "mpi.h"
using namespace std;

typedef struct {
    int origem;
    int destino;
    int tempoVoo;
    int horarioEstimadoPartida;
    int horarioReaPartida;
    int horarioEstimadoChegada;
    int horarioRealChegada;
} VOO;

//Função de comparação para definir qual voo terá prioridade para decolar/pousar
bool comp(VOO a, VOO b){
    if(a.horarioEstimadoPartida < b.horarioEstimadoPartida)
        return true;
    if(a.horarioEstimadoPartida > b.horarioEstimadoPartida)
        return false;
    if(a.tempoVoo > b.tempoVoo)
        return true;
    return false;
}

//Função para gerar números inteiros aleatorios no intervalo entre n1 e n2
int gerarInteirosAleatorio(int n1, int n2){
    int menor = min(n1,n2);
    int maior = max(n1,n2);
    return rand() % (maior-menor+1) + menor; 
}

//Procedimento para gerar os voôs de partida de um aeroporto
void gerarVoos(int aeroportoID, int totalAeroportos, vector<VOO>& voosPartida){
    int numeroTotalDeVoos = gerarInteirosAleatorio(1,10); // Esse valor informa a quantidade total de voos de um aeroporto específico
    
    VOO aux;

    for(int i = 0; i < numeroTotalDeVoos; i++){
        aux.origem = aeroportoID;
        while (true){
            aux.destino = gerarInteirosAleatorio(0, totalAeroportos-1);
            if(aux.destino != aeroportoID)
                break;
        }
        aux.tempoVoo = gerarInteirosAleatorio(1, 10);
        aux.horarioEstimadoPartida = gerarInteirosAleatorio(1, 10);
        aux.horarioEstimadoChegada = aux.horarioEstimadoPartida + aux.tempoVoo;
        
        voosPartida.push_back(aux);
    }
    sort(voosPartida.begin(),voosPartida.end(), comp); //ordenação do vetor de partidas
}

void imprimirVoos(vector<VOO>& v){
    cout << "Origem   " << "Destino   " << "Tempo_de_Voo   " <<  
    "Horario_Partida   " << "Horario_Chegada" << endl;
    for(auto x : v){
        cout << x.origem << "\t   " << x.destino 
        << "\t\t" << x.tempoVoo << "\t\t" << x.horarioEstimadoPartida << "\t\t" << x.horarioEstimadoChegada <<endl;
    }
}

int main(int argc, char* argv[]){

    srand((int)time(0)); //para gerar números aleatórios

    int rank = 1, size = 8;

    vector<VOO> voosPartida; // Vetor dos voos de partida do aeroporto
    vector<VOO> voosChegada; // Vetor dos voos de partida do aeroporto
    //vector<int> qtdeVoosPartida(size+1, 0); //Vetor com a quantidade de voos que partirão desse aeroporto
    vector<int> qtdeVoosChegada(size+1, 0); //Vetor com a quantidade de voos que chegarão de cada aeroporto
    
    gerarVoos(rank, size, voosPartida);
    imprimirVoos(voosPartida);
    //cout << gerarInteirosAleatorio(1,100) << endl;

    return 0;
}
