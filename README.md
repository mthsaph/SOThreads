---

# 🔢 Matrix Prime Counter (C++ / Windows Threads)

Este projeto implementa um sistema para **contar quantos números primos aparecem em uma matriz NxM**, utilizando duas abordagens:

### Singlethread (processamento sequencial)
### Multithread com Windows API (`CreateThread`, `CRITICAL_SECTION`)

O objetivo é **comparar o tempo de execução** entre as duas abordagens ao processar uma matriz grande (ex.: 1900×1900).

---

## 🚀 Funcionalidades Principais

### ✔️ Geração de Matriz

* A matriz é alocada dinamicamente (int**).
* Pode ser preenchida com números aleatórios usando uma seed definida.

### ✔️ Divisão em Submatrizes

O grid principal é dividido em sub-blocos de tamanho:

```
subN × subM
```

Cada submatriz vira uma tarefa a ser processada por uma thread.

### ✔️ Execução Multithread

O programa:

* Cria `nThreads` threads suspensas com `CreateThread()`
* Usa um vetor global `vecPar` contendo os limites de cada submatriz
* Cada thread:

  * Retira um bloco de `vecPar` (estilo *work stealing*)
  * Conta quantos números primos existem naquele bloco
  * Atualiza o resultado global usando `CRITICAL_SECTION`

### ✔️ Execução Singlethread

Percorre a matriz inteira em um único loop, contando todos os números primos.

### ✔️ Comparação de Performance

O programa exibe:

* Quantidade de primos encontrados
* Tempo de execução da abordagem **singlethread**
* Tempo de execução da abordagem **multithread**

---

## 🧩 Estrutura do Código

### 🔹 `genMATRIX(n, m)`

Aloca dinamicamente uma matriz NxM.

### 🔹 `fillMATRIX(...)`

Preenche a matriz com números aleatórios.

### 🔹 `isPrime(valor)`

Determina se um número é primo.

*(⚠️ implementação simples e não otimizada)*

### 🔹 `defPar(...)`

Divide a matriz em submatrizes e gera o vetor global `vecPar`.

Cada item contém:

* ID da tarefa
* Posições de início/fim (linhas/colunas)
* Ponteiro da matriz

### 🔹 `qntPrimeSingle(...)`

Processa toda a matriz de forma sequencial.

### 🔹 `qntPrimeMulti(...)`

Função executada por cada thread:

1. Retira uma submatriz de `vecPar`
2. Conta quantos primos existem nela
3. Atualiza `primosTotal` com proteção via `CRITICAL_SECTION`
4. Finaliza com `_endthread()`

---

## 🧵 Multithreading

O código usa recursos nativos do Windows:

* `CreateThread()`
* `ResumeThread()`
* `WaitForMultipleObjects()`
* `CRITICAL_SECTION`
* `_endthread()`

### Estratégia de paralelização:

Cada thread executa enquanto houver submatrizes no vetor global:

```cpp
while (vecPar.size() != 0) {
    parametro* p = &vecPar.back();
    vecPar.pop_back();
    // processa submatriz
}
```

---

## ⏱️ Performance

O programa mede o tempo total com `time(NULL)` para:

* Execução **singlethread**
* Execução **multithread**

E imprime:

```
Total de números primos SINGLETHREAD: X
Tempo de execução: Y segundos

Total de números primos MULTITHREAD: X
Tempo de execução: Z segundos
```

---

## 📌 Dependências

* Windows (usa API nativa)
* `process.h`
* `windows.h`
* Compilador compatível (MinGW, MSVC)

---

## 📁 Estrutura Geral

```
main.cpp
│
├── genMATRIX / delMATRIX
├── fillMATRIX
├── isPrime
├── qntPrimeSingle
├── qntPrimeMulti (Worker Threads)
├── defPar (Divide matriz)
└── Variáveis globais (vecPar, primosTotal, seção crítica)
```
