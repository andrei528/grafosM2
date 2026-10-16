#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <set>
#include <map>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <utility>
#include "LDE.hpp"

using namespace std;

struct ResultadoAGM
{
    vector<pair<int, int>> arestas;
    double pesoTotal = 0.0;
    double tempoMs = 0.0;
    bool completa = false;
};

struct ResultadoColoracao
{
    vector<int> vertices;
    vector<int> cores;
    int quantidadeCores = 0;
    double tempoMs = 0.0;
};

bool direc, ponderado;

template <typename T>
ResultadoAGM prim(T &grafo)
{
    auto inicio = chrono::steady_clock::now();

    ResultadoAGM resultado;
    vector<int> vertices = grafo.listarVertices();
    int n = vertices.size();

    if (n == 0)
    {
        resultado.completa = true;
        auto fim = chrono::steady_clock::now();
        resultado.tempoMs =
            chrono::duration<double, milli>(fim - inicio).count();
        return resultado;
    }

    // Mapeia os IDs reais dos vertices para suas posicoes.
    map<int, int> posicao;
    for (int i = 0; i < n; i++)
    {
        posicao[vertices[i]] = i;
    }

    vector<bool> naArvore(n, false);
    naArvore[0] = true;

    int verticesIncluidos = 1;

    // Enquanto houver vertices fora da arvore.
    while (verticesIncluidos < n)
    {
        double menorPeso = numeric_limits<double>::max();
        int origem = -1;
        int destino = -1;

        // Procura a menor aresta entre a arvore e os vertices externos.
        for (int i = 0; i < n; i++)
        {
            if (!naArvore[i])
            {
                continue;
            }

            for (int vizinho : grafo.retornarVizinhos(vertices[i]))
            {
                auto it = posicao.find(vizinho);

                if (it == posicao.end())
                {
                    continue;
                }

                int j = it->second;

                if (naArvore[j])
                {
                    continue;
                }

                double peso = grafo.pesoAresta(vertices[i], vizinho);

                if (peso >= 0 && peso < menorPeso)
                {
                    menorPeso = peso;
                    origem = vertices[i];
                    destino = vizinho;
                }
            }
        }

        // Nao existe aresta ligando a arvore ao restante do grafo.
        if (origem == -1)
        {
            resultado.completa = false;
            break;
        }

        resultado.arestas.push_back({origem, destino});
        resultado.pesoTotal += menorPeso;

        naArvore[posicao[destino]] = true;
        verticesIncluidos++;
    }

    resultado.completa = (verticesIncluidos == n);

    auto fim = chrono::steady_clock::now();
    resultado.tempoMs =
        chrono::duration<double, milli>(fim - inicio).count();

    return resultado;
}

struct ArestaAGM
{
    int origem;
    int destino;
    double peso;
};

int encontrarConjunto(vector<int> &pai, int vertice)
{
    if (pai[vertice] != vertice)
    {
        pai[vertice] = encontrarConjunto(pai, pai[vertice]);
    }

    return pai[vertice];
}

void unirConjuntos(
    vector<int> &pai,
    vector<int> &altura,
    int a,
    int b)
{
    a = encontrarConjunto(pai, a);
    b = encontrarConjunto(pai, b);

    if (a == b)
    {
        return;
    }

    if (altura[a] < altura[b])
    {
        pai[a] = b;
    }
    else if (altura[a] > altura[b])
    {
        pai[b] = a;
    }
    else
    {
        pai[b] = a;
        altura[a]++;
    }
}

template <typename T>
ResultadoAGM kruskal(T &grafo)
{
    auto inicio = chrono::steady_clock::now();

    ResultadoAGM resultado;
    vector<int> vertices = grafo.listarVertices();
    int n = vertices.size();

    if (n == 0)
    {
        resultado.completa = true;

        auto fim = chrono::steady_clock::now();
        resultado.tempoMs =
            chrono::duration<double, milli>(fim - inicio).count();

        return resultado;
    }

    map<int, int> posicao;

    for (int i = 0; i < n; i++)
    {
        posicao[vertices[i]] = i;
    }

    vector<ArestaAGM> arestas;

    // Recolhe cada aresta não direcionada apenas uma vez.
    for (int i = 0; i < n; i++)
    {
        int u = vertices[i];

        for (int v : grafo.retornarVizinhos(u))
        {
            auto it = posicao.find(v);

            if (it == posicao.end())
            {
                continue;
            }

            int j = it->second;

            if (i >= j)
            {
                continue;
            }

            double peso = grafo.pesoAresta(u, v);

            if (peso >= 0)
            {
                arestas.push_back({u, v, peso});
            }
        }
    }

    // Ordena as arestas do menor peso para o maior.
    sort(arestas.begin(), arestas.end(),
         [](const ArestaAGM &a, const ArestaAGM &b)
         {
             return a.peso < b.peso;
         });

    vector<int> pai(n);
    vector<int> altura(n, 0);

    for (int i = 0; i < n; i++)
    {
        pai[i] = i;
    }

    // Processa as arestas em ordem crescente.
    for (const ArestaAGM &aresta : arestas)
    {
        int u = posicao[aresta.origem];
        int v = posicao[aresta.destino];

        int conjuntoU = encontrarConjunto(pai, u);
        int conjuntoV = encontrarConjunto(pai, v);

        // Só adiciona se os vértices estiverem em árvores diferentes.
        if (conjuntoU != conjuntoV)
        {
            resultado.arestas.push_back(
                {aresta.origem, aresta.destino});

            resultado.pesoTotal += aresta.peso;

            unirConjuntos(pai, altura, conjuntoU, conjuntoV);

            // Uma árvore com n vértices possui n - 1 arestas.
            if (resultado.arestas.size() == (size_t)(n - 1))
            {
                break;
            }
        }
    }

    resultado.completa =
        (resultado.arestas.size() == (size_t)(n - 1));

    auto fim = chrono::steady_clock::now();
    resultado.tempoMs =
        chrono::duration<double, milli>(fim - inicio).count();

    return resultado;
}

void imprimirResultadoAGM(const ResultadoAGM &resultado)
{
    if (!resultado.completa)
    {
        cout << "\nNao foi possivel encontrar uma arvore geradora "
             << "minima completa.\n";
        cout << "O grafo pode ser desconexo.\n";
        cout << fixed << setprecision(3);
        cout << "Tempo de execucao: "
             << resultado.tempoMs << " ms\n";
        return;
    }

    cout << "\n=== Resultado da AGM ===\n";

    cout << "Arestas selecionadas:\n";
    for (const auto &aresta : resultado.arestas)
    {
        cout << aresta.first << " - " << aresta.second << '\n';
    }

    cout << fixed << setprecision(2);
    cout << "Peso total: " << resultado.pesoTotal << '\n';

    cout << fixed << setprecision(3);
    cout << "Tempo de execucao: "
         << resultado.tempoMs << " ms\n";
}

class Grafos
{
protected:
    bool directed;
    bool pondered;
    int numVertices;

public:
    Grafos(bool directed, bool pondered, int numVertices) : directed(directed), pondered(pondered), numVertices(numVertices) {}
    bool isDirected()
    {
        return directed;
    }
    bool isPondered()
    {
        return pondered;
    }

    virtual ~Grafos() { cout << "Destrutor da classe Grafos chamado."; }
};

class GrafoMatriz : public Grafos
{
private:
    vector<vector<float>> matrizAdj;
    vector<string> labels;

public:
    GrafoMatriz(bool directed, bool pondered, int numVertices) : Grafos(directed, pondered, numVertices), matrizAdj(numVertices, vector<float>(numVertices, 0))
    {
        for (int i = 0; i < numVertices; i++)
        {
            labels.push_back("v" + to_string(i));
        }
    }

    // Funções de vértices

    // Inserir vertice
    bool inserirVertice(string label)
    {
        for (int i = 0; i < numVertices; i++)
        {
            matrizAdj[i].push_back(0);
        }
        vector<float> vetorTemp(numVertices + 1, 0);
        matrizAdj.push_back(vetorTemp);
        labels.push_back(label);
        numVertices++;
        return true;
    }

    // Remover vertice
    bool removerVertice(int id)
    {
        if (id < 0 || id >= numVertices)
            return false;
        matrizAdj.erase(matrizAdj.begin() + id);
        for (int i = 0; i < numVertices - 1; i++)
        {
            matrizAdj[i].erase(matrizAdj[i].begin() + id);
        }
        labels.erase(labels.begin() + id);
        numVertices--;
        return true;
    }

    // Retorna o nome do vértice
    string labelVertice(int id)
    {
        if (id < 0 || id >= numVertices)
            return "Id de vertice nao existe";
        return labels[id];
    }

    // Funções de arestas

    bool inserirAresta(int origem, int destino, float peso = 1.0)
    {
        if (origem < 0 || origem >= numVertices || destino < 0 || destino >= numVertices)
            return false;

        float valor = pondered ? peso : 1;

        matrizAdj[origem][destino] = valor;
        if (!directed)
        {
            matrizAdj[destino][origem] = valor;
        }
        return true;
    }

    // Remove a aresta
    bool removerAresta(int origem, int destino)
    {
        if (origem < 0 || origem >= numVertices || destino < 0 || destino >= numVertices)
            return false;

        matrizAdj[origem][destino] = 0;

        if (!directed)
        {
            matrizAdj[destino][origem] = 0;
        }
        return true;
    }

    // Verifica se a aresta existe
    bool existeAresta(int origem, int destino)
    {
        if (origem < 0 || origem >= numVertices || destino < 0 || destino >= numVertices)
            return false;

        return matrizAdj[origem][destino] > 0;
    }

    // Mostra a matriz do grafo
    void imprimeGrafo()
    {
        cout << "Matriz de Adjacencia:" << endl;

        for (int i = 0; i < numVertices; i++)
        {
            for (int j = 0; j < numVertices; j++)
            {
                cout << matrizAdj[i][j] << " ";
            }

            cout << endl;
        }
    }

    // Retorna o peso da aresta
    float pesoAresta(int origem, int destino)
    {
        if (origem < 0 || origem >= numVertices || destino < 0 || destino >= numVertices)
        {
            return -1;
        }
        else if (matrizAdj[origem][destino] == 0)
        {
            return -1;
        }
        else
        {
            return matrizAdj[origem][destino];
        }
    }

    // Função para retornar vizinhos
    vector<int> retornarVizinhos(int id)
    {
        if (id < 0 || id >= numVertices)
            return {};
        vector<int> vizinhos;
        for (int i = 0; i < numVertices; i++)
        {
            if (matrizAdj[id][i] > 0)
            {
                vizinhos.push_back(i);
            }
        }
        return vizinhos;
    }

    vector<int> listarVertices()
    {
        vector<int> vertices;
        for (int i = 0; i < numVertices; i++)
        {
            vertices.push_back(i);
        }
        return vertices;
    }
};

struct VerticeInfo
{
    string label;
    LDE<float> arestas;
};

class GrafoLista : public Grafos
{
private:
    LDE<VerticeInfo> listaVertices;

public:
    GrafoLista(bool directed, bool pondered, int numVertices) : Grafos(directed, pondered, numVertices), listaVertices()
    {
        for (int i = 0; i < numVertices; i++)
        {
            insertNode(&listaVertices, i, VerticeInfo{"v" + to_string(i), LDE<float>()});
        }
    }

    ~GrafoLista()
    {
        Node<VerticeInfo> *aux = listaVertices.head;
        while (aux != NULL)
        {
            Node<VerticeInfo> *temp = aux;
            aux = aux->prox;
            deleteLDE(&temp->data.arestas);
            delete temp;
        }
        listaVertices.head = listaVertices.tail = NULL;
    }

    GrafoLista(GrafoLista &&outro) noexcept : Grafos(outro.directed, outro.pondered, outro.numVertices), listaVertices(outro.listaVertices)
    {
        outro.listaVertices.head = NULL;
        outro.listaVertices.tail = NULL;
    }

    // funções básicas para vértice

    // Insere um novo vertice
    bool inserirVertice(string label)
    {
        int id = (listaVertices.tail == NULL) ? 0 : listaVertices.tail->id + 1;
        insertNode(&listaVertices, id, VerticeInfo{label, LDE<float>()});
        numVertices++;

        return true;
    }

    // Remove um vertice
    bool removerVertice(int id)
    {
        Node<VerticeInfo> *vertice = findNode(&listaVertices, id);
        if (vertice == NULL)
            return false;

        Node<VerticeInfo> *aux = listaVertices.head;
        while (aux != NULL)
        {
            removeNode(&aux->data.arestas, id);
            aux = aux->prox;
        }
        deleteLDE(&vertice->data.arestas);
        removeNode(&listaVertices, id);
        numVertices--;
        return true;
    }

    // Retorna nome do vertice
    string labelVertice(int id)
    {
        Node<VerticeInfo> *vertice = findNode(&listaVertices, id);

        if (vertice == NULL)
            return "Id de vertice nao existe";

        return vertice->data.label;
    }

    // Funções de arestas

    // Insere a aresta
    bool inserirAresta(int origem, int destino, float peso = 1.0)
    {

        Node<VerticeInfo> *vOrigem = findNode(&listaVertices, origem);
        Node<VerticeInfo> *vDestino = findNode(&listaVertices, destino);

        if (vOrigem == NULL || vDestino == NULL)
            return false;

        float valor = pondered ? peso : 1;

        insertNode(&vOrigem->data.arestas, destino, valor);

        if (!directed)
        {
            insertNode(&vDestino->data.arestas, origem, valor);
        }
        return true;
    }

    // Remove aresta
    bool removerAresta(int origem, int destino)
    {

        Node<VerticeInfo> *vOrigem = findNode(&listaVertices, origem);
        Node<VerticeInfo> *vDestino = findNode(&listaVertices, destino);

        if (vOrigem == NULL || vDestino == NULL)
            return false;

        removeNode(&vOrigem->data.arestas, destino);

        if (!directed)
        {
            removeNode(&vDestino->data.arestas, origem);
        }

        return true;
    }

    // Verifica se aresta existe
    bool existeAresta(int origem, int destino)
    {

        Node<VerticeInfo> *vOrigem = findNode(&listaVertices, origem);
        Node<VerticeInfo> *vDestino = findNode(&listaVertices, destino);

        if (vOrigem == NULL || vDestino == NULL)
            return false;

        return searchNode(&vOrigem->data.arestas, destino);
    }

    // Mostra a lista do grafo
    void imprimeGrafo()
    {
        cout << "Lista de Adjacencia:" << endl;

        Node<VerticeInfo> *vertice = listaVertices.head;

        while (vertice != NULL)
        {
            cout << vertice->id << ": ";

            Node<float> *aresta = vertice->data.arestas.head;

            while (aresta != NULL)
            {
                cout << aresta->id;

                if (pondered)
                {
                    cout << "(" << aresta->data << ")";
                }

                if (aresta->prox != NULL)
                {
                    cout << " -> ";
                }

                aresta = aresta->prox;
            }

            cout << endl;

            vertice = vertice->prox;
        }
    }

    // Retorna peso da aresta
    float pesoAresta(int origem, int destino)
    {
        Node<VerticeInfo> *vOrigem = findNode(&listaVertices, origem);
        Node<VerticeInfo> *vDestino = findNode(&listaVertices, destino);

        if (vOrigem == NULL || vDestino == NULL)
            return -1;

        Node<float> *aresta = findNode(&vOrigem->data.arestas, destino);

        if (aresta == NULL)
            return -1;
        return aresta->data;
    }

    // Retorna vizinhos de vértice
    vector<int> retornarVizinhos(int id)
    {
        Node<VerticeInfo> *vertice = findNode(&listaVertices, id);

        if (vertice == NULL)
            return {};

        Node<float> *aux = vertice->data.arestas.head;

        vector<int> vizinhos;

        while (aux != NULL)
        {
            vizinhos.push_back(aux->id);
            aux = aux->prox;
        }
        return vizinhos;
    }

    vector<int> listarVertices()
    {
        vector<int> vertices;
        Node<VerticeInfo> *aux = listaVertices.head;
        while (aux != NULL)
        {
            vertices.push_back(aux->id);
            aux = aux->prox;
        }
        return vertices;
    }
};

int contarCores(const vector<int> &cores)
{
    set<int> coresUtilizadas;

    for (int cor : cores)
    {
        if (cor > 0)
        {
            coresUtilizadas.insert(cor);
        }
    }

    return coresUtilizadas.size();
}

template <typename T>
void imprimirResultado(
    T &grafo,
    const ResultadoColoracao &resultado)
{
    cout << fixed << setprecision(3);

    cout << "Cores utilizadas: "
         << resultado.quantidadeCores << endl;

    cout << "Tempo de execucao: "
         << resultado.tempoMs << " ms" << endl;

    if (resultado.vertices.size() < 10)
    {
        cout << "\nColoracao dos vertices:\n";

        for (size_t i = 0; i < resultado.vertices.size(); i++)
        {
            int vertice = resultado.vertices[i];

            cout << grafo.labelVertice(vertice)
                 << " (ID " << vertice << ") -> Cor "
                 << resultado.cores[i] << endl;
        }
    }
}

template <typename T>
ResultadoColoracao heuristicaSimples(T &grafo)
{
    auto inicio = chrono::steady_clock::now();

    ResultadoColoracao resultado;
    resultado.vertices = grafo.listarVertices();

    // Começa todos os vértices sem cor.
    resultado.cores.resize(resultado.vertices.size(), 0);

    // Mapeia o ID real do vértice para sua posição no vetor.
    map<int, int> posicao;

    for (size_t i = 0; i < resultado.vertices.size(); i++)
    {
        posicao[resultado.vertices[i]] = i;
    }

    // Percorre os vértices na ordem em que foram listados.
    for (size_t i = 0; i < resultado.vertices.size(); i++)
    {
        int vertice = resultado.vertices[i];

        set<int> coresVizinhos;

        // Descobre as cores utilizadas pelos vizinhos já coloridos.
        vector<int> vizinhos = grafo.retornarVizinhos(vertice);

        for (int vizinho : vizinhos)
        {
            auto it = posicao.find(vizinho);

            if (it != posicao.end())
            {
                int cor = resultado.cores[it->second];

                if (cor > 0)
                {
                    coresVizinhos.insert(cor);
                }
            }
        }

        // Escolhe a menor cor disponível.
        int cor = 1;

        while (coresVizinhos.count(cor) > 0)
        {
            cor++;
        }

        resultado.cores[i] = cor;
    }

    resultado.quantidadeCores = contarCores(resultado.cores);

    auto fim = chrono::steady_clock::now();

    resultado.tempoMs =
        chrono::duration<double, milli>(fim - inicio).count();

    return resultado;
}

template <typename T>
ResultadoColoracao welshPowell(T &grafo)
{
    auto inicio = chrono::steady_clock::now();

    ResultadoColoracao resultado;
    resultado.vertices = grafo.listarVertices();

    // Ordena os vertices pelo grau, do maior para o menor.
    sort(resultado.vertices.begin(), resultado.vertices.end(),
         [&grafo](int a, int b)
         {
             return grafo.retornarVizinhos(a).size() >
                    grafo.retornarVizinhos(b).size();
         });

    resultado.cores.resize(resultado.vertices.size(), 0);

    map<int, int> posicao;
    for (size_t i = 0; i < resultado.vertices.size(); i++)
    {
        posicao[resultado.vertices[i]] = i;
    }

    // Colore os vertices na ordem definida.
    for (size_t i = 0; i < resultado.vertices.size(); i++)
    {
        int vertice = resultado.vertices[i];
        set<int> coresVizinhos;

        vector<int> vizinhos = grafo.retornarVizinhos(vertice);

        for (int vizinho : vizinhos)
        {
            auto it = posicao.find(vizinho);

            if (it != posicao.end())
            {
                int cor = resultado.cores[it->second];

                if (cor > 0)
                    coresVizinhos.insert(cor);
            }
        }

        int cor = 1;
        while (coresVizinhos.count(cor) > 0)
            cor++;

        resultado.cores[i] = cor;
    }

    resultado.quantidadeCores = contarCores(resultado.cores);

    auto fim = chrono::steady_clock::now();
    resultado.tempoMs =
        chrono::duration<double, milli>(fim - inicio).count();

    return resultado;
}

template <typename T>
ResultadoColoracao dsatur(T &grafo)
{
    auto inicio = chrono::steady_clock::now();

    ResultadoColoracao resultado;
    resultado.vertices = grafo.listarVertices();

    int n = resultado.vertices.size();
    resultado.cores.resize(n, 0);

    // Mapeia o ID do vertice para sua posicao nos vetores.
    map<int, int> posicao;

    for (int i = 0; i < n; i++)
    {
        posicao[resultado.vertices[i]] = i;
    }

    // Guarda os vizinhos e os graus de cada vertice.
    vector<vector<int>> vizinhos(n);
    vector<int> graus(n, 0);

    for (int i = 0; i < n; i++)
    {
        vizinhos[i] =
            grafo.retornarVizinhos(resultado.vertices[i]);

        graus[i] = vizinhos[i].size();
    }

    // Grafo vazio: nao ha vertices para colorir.
    if (n == 0)
    {
        resultado.quantidadeCores = 0;

        auto fim = chrono::steady_clock::now();
        resultado.tempoMs =
            chrono::duration<double, milli>(
                fim - inicio)
                .count();

        return resultado;
    }

    // Encontra o vertice de maior grau.
    int escolhido = 0;

    for (int i = 1; i < n; i++)
    {
        if (graus[i] > graus[escolhido])
        {
            escolhido = i;
        }
    }

    // Colore o vertice de maior grau com a primeira cor.
    resultado.cores[escolhido] = 1;
    int coloridos = 1;

    // Continua ate todos os vertices receberem uma cor.
    while (coloridos < n)
    {
        escolhido = -1;
        int maiorSaturacao = -1;
        int maiorGrau = -1;

        // Seleciona o vertice de maior saturacao.
        // Em caso de empate, escolhe o de maior grau.
        for (int i = 0; i < n; i++)
        {
            if (resultado.cores[i] != 0)
                continue;

            set<int> coresVizinhos;

            for (int vizinho : vizinhos[i])
            {
                auto it = posicao.find(vizinho);

                if (it != posicao.end())
                {
                    int cor = resultado.cores[it->second];

                    if (cor > 0)
                        coresVizinhos.insert(cor);
                }
            }

            int saturacao = coresVizinhos.size();

            if (saturacao > maiorSaturacao ||
                (saturacao == maiorSaturacao &&
                 graus[i] > maiorGrau))
            {
                escolhido = i;
                maiorSaturacao = saturacao;
                maiorGrau = graus[i];
            }
        }

        if (escolhido == -1)
            break;

        // Encontra a menor cor que nao aparece nos vizinhos.
        set<int> coresVizinhos;

        for (int vizinho : vizinhos[escolhido])
        {
            auto it = posicao.find(vizinho);

            if (it != posicao.end())
            {
                int cor = resultado.cores[it->second];

                if (cor > 0)
                    coresVizinhos.insert(cor);
            }
        }

        int cor = 1;

        while (coresVizinhos.count(cor) > 0)
        {
            cor++;
        }

        resultado.cores[escolhido] = cor;
        coloridos++;
    }

    resultado.quantidadeCores =
        contarCores(resultado.cores);

    auto fim = chrono::steady_clock::now();

    resultado.tempoMs =
        chrono::duration<double, milli>(
            fim - inicio)
            .count();

    return resultado;
}

bool tentarColorir(
    int indice,
    int quantidadeVertices,
    int k,
    const vector<vector<int>> &adjacencia,
    vector<int> &cores)
{
    // Todos os vertices receberam uma cor valida.
    if (indice == quantidadeVertices)
        return true;

    // Tenta cada cor disponivel.
    for (int cor = 1; cor <= k; cor++)
    {
        bool valida = true;

        // Verifica se algum vizinho ja usa essa cor.
        for (int vizinho : adjacencia[indice])
        {
            if (cores[vizinho] == cor)
            {
                valida = false;
                break;
            }
        }

        if (valida)
        {
            cores[indice] = cor;

            if (tentarColorir(
                    indice + 1,
                    quantidadeVertices,
                    k,
                    adjacencia,
                    cores))
            {
                return true;
            }

            // A combinacao nao funcionou: volta atras.
            cores[indice] = 0;
        }
    }

    return false;
}

template <typename T>
ResultadoColoracao forcaBruta(T &grafo)
{
    auto inicio = chrono::steady_clock::now();

    ResultadoColoracao resultado;
    resultado.vertices = grafo.listarVertices();

    int n = resultado.vertices.size();

    resultado.cores.resize(n, 0);

    // Grafo vazio.
    if (n == 0)
    {
        resultado.quantidadeCores = 0;

        auto fim = chrono::steady_clock::now();
        resultado.tempoMs =
            chrono::duration<double, milli>(fim - inicio).count();

        return resultado;
    }

    // Mapeia os IDs reais para posicoes de 0 a n-1.
    map<int, int> posicao;

    for (int i = 0; i < n; i++)
        posicao[resultado.vertices[i]] = i;

    // Converte a vizinhanca para indices internos.
    vector<vector<int>> adjacencia(n);

    for (int i = 0; i < n; i++)
    {
        vector<int> vizinhos =
            grafo.retornarVizinhos(resultado.vertices[i]);

        for (int vizinho : vizinhos)
        {
            auto it = posicao.find(vizinho);

            if (it != posicao.end())
                adjacencia[i].push_back(it->second);
        }
    }

    bool encontrou = false;

    int primeiraTentativa = 1;

    for (int k = primeiraTentativa; k <= n; k++)
    {
        fill(resultado.cores.begin(),
             resultado.cores.end(), 0);

        if (tentarColorir(
                0, n, k, adjacencia, resultado.cores))
        {
            encontrou = true;
            break;
        }
    }

    if (encontrou)
        resultado.quantidadeCores =
            contarCores(resultado.cores);

    auto fim = chrono::steady_clock::now();

    resultado.tempoMs =
        chrono::duration<double, milli>(fim - inicio).count();

    return resultado;
}

template <typename T>
T lerGrafo(string caminho)
{
    ifstream arquivo(caminho);
    if (!arquivo.is_open())
    {
        T g(false, false, 0);
        return g;
    }
    int V, A, D, P;
    arquivo >> V >> A >> D >> P;

    direc = (D == 1);
    ponderado = (P == 1);

    T g(direc, ponderado, V);

    int origem, destino = 0;

    while (arquivo >> origem >> destino)
    {
        float peso = 1;
        if (ponderado)
        {
            arquivo >> peso;
        }
        g.inserirAresta(origem, destino, peso);
    }

    return g;
}

template <typename T>
void bfs(T &grafo, int origem)
{
    queue<int> fila;
    set<int> visitados;

    fila.push(origem);
    visitados.insert(origem);

    while (!fila.empty())
    {
        int atual = fila.front();
        fila.pop();

        cout << atual << " ";

        vector<int> vizinhos = grafo.retornarVizinhos(atual);
        for (int viz : vizinhos)
        {
            if (!visitados.count(viz))
            {
                visitados.insert(viz);
                fila.push(viz);
            }
        }
    }
}

template <typename T>
void dfsAux(T &grafo, int atual, set<int> &visitados)
{
    visitados.insert(atual);
    cout << atual << " ";

    vector<int> vizinhos = grafo.retornarVizinhos(atual);
    for (int viz : vizinhos)
    {
        if (!visitados.count(viz))
        {
            dfsAux(grafo, viz, visitados);
        }
    }
}

template <typename T>
void dfs(T &grafo, int origem)
{
    set<int> visitados;
    dfsAux(grafo, origem, visitados);
}

void imprimirCaminho(int v, int origem, map<int, int> &anteriores)
{
    if (v == origem)
    {
        cout << v;
        return;
    }
    if (anteriores.count(v) == 0)
    {
        cout << "sem caminho";
        return;
    }
    imprimirCaminho(anteriores[v], origem, anteriores);
    cout << " -> " << v;
}

template <typename T>
void dijkstra(T &grafo, int origem)
{
    const float INFINITO = 1e9;
    map<int, float> distancias;
    map<int, int> anteriores;
    set<int> fechados;

    vector<int> vertices = grafo.listarVertices();
    for (int v : vertices)
    {
        distancias[v] = INFINITO;
    }
    distancias[origem] = 0;

    while (true)
    {
        int atual = -1;
        float menorDist = INFINITO;
        for (int v : vertices)
        {
            if (fechados.count(v) == 0 && distancias[v] < menorDist)
            {
                menorDist = distancias[v];
                atual = v;
            }
        }
        if (atual == -1)
            break;

        vector<int> vizinhos = grafo.retornarVizinhos(atual);
        for (int viz : vizinhos)
        {
            float novaDistancia = distancias[atual] + grafo.pesoAresta(atual, viz);
            if (distancias[viz] > novaDistancia)
            {
                distancias[viz] = novaDistancia;
                anteriores[viz] = atual;
            }
        }
        fechados.insert(atual);
    }
    for (int v : vertices)
    {
        cout << "Vertice " << v << " - distancia: ";
        if (distancias[v] >= INFINITO)
        {
            cout << "nao alcancavel" << endl;
        }
        else
        {
            cout << distancias[v] << " - caminho: ";
            imprimirCaminho(v, origem, anteriores);
            cout << endl;
        }
    }
}

template <typename T>
void menu(T &grafo)
{
    int opcao;
    do
    {
        cout << "\n===== MENU =====" << endl;
        cout << "1 - Inserir vertice" << endl;
        cout << "2 - Remover vertice" << endl;
        cout << "3 - Inserir aresta" << endl;
        cout << "4 - Remover aresta" << endl;
        cout << "5 - Verifica se aresta existe" << endl;
        cout << "6 - Peso de uma aresta" << endl;
        cout << "7 - Nome( label) de um vertice" << endl;
        cout << "8 - Vizinhos de um vertice" << endl;
        cout << "9 - Imprimir grafo" << endl;
        cout << "10 - BFS" << endl;
        cout << "11 - DFS" << endl;
        cout << "12 - Dijkstra" << endl;
        cout << "13 - Coloracao: Heuristica Simples" << endl;
        cout << "14 - Coloracao: Welsh-Powell" << endl;
        cout << "15 - Coloracao: DSATUR" << endl;
        cout << "16 - Coloracao: Forca Bruta" << endl;
        cout << "17 - Arvore Geradora Minima (Prim)" << endl;
        cout << "18 - Arvore Geradora Minima (Kruskal)" << endl;
        cout << "0 - Sair" << endl;
        cout << "Escolha: ";
        cin >> opcao;

        switch (opcao)
        {
        case 1:
        {
            string label;
            cout << "Label do vertice: ";
            cin >> label;
            grafo.inserirVertice(label);
            break;
        }
        case 2:
        {
            int id;
            cout << "Id do vertice a remover: ";
            cin >> id;
            grafo.removerVertice(id);
            break;
        }
        case 3:
        {
            int origem, destino;
            float peso = 1;
            cout << "Origem: ";
            cin >> origem;
            cout << "Destino: ";
            cin >> destino;
            if (grafo.isPondered())
            {
                cout << "Peso: ";
                cin >> peso;
            }
            grafo.inserirAresta(origem, destino, peso);
            break;
        }
        case 4:
        {
            int origem, destino;
            cout << "Origem: ";
            cin >> origem;
            cout << "Destino: ";
            cin >> destino;
            grafo.removerAresta(origem, destino);
            break;
        }
        case 5:
        {
            int origem, destino;
            cout << "Origem: ";
            cin >> origem;
            cout << "Destino: ";
            cin >> destino;
            cout << "Existe aresta? " << (grafo.existeAresta(origem, destino) ? "sim" : "nao") << endl;
            break;
        }
        case 6:
        {
            int origem, destino;
            cout << "Origem: ";
            cin >> origem;
            cout << "Destino: ";
            cin >> destino;
            cout << "Peso: " << grafo.pesoAresta(origem, destino) << endl;
            break;
        }
        case 7:
        {
            int id;
            cout << "Id do vertice: ";
            cin >> id;
            cout << "Label: " << grafo.labelVertice(id) << endl;
            break;
        }
        case 8:
        {
            int id;
            cout << "Id do vertice: ";
            cin >> id;
            vector<int> vizinhos = grafo.retornarVizinhos(id);
            cout << "Vizinhos: ";
            for (int v : vizinhos)
            {
                cout << v << " ";
            }
            cout << endl;
            break;
        }
        case 9:
        {
            system("cls");
            grafo.imprimeGrafo();
            cout << endl;
            break;
        }
        case 10:
        {
            int origem;
            cout << "Vertice de origem: ";
            cin >> origem;
            cout << "BFS: ";
            bfs(grafo, origem);
            cout << endl;
            break;
        }
        case 11:
        {
            int origem;
            cout << "Vertice de origem: ";
            cin >> origem;
            cout << "DFS: ";
            dfs(grafo, origem);
            cout << endl;
            break;
        }
        case 12:
        {
            int origem;
            cout << "Vertice de origem: ";
            cin >> origem;
            dijkstra(grafo, origem);
            break;
        }
        case 13:
        {
            if (grafo.isDirected())
            {
                cout << "A coloracao exige um grafo nao direcionado."
                     << endl;
                break;
            }

            ResultadoColoracao resultado = heuristicaSimples(grafo);

            cout << "\n=== Heuristica Simples ===" << endl;
            imprimirResultado(grafo, resultado);

            break;
        }
        case 14:
        {
            if (grafo.isDirected())
            {
                cout << "A coloracao exige um grafo nao direcionado."
                     << endl;
                break;
            }

            ResultadoColoracao resultado = welshPowell(grafo);

            cout << "\n=== Welsh-Powell ===" << endl;
            imprimirResultado(grafo, resultado);

            break;
        }
        case 15:
        {
            if (grafo.isDirected())
            {
                cout << "A coloracao exige um grafo nao direcionado."
                     << endl;
                break;
            }

            ResultadoColoracao resultado = dsatur(grafo);

            cout << "\n=== DSATUR ===" << endl;
            imprimirResultado(grafo, resultado);

            break;
        }
        case 16:
        {
            if (grafo.isDirected())
            {
                cout << "A coloracao exige um grafo nao direcionado."
                     << endl;
                break;
            }

            ResultadoColoracao resultado = forcaBruta(grafo);

            cout << "\n=== Forca Bruta ===" << endl;
            imprimirResultado(grafo, resultado);

            break;
        }

        case 17:
        {
            if (grafo.isDirected())
            {
                cout << "O algoritmo Prim exige um grafo nao direcionado.\n";
                break;
            }

            if (!grafo.isPondered())
            {
                cout << "O algoritmo Prim exige um grafo ponderado.\n";
                break;
            }

            ResultadoAGM resultado = prim(grafo);
            imprimirResultadoAGM(resultado);
            break;
        }
        case 18:
        {
            if (grafo.isDirected())
            {
                cout << "O algoritmo Kruskal exige um grafo nao direcionado.\n";
                break;
            }

            if (!grafo.isPondered())
            {
                cout << "O algoritmo Kruskal exige um grafo ponderado.\n";
                break;
            }

            ResultadoAGM resultado = kruskal(grafo);
            imprimirResultadoAGM(resultado);
            break;
        }
        case 0:
        {
            cout << "Saindo..." << endl;
            break;
        }
        default:
            cout << "Opcao invalida" << endl;
        }

    } while (opcao != 0);
}