#include <iostream>
#include <vector>
#include <process.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <locale.h>

using namespace std;

void qntPrimeMulti(void* parametroF); // função que é passada para as threads
bool isPrime(int valor); // verifica se o numero é primo
int** genMATRIX(int n, int m); // aloca a memoria para a matriz de tamanho nxm
void delMATRIX(int** matrix, int n, int m); // deleta a memoria alocada para a matriz
void printMATRIX(int** matrix, int n, int m); // imprime a matriz
void qntPrimeSingle(int** endM, int n, int m); // preenche a variavel primosTotal com o numero de numeros primos usando singlethread
void printMENU(); // imprime o menu na tela
void defPar(int n, int m, int subN, int subM, int** endM); // função que define o vector de parametros, "bounderies" das matrizes
void fillMATRIX(int** endM, int n, int m); // função para preencher as matriz com numeros aleatorios com base na seed

typedef struct {
	int id;
	int startIM;
	int startJM;
	int endIM;
	int endJM;
	int** pointerM;
} parametro; // declarando os parâmetros

int primosTotal = 0;
CRITICAL_SECTION hSecaoCritica; // VARIÁVEL DE SEÇÃO CRÍTICA
vector<parametro> vecPar;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// FIM DAS DECLARAÇÕES GLOBAIS/PROTOTIPAÇÕES E COMEÇO DA MAIN////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

	InitializeCriticalSection(&hSecaoCritica);
	int n = 1900, m = 1900, nThreads = 4, subN = 100, subM = 100, seed = 666;
	srand(seed);
	setlocale(LC_ALL, "pt_BR.UTF-8");
	time_t t0, t1;
	vector<HANDLE> vecTID;

	// cin >> n, m;

	int** endM = genMATRIX(n, m);

	fillMATRIX(endM, n, m);

	cout << vecPar.size() << endl;

	defPar(n, m, subN, subM, endM);

	cout << vecPar.size() << endl;

	for (int i = 0; i < nThreads; i++) {
		vecTID.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&qntPrimeMulti, NULL, CREATE_SUSPENDED, NULL));
	}

	t0 = time(NULL);
	qntPrimeSingle(endM, n, m);
	t1 = time(NULL);

	cout << "Total de números primos SINGLETHREAD: " << primosTotal << endl;
	cout << "Tempo de execução do programa: " << t1 - t0 << " segundos" << endl;

	primosTotal = 0;

	t0 = time(NULL);

	for (int i = 0; i < nThreads; i++) {
		ResumeThread(vecTID[i]);
	}

	WaitForMultipleObjects(nThreads, vecTID.data(), true, INFINITE);

	t1 = time(NULL);
	cout << "Total de números primos MULTITHREAD: " << primosTotal << endl;
	cout << "Tempo de execução do programa: " << t1 - t0 << " segundos" << endl;

	delMATRIX(endM, n, m);
	for (int i = 0; i < nThreads; i++) { // CLOSING HADLETS PQ SIM
		CloseHandle(vecTID[i]);
	}
	DeleteCriticalSection(&hSecaoCritica);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// FIM DA MAIN E COMEÇO DAS FUNÇÕES//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isPrime(int valor) { // FUNÇÃO QUE RETORNA TRUE PARA NUMERO PRIMO E FALSE PARA NAO PRIMO

	if (valor > 0) {

		for (int k = 2; k < valor; k++) {
			if (valor % k == 0) {
				return FALSE;
			}
		}

		return TRUE;
	}
	else
		return FALSE;
}

int** genMATRIX(int n, int m) { // função que gera a matrix todas as posições são inicializadas em 0

	int** matrix = new int*[n];

	for (int i = 0; i < n; i++)
		matrix[i] = new int[m];

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			matrix[i][j] = 0;

	return matrix;
}

void delMATRIX(int** matrix, int n, int m) { // deleta a matrix pra não ficar alocando memoria infinitamente

	for (int i = 0; i < n; i++)
		delete[] matrix[i];

	delete[] matrix;
}

void printMATRIX(int** matrix, int n, int m) { // imprime a matriz na tela

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			cout << matrix[i][j] << "	";
		cout << endl;
	}
}

void qntPrimeSingle(int** endM, int n, int m) { // soma a quantidade de numeros primos e aloca nos primosTotal

	int alvo = 0;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			alvo = endM[i][j];
			if (isPrime(alvo))
				primosTotal += 1;
		}
	}
}

void printMENU() { // imprime o menu

	cout << "1 - Definir o tamanho da matriz (PADRÃO = 1X1)" << endl;
	cout << "2 - Definir semente para o gerador de números aleatórios (PADRÃO = 666)" << endl;
	cout << "3 - Preencher a matriz com números aleatórios (PADRÃO = [n][m] = 0)" << endl;
	cout << "4 - Definir o tamanho das submatrizes (PADRÃO = 1X1)" << endl;
	cout << "5 - Definir o número de Threads (PADRÃO = 1)" << endl;
	cout << "6 - Executar" << endl;
	cout << "7 - Visualizar o tempo de execução e quantidade de números primos;" << endl;
	cout << "8 - Encerrar" << endl;

}

void defPar(int n, int m, int subN, int subM, int** endM ) { // define o vector de parametros global que as threads usam de bounderies 

	parametro addV{};

	for (int i = 0; i <= n / subN; i++) {
		for (int j = 0; j <= m / subM; j++) {

			addV.id = rand() % 100;
			if (i == n / subN) {
				addV.startIM = i * subN;
				addV.endIM = n;
			}
			else {
				addV.startIM = i * subN;
				addV.endIM = (i + 1) * subN;
			}

			if (j == m / subM) {
				addV.startJM = j * subM;
				addV.endJM = m;
			}
			else {
				addV.startJM = j * subM;
				addV.endJM = (j + 1) * subM;
			}

			addV.pointerM = endM;
			vecPar.push_back(addV);

		}
	}
}

void fillMATRIX(int** endM, int n, int m) { // PREENCHE A MATRIZ COM NUMEROS ALEATORIOS
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			endM[i][j] = rand() % 99999999 + 1;
	}
}

void qntPrimeMulti(void* parametroMATRIX) { // PERCORRE A SUBMATRIZ CHAMANDO A FUNÇÃO isPrime a cada numero

	int primoN = 0, alvo = 0;

	while (vecPar.size() != 0) {

		if (vecPar.size() == 0) 
			break;
		else {

			parametro* pointerP = &vecPar.back();
			vecPar.pop_back();

			for (int i = pointerP->startIM; i < pointerP->endIM; i++) {
				for (int j = pointerP->startJM; j < pointerP->endJM; j++) {
					alvo = pointerP->pointerM[i][j];
					if (isPrime(alvo))
						primoN += 1;
				}
			}
		}
	}

	EnterCriticalSection(&hSecaoCritica);

	primosTotal += primoN;

	LeaveCriticalSection(&hSecaoCritica);

	_endthread();
}