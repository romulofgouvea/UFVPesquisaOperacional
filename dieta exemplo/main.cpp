//Basic Libraries C++
#include<iostream>
#include<stdio.h>
#include <sstream>
#include <ilcplex/ilocplex.h>

//Basic Libraries CPLEX
typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVar3Matrix;
typedef IloArray<IloNumVar3Matrix> IloNumVar4Matrix;
typedef IloArray<IloNumArray> IloNumMatrix;
typedef IloArray<IloNumMatrix> IloNum3Matrix;

//Basic Starting parameter CPLEX
using namespace std;


///Main
int main(int argc, char *argv[])
{
    

// DECLARANDO CONJUNTOS
    int I; // conjunto de ingredientes (qtd de ingredientes)
    int V; // conjunto de vitaminas (qtd de vitaminas)

    // lendo o arquivo
    FILE* fp;
    fp = fopen(argv[1],"r");

	//Número de colunas da matriz, ou o numero de ingredientes
    fscanf(fp, "%d", &I);
	//Númer de linhas da matriz, ou o numero de vitaminas
    fscanf(fp, "%d", &V);

	

    // declarando variáveis para FOR
    int i, v;
    
    //entrada Qtd: quantidade da vitamina v no ingrediente i
	//MAtriz com todos os dados de quantidades
    float **qtd;
    qtd = (float**) malloc( V * sizeof(float*));
	//Alocação das linhas com ingredientes
    for( v = 0; v < V; v++)
    {
        qtd[v] = (float*) malloc (I * sizeof(float));
    }

    // entrada P: preço por ingrediente
	//Vetor que representa o preço de cada ingrediente
    float *p;
    p = (float*) malloc(I * sizeof(float));

    // entrada Req: requisito mínimo da vitamina v
	//Vetor de requisitos para as vitaminas
    float *req;
    req = (float*) malloc(V * sizeof(float));

    // Preenchendo a matriz de qtd
    for(v = 0; v < V; v++)
    {
        for(i = 0; i < I; i++)
        {
            fscanf(fp, "%f", &qtd[v][i]);
        }
    }

    // Preenchendo o vetor de preços P
    for(i = 0; i < I ; i++)
    {
        fscanf(fp, "%f", &p[i]);
    }

    // lendo a linha final, que contem os requisitos de cada vit
    for(v = 0; v < V; v++)
    {
        fscanf(fp, "%f", &req[v]);
    }


    // DECLARANDO O AMBIENTE E O MODELO MATEMATICO
    IloEnv env;
	IloModel modelo;
    // iniciando o modelo
    modelo = IloModel(env);
    
    // DECLARAÇÃO DE VARIÁVEIS
    // parametros: ambiente, tamanho, valor min, valor max, tipo: ILOFLOAT, ILOINT, ILOBOOL
    IloNumVarArray x(env, I, 0, IloInfinity, ILOFLOAT);

    //adicionar minha variável no modelo
    for(i = 0; i < I; i++)
    {
        // DEFININDO O NOME DA VAR
        stringstream var;
        var << "x["<<i+1<<"]: ";
        x[i].setName(var.str().c_str()); 
    
        modelo.add(x[i]);
    }    


    // DEFININDO O NOME DA VAR
    //stringstream var;
    //var << "nomeVar["<<i+1<<"]: ";
    //NOMEVAR.setName(var.str().c_str());        

    // DECLARAÇÃO DA FUNÇÃO OBJETIVO
    IloExpr fo(env);
   
    // montando a FO
    for(i = 0; i < I; i++)
    {
        fo += p[i]*x[i];
    }
    
    //IloMinimize e IloMaximize
    modelo.add(IloMinimize(env, fo));

    
    // DECLARAÇÃO DAS RESTRIÇÕES
    //IloExpr rest1(env);
    // IloRange nomeRange(env, valorMin, RESTRICAO, valorMax);
    //modelo.add(nomeRange);  

    // para todo v 
    for( v = 0 ; v < V; v++)
    {
        //o somatório de qtd * x
        IloExpr rest1(env);
        for(i = 0 ; i < I; i++)
        {
            rest1 += qtd[v][i]*x[i];
        }
        // meu somatório está completo, agora irei adicionar a restrição no modelo
        // IloRange nomeRange(env, valorMin, RESTRICAO, valorMax);
        rest1 -= req[v];
        IloRange qtdMin(env, 0, rest1, IloInfinity);
        // DEFININDO O NOME DA RESTRICAO
        stringstream var;
        var << "ReqMin["<<v+1<<"]: ";
        qtdMin.setName(var.str().c_str());
        modelo.add(qtdMin);
            
    }              


    // EXECUTAR MODELO
    IloCplex cplex(modelo);

    cplex.exportModel("dieta.lp");
    
    cplex.solve();


    return 0;
}












