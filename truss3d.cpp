#include "truss3d.h"

#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <cmath>

#include <QString>

#include <mth/matrix.h>

Truss3D::Truss3D(char *filename)
{
    this->isSolved = false;

    std::ifstream file(filename, std::ios::in);
    if(file.fail()) std::cerr<<"Error in file loading: "<<filename;
    int index;

    // Dados de condições de apoio
    file>>nre;
    restrictions= new bool *[nre];

    for(int i=0; i<nre; i++)
    {
        restrictions[i] = new bool[3];
        file>>index>>restrictions[i][0]>>restrictions[i][1]>>restrictions[i][2];
    }

    // Dados de carregamento preescrito
    file>>nlo;
    loading = new double *[nlo];

    for(int i=0; i<nlo; i++)
    {
        loading[i] = new double[3];
        file>>index>>loading[i][0]>>loading[i][1]>>loading[i][2];
    }

    // Dados de deslocamento preescrito
    file>>ndi;
    displacements = new double *[ndi];

    for(int i=0; i<ndi; i++)
    {
        displacements[i] = new double[3];
        file>>index>>displacements[i][0]>>displacements[i][1]>>displacements[i][2];
    }

    // Dados dos nos
    file>>nNodes;
    this->nodes = new Node3D *[nNodes];

    double *tempCoord = new double[3];
    int til, tid, tir;

    for(int i=0; i<nNodes; i++)
    {
        file>>index>>tempCoord[0]>>tempCoord[1]>>tempCoord[2]>>tir>>til>>tid;
        nodes[i] = new Node3D(index, tempCoord, restrictions[tir], loading[til], displacements[tid]);
    }

    // Dados de material
    file>>nma;
    materials = new Material *[nma];
    std::string tname;
    double te, ta;

    for(int i=0; i<nma; i++)
    {
        file>>index>>tname>>te>>ta;
        materials[i] = new Material(tname, te, ta);
    }

    // Dados dos Elementos
    file>>nElements;
    this->elements = new Truss3DElement *[nElements];

    int index_el, index_no1, index_no2;
    int itm;

    for(int i=0; i<nElements; i++)
    {
        file>>index_el>>index_no1>>index_no2>>itm;
        elements[i] = new Truss3DElement(index_el, nodes[index_no1], nodes[index_no2], materials[itm]); // cria o elementos
    }
}


Truss3D::Truss3D(DXFReader *file)
{
    // set unsolved model
    this->isSolved = false;



    // Dados de condições de apoio
    nre = 1+1;
    restrictions= new bool *[nre];
    restrictions[0] = new bool[3];
    restrictions[1] = new bool[3];

    restrictions[0][0]=restrictions[0][1]=restrictions[0][2] = false;

    // Dados de carregamento preescrito
    nlo = 1+1;
    loading = new double *[nlo];

    loading[0] = new double[3];
    loading[1] = new double[3];

    loading[0][0]=loading[0][1]=loading[0][2] = 0.0;


    // Dados de deslocamento preescrito
    ndi = 1;
    displacements = new double *[ndi];
    displacements[0] = new double[3];

    displacements[0][0]=displacements[0][1]=displacements[0][2] = 0.0;

    // Dados dos nos
    nNodes = file->pbuffer->count;
    this->nodes = new Node3D *[nNodes];

    double *tempCoord = new double[3];

    for(int i=0; i<nNodes; i++)
    {
        tempCoord[0] = file->pbuffer->data[3*i];
        tempCoord[1] = file->pbuffer->data[3*i+1];
        tempCoord[2] = file->pbuffer->data[3*i+2];

        nodes[i] = new Node3D(i, tempCoord, restrictions[0], loading[0], displacements[0]);
    }

    // Dados de material
    nma = 1;
    materials = new Material *[nma];

    materials[0] = new Material("unamed", 1.0, 1.0);

    // Dados dos Elementos
    nElements = file->lbuffer->count;
    this->elements = new Truss3DElement *[nElements];

    int index_no1, index_no2;

    for(int i=0; i<nElements; i++)
    {
        index_no1 = file->lbuffer->data[3*i];
        index_no2 = file->lbuffer->data[3*i+1];
        elements[i] = new Truss3DElement(i, nodes[index_no1], nodes[index_no2], materials[0]); // cria o elementos
    }

}



Truss3D::Truss3D()
{
    nNodes = 0;
    nElements = 0;
    this->isSolved = false;
}




void Truss3D::evalStiffnessMatrix(void)
{
    k.resize(3*nNodes, 3*nNodes);
    k = 0.0;

    f.resize(3*nNodes);
    f = 0.0;

    Mth::Matrix ke(6,6);

    Node3D **ptrNodes = new Node3D*[2];

    // Calcula a matriz de rigidez global
    for(int i=0; i<nElements; i++)
    {
        // Calcula a matriz de rigidez local
        elements[i]->getStiffnessMatrix(ke);

        ptrNodes[0] = elements[i]->node1;
        ptrNodes[1] = elements[i]->node2;

        // Posiciona a matriz de rigidez local do elemento i na matriz de rigidez global
        for(int ni=0;ni<2;ni++)
            for(int nj=0;nj<2;nj++)
                for(int ii=0;ii<3;ii++)
                    for(int ij=0;ij<3;ij++)
                    {
                        int indexI = 3*ptrNodes[ni]->index+ii;
                        int indexJ = 3*ptrNodes[nj]->index+ij;
                        k(indexI, indexJ) += ke(3*ni+ii, 3*nj+ij);
                    }

    }

    for(int i=0; i<nNodes; i++)
    {
        f(3*nodes[i]->index) = nodes[i]->loading[0];
        f(3*nodes[i]->index+1) = nodes[i]->loading[1];
        f(3*nodes[i]->index+2) = nodes[i]->loading[2];
    }

    delete [] ptrNodes;

}



void Truss3D::solve(void)
{

    this->isSolved = true;

    std::ofstream flog("log_solver.txt");


    Mth::Matrix kc(k); // cópias
    Mth::Vector fc(f);

    // Aplica as condicoes de contorno
    for(int i=0; i<nNodes; i++)
        for(int j=0; j<3; j++)
            if(nodes[i]->restrictions[j]==true)
            {
                int n = 3*nodes[i]->index+j;
                for(int t=0; t<3*nNodes; t++)
                    kc(n, t) = 0.0;
                kc(n, n) = 1.0;
                fc(n) = nodes[i]->displacements[j];
            }

    for(int i=0; i<3*nNodes; i++)
    {
        bool check = true;
        for(int j=0; j<3*nNodes; j++)
            if(fabs(kc(i, j))>1.e-10) {check = false ; break;}
        if(check)
        {
            int t = i/3;
            int v = i-3*t;
            std::cerr<<"\n error in line: "<<i<<" node: "<<t<<" v: "<<v;
            //            for(int t=0; t<3*nNodes; t++)
            //                kc(i, t) = kc(t, i) = 0.0;
            //            kc(i, i) = 1.0;
            //            fc(i) = 0.0;

        }
    }

    for(int i=0; i<3*nNodes; i++)
    {
        bool check = true;
        for(int j=0; j<3*nNodes; j++)
            if(fabs(kc(j, i))>1.e-10) {check = false ; break;}
        if(check) std::cerr<<"\n error in column: "<<i;
    }




    //    std::cerr<<"\n\n Matriz de rigidez\n";
    //    std::cerr<<kc;

    //    std::cerr<<"\n\n Vetor de carga\n";
    //    std::cerr<<fc;

    //    Mth::Matrix kcc(kc);
    //    Mth::Vector fcc(fc);

    //    std::cerr<<"\n\n Teste\n";
    //    kcc.inverse();
    //    std::cerr<<kcc*fcc;

    // Aloca vetor para resultados
    u.resize(3*nNodes);

    kc.solve(fc, u);


    u.clear();

    flog<<"\n\n Solução\n";
    flog<<u;


    //    std::cerr<<"\n\n Matriz de rigidez\n";
    //    std::cerr<<k;
    reactions.resize(3*nNodes);

    reactions = k*u;

    reactions.clear(1.e-10);

    flog<<"\n\n Reações\n";
    flog<<reactions;


    Mth::Matrix ue(6);


    stress.resize(nElements);
    for(int i=0; i<nElements; i++)
    {
        ue(0) = u(3*elements[i]->node1->index);
        ue(1) = u(3*elements[i]->node1->index+1);
        ue(2) = u(3*elements[i]->node1->index+2);
        ue(3) = u(3*elements[i]->node2->index);
        ue(4) = u(3*elements[i]->node2->index+1);
        ue(5) = u(3*elements[i]->node2->index+2);

        stress(i) = elements[i]->getStress(ue);
    }

    //    stress.clear();
    flog<<"\n\n Tensoes normais\n";
    flog<<stress;

    flog.close();

}


void Truss3D::stresslimits(double &min, double &max)
{
    min = stress(0);
    max = stress(0);

    for(int i=1; i<stress.n; i++)
    {
        if(stress(i)>max) max = stress(i);
        if(stress(i)<min) min = stress(i);
    }

}


Truss3D::~Truss3D()
{
    //        for(int i=0; i<nNodes; i++)
    //            delete nodes[i];
    //        delete [] nodes;

    //        for(int i=0; i<nElements; i++)
    //            delete elements[i];
    //        delete [] elements;


    //    delete [] this->stress;
    //    delete [] this->reactiveForce;
    //    delete [] this->displacement;

}



