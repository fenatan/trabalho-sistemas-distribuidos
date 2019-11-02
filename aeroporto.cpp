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

//Função para gerar números inteiros aleatorios no intervalo entre n1 e n2
int gerarInteiroAleatorio(int n1, int n2){
    int menor = min(n1,n2);
    int maior = max(n1,n2);
    return rand() % (maior-menor+1) + menor; 
}

//Procedimento para gerar os voôs de partida de um aeroporto
void gerarVoos(int aeroportoID, int totalAeroportos, vector<VOO>& voosPartida){
    int numeroTotalDeVoos = gerarInteiroAleatorio(1,10); // Esse valor informa a quantidade total de voos de um aeroporto específico
    
    VOO aux;

    for(int i = 0; i < numeroTotalDeVoos; i++){
        aux.origem = aeroportoID;
        while (true){
            aux.destino = gerarInteiroAleatorio(1, totalAeroportos);
            if(aux.destino != aeroportoID)
                break;
        }
        aux.tempoVoo = gerarInteiroAleatorio(1, 10);
        aux.horarioEstimadoPartida = gerarInteiroAleatorio(1, 10);
        aux.horarioEstimadoChegada = aux.horarioEstimadoPartida + aux.tempoVoo;
        
        voosPartida.push_back(aux);
    }
}

int main(int argc, char* argv[]){

    srand((int)time(0)); //para gerar números aleatórios

    int rank, size;

    vector<VOO> voosPartida; // Vetor dos voos de partida do aeroporto
    vector<VOO> voosChegada; // Vetor dos voos de partida do aeroporto

    gerarVoos(rank, size, voosPartida);
    
    cout << gerarInteiroAleatorio(1,100) << endl;

    return 0;
}
