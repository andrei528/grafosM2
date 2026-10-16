#include <iostream>
#include "LDE.hpp"
#include "grafos.hpp"

using namespace std;

int main(){
    int tipoGrafo;
    cout << "1 - Matriz\n2 - Lista\nEscolha: ";
    cin >> tipoGrafo;

    int origemDados;
    cout << "1 - Carregar de arquivo\n2 - Criar vazio\nEscolha: ";
    cin >> origemDados;

    if(tipoGrafo == 1){
        if(origemDados == 1){
            string caminho;
            cout << "Caminho: "; cin >> caminho;

            GrafoMatriz gm = lerGrafo<GrafoMatriz>(caminho);
            menu(gm);
        }
        else{
            char direcionado, ponderado;
            bool direc, pond;
            int numVertices;
            cout << "\nDirecionado? (s/n) "; cin >> direcionado;
            cout << "\nPonderado? (s/n) "; cin >> ponderado;
            cout << "\nNumero de vertices: "; cin >> numVertices;

            direc = (direcionado == 's' || direcionado == 'S');
            pond = (ponderado == 's' || ponderado == 'S');

            GrafoMatriz gm(direc, pond, numVertices);
            menu(gm);
        }
    }
    else{
        if(origemDados == 1){
            string caminho;
            cout << "Caminho: "; cin >> caminho;

            GrafoLista gl = lerGrafo<GrafoLista>(caminho);
            menu(gl);
        }
        else{
            char direcionado, ponderado;
            bool direc, pond;
            int numVertices;
            cout << "\nDirecionado? (s/n) "; cin >> direcionado;
            cout << "\nPonderado? (s/n) "; cin >> ponderado;
            cout << "\nNumero de vertices: "; cin >> numVertices;

            direc = (direcionado == 's' || direcionado == 'S');
            pond = (ponderado == 's' || ponderado == 'S');

            GrafoLista gl(direc, pond, numVertices);
            menu(gl);
        }
    }

    return 0;
}