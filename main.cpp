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


//typedef IloArray<IloNumVarArray> NumVarMatrix;
//typedef IloArray<NumVarMatrix>   NumVar3Matrix;

//Basic Starting parameter CPLEX
using namespace std;


///Main
int main(int argc, char *argv[])
{
    

    // DECLARANDO CONJUNTOS
    int N; // Pegar numero de Ações (Linhas)
    int T; // pegar numero de dias (Colunas)

	//Abrir arquivo de entrada
    FILE* fp;
	fp = fopen(argv[1],"r");

	//LENDO DADOS DA ENTRADA
	fscanf(fp, "%d", &N);	
	fscanf(fp, "%d", &T);


    float **Rij;
    Rij = (float**) malloc( T * sizeof(float*));

	//Alocação das linhas com ingredientes
    for( int i = 0; i < N; i++){
        Rij[i] = (float*) malloc (T * sizeof(float));
    }

    //Pegar os valores
    for(int i = 0; i < N; i++){
        for(int j = 0; j < T; j++){
            fscanf(fp, "%f", &Rij[i][j]);
            //printf("%f", Rij[i][j]);
        }
        //printf("\n");
    }

    //Pegar Limite minino
    int R;
    fscanf(fp, "%d", &R);


    // DECLARANDO O AMBIENTE E O MODELO MATEMATICO
    IloEnv env;
	IloModel modelo;
    // iniciando o modelo
    modelo = IloModel(env);
    
    // DECLARAÇÃO DE VARIÁVEIS
    // parametros: ambiente, tamanho, valor min, valor max, tipo: ILOFLOAT, ILOINT, ILOBOOL
    ///IloNumVarArray x(env, T, 0, IloInfinity, ILOFLOAT); //mat[NxT]
    IloNumVarMatrix x(env,N); //mat[NxT]

    //adicionar minha variável no modelo
    for(int i = 0; i < N; i++){
        // DEFININDO O NOME DA VAR
        x[i] = IloNumVarArray(env, T, 0, IloInfinity, ILOFLOAT);
        for (int t=0; t < T; t++){
            stringstream var;
            var << "x["<<i+1<<"]["<<t+1<<"]: ";
            x[i][t] = IloNumVar(env, 0.0, 1.0, ILOFLOAT); //Nao negatividade     
            x[i][t].setName(var.str().c_str());
            modelo.add(x[i][t]);
        }
    }

    // DECLARAÇÃO DA FUNÇÃO OBJETIVO
    IloExpr fo(env); //fo do enviroment - Ambiente
   
    // montando a FO
    for(int i = 0; i < N; i++){
        for (int t=0; t < T; t++){
            fo += x[i][t]*Rij[i][t];
        }
    }
    
    //IloMinimize e IloMaximize
    modelo.add(IloMaximize(env, fo)); //Maximize o FO no ambiente env

    // DECLARAÇÃO DAS RESTRIÇÕES
    // para toda j (dia) 
    for(int i = 0; i < N; i++){

        //O somatório de xij * rij
        IloExpr rest1(env);
        for (int t=0; t < T; t++){
            rest1 += Rij[i][t] * x[i][t];
        }
        // meu somatório está completo, agora irei adicionar a restrição no modelo
        // IloRange nomeRange(env, valorMin, RESTRICAO, valorMax);
        rest1 -= R;
        IloRange acaoMin(env, 0, rest1, IloInfinity);
        // DEFININDO O NOME DA RESTRICAO
        stringstream var;
        var << "acaoMin["<<i+1<<"]: ";
        acaoMin.setName(var.str().c_str());
        modelo.add(acaoMin);    
    }          

    // para toda j (dia) 
    for (int t=0; t < T; t++){

        //O somatório de xij * rij
        IloExpr rest2(env);
        for(int i = 0; i < N; i++){
            rest2 += x[i][t];
        }
        // meu somatório está completo, agora irei adicionar a restrição no modelo
        // IloRange nomeRange(env, valorMin, RESTRICAO, valorMax);
        IloRange percentDia(env, 1, rest2, 1);

        // DEFININDO O NOME DA RESTRICAO
        stringstream var;
        var << "percentDia["<<t+1<<"]: ";
        percentDia.setName(var.str().c_str());
        modelo.add(percentDia);    
    }      



    // EXECUTAR MODELO
    IloCplex cplex(modelo);

    cplex.exportModel("artigo.lp");
    
    cplex.solve();


	//MOSTRANDO O MODELO
    cout<< "FO: " << cplex.getObjValue()<<endl;

    for(int i = 0; i < N; i++)
    {
        for(int t = 0; t < T; t++)
        {
            cout<<cplex.getValue(x[i][t]) << "\t";
        }
        cout<<endl;
    }

    return 0;
}












