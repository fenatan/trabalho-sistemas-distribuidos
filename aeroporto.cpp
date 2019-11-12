/* Programa que simula um sistema distribuido entre aeroportos.
Desenvolvido por: FELIPE NATAN DOS SANTOS e JOSÉ GUILHERME BENITES MACHADO

	O programa gera aleatoriamente um numero de voos para cada aeroporto com origem e destino, depois através de uma comunicação MPI
o aeroporto origem comunica aos aeroportos destinos os voos que vai enviar. Após todos os aeroportos receberem seus voos eles imprimem na
tela sua tabela de voos do dia.
*/



#include<bits/stdc++.h>
#include <unistd.h> // função "usleep"
#include <ctime> // funções "rand" e "srand"
#include "mpi.h"
using namespace std;

typedef struct {
    int origem;
    int destino;
    int tempoVoo;
    int horarioPartida;
    int horarioChegada;
} VOO;

//Função de comparação para definir qual voo terá prioridade para decolar
bool compPartida(VOO a, VOO b){
    if(a.horarioPartida < b.horarioPartida)
        return true;
    if(a.horarioPartida > b.horarioPartida)
        return false;
    if(a.tempoVoo > b.tempoVoo)
        return true;
    return false;
}

//Função de comparação para definir qual voo terá prioridade para pousar
bool compChegada(VOO a, VOO b){
    if(a.horarioChegada < b.horarioChegada)
        return true;
    if(a.horarioChegada > b.horarioChegada)
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
void gerarVoos(int aeroportoID, int totalAeroportos, vector<VOO>& voosPartida,  vector<int>& qtdeVoosPartida){
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
        aux.horarioPartida = gerarInteirosAleatorio(1, 10);
        aux.horarioChegada = aux.horarioPartida + aux.tempoVoo;
        
        qtdeVoosPartida[aux.destino]++;

        voosPartida.push_back(aux);
    }
    sort(voosPartida.begin(),voosPartida.end(), compPartida); //ordenação do vetor de partidas

    //Tratamento para voos que saem no mesmo horário
    for(int i = 0; i < numeroTotalDeVoos-1; i++){
        if(voosPartida[i+1].horarioPartida <= voosPartida[i].horarioPartida){
            voosPartida[i+1].horarioPartida = voosPartida[i].horarioPartida+1;
            voosPartida[i+1].horarioChegada = voosPartida[i+1].horarioPartida + voosPartida[i+1].tempoVoo;
        }
    }
}

void imprimirVoos(vector<VOO>& v){
    cout << "Origem   " << "Destino   " << "Tempo_de_Voo   " <<  
    "Horario_Partida   " << "Horario_Chegada" << endl;
    for(auto x : v){
        cout << x.origem << "\t   " << x.destino 
        << "\t\t" << x.tempoVoo << "\t\t" << x.horarioPartida << "\t\t" << x.horarioChegada <<endl;
    }
    cout << "--------------------------------------------------------------------------" << endl;
}

// Procedimento para avisar a quantidade de voos que serão enviados para cada aeroporto
void comunicarVoos(int rank, int size, vector<VOO>& v, vector<int>& qtdeVoosPartida, vector<int>& qtdeVoosChegada){
    int dest, orig, receb;
    MPI_Status status;
    
    for(dest=0;dest < size; dest++)
    {
        if(dest != rank)
        {
            cout << "Enviando " << qtdeVoosPartida[dest] << " voos para o aeroporto " << dest << endl;
            MPI_Send(&qtdeVoosPartida[dest], 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
       
    }
    cout << endl;
    for(orig=0;orig < size; orig++)
    {
        if(orig != rank)
        {
            MPI_Recv(&receb, 1, MPI_INT, orig, 0, MPI_COMM_WORLD,&status);
			qtdeVoosChegada[orig] = receb;
            cout << "Recebendo " << receb << " voos do aeroporto " << orig << endl;
        }
       
    }
    cout << "--------------------------------------------------------------------------" << endl;
}

//procedimento para enviar voos
void enviarVoos(int rank, int size, vector<VOO>& voosPartida){
   
    for(long unsigned int i = 0; i < voosPartida.size(); i++){
        MPI_Send(&voosPartida[i], sizeof(VOO), MPI_BYTE, voosPartida[i].destino, 0, MPI_COMM_WORLD);
    }

}

//Procedimento para receber voos
void receberVoos(int rank, int size, vector<VOO>& voosChegada, vector<int>& qtdeVoosChegada){
    MPI_Status status;

    VOO aux;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < qtdeVoosChegada[i]; j++){
            MPI_Recv(&aux, sizeof(VOO), MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);
            voosChegada.push_back(aux);
        }
    }

    sort(voosChegada.begin(), voosChegada.end(), compChegada);

    //Tratamento para voos que chegam no mesmo horário
    for(long unsigned int i = 0; i < voosChegada.size()-1; i++){
        if(voosChegada[i+1].horarioChegada <= voosChegada[i].horarioChegada){
            voosChegada[i+1].horarioChegada = voosChegada[i].horarioChegada+1;
        }
    }
}

int main(int argc, char* argv[]){

    srand((int)time(0)); //para gerar números aleatórios
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    vector<VOO> voosPartida; // Vetor dos voos de partida do aeroporto
    vector<VOO> voosChegada; // Vetor dos voos de chegada do aeroporto
    vector<int> qtdeVoosPartida(size, 0); //Vetor com a quantidade de voos que partirão desse aeroporto
    vector<int> qtdeVoosChegada(size, 0); //Vetor com a quantidade de voos que chegarão de cada aeroporto
    
    gerarVoos(rank, size, voosPartida, qtdeVoosPartida);
    cout << "Decolagens do aeroporto " << rank << endl;
    imprimirVoos(voosPartida);
    comunicarVoos(rank, size, voosPartida, qtdeVoosPartida, qtdeVoosChegada);
    enviarVoos(rank, size, voosPartida);
    receberVoos(rank, size, voosChegada, qtdeVoosChegada);
    cout << endl << "Pousos no aeroporto " << rank << endl;
    imprimirVoos(voosChegada);
    
    MPI_Finalize();
    return 0;
}
