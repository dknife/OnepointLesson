//
//  main.cpp
//  GeneticEquation
//
//  Created by young-min kang on 9/24/15.
//  Copyright (c) 2015 young-min kang. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <iostream>

#define NUMGENES 1024
#define NUMGENERATIONS 200
#define GENELENGTH 32


class Gene {
    // sign of mantissa: 1
    // mantissa: 23
    // sign of exponent: 1
    // exponent: 7
public:
    bool gene[GENELENGTH];
    void randomCreation() {
        for(int i=0;i<GENELENGTH;i++) {
            gene[i] = rand()%2;
        }
    }
    float bitsToNumber(int s, int e) {
        float val = 0.0;
        for(int i=s;i<=e;i++) {
            val *= 2.0;
            val += gene[i]?1:0;
        }
        return val;
    }
    float getPhenoType() {
        bool signOfMantissa = gene[0];
        bool signOfExponent = gene[24];
        float Mantissa = bitsToNumber( 1,23) * (signOfMantissa?-1.0:1.0);
        float Exponent = bitsToNumber(25,31) * (signOfExponent?-1.0:1.0);
        return Mantissa * pow(2, Exponent);
    }
};

float phenoType[NUMGENES];
Gene  genoType[NUMGENES];
float fit[NUMGENES];


float fitness(float x) {
    float x2 = x*x;
    float x3 = x2*x;
    float x4 = x3*x;
    float x5 = x4*x;
    float fit = x5 + x4 + 7.0*x3 + x2 + x - 18.0;
 
    fit *= fit;
    if ( fit == INFINITY || fit == NAN) {
        fit = 1000000.0;
    }
    return fit;
}

void swapGene(int i, int j) {
    float temp = phenoType[i];
    phenoType[i] = phenoType[j];
    phenoType[j] = temp;
    temp = fit[i];
    fit[i] = fit[j];
    fit[j] = temp;
    for(int idx=0;idx<GENELENGTH;idx++){
        bool t = genoType[i].gene[idx];
        genoType[i].gene[idx] = genoType[j].gene[idx];
        genoType[j].gene[idx] = t;
    }
}




void initializeGenes() {
    for(int i=0;i<NUMGENES;i++) {
        genoType[i].randomCreation();
        phenoType[i] = genoType[i].getPhenoType();
    }
}

void computeFitness() {
    for(int i=0;i<NUMGENES;i++) {
        fit[i] = fitness(phenoType[i]);
    }
}

void printBestGenes() {
    int best = 0; float bestFit = fit[0];
    for(int i=1;i<NUMGENES;i++) {
        if (fit[i]<bestFit) {
            best = i; bestFit = fit[i];
        }
    }
    swapGene(0, best);
    printf("Best Gene = %f (%f)\n", phenoType[0], fit[0]);
}

void nextGeneration() {
    static int generation = 0;
    printf("%d: ", generation++);
    // genoType[0] is replicated
    
    // crossover
    for(int i=1; i<NUMGENES; i++) {
        int idx1 = rand()%NUMGENES;
        int idx2 = rand()%NUMGENES;
        int idx3 = rand()%NUMGENES;
        int idx4 = rand()%NUMGENES;
        if (fit[idx1]>fit[idx3]) {
            swapGene(idx1, idx3);
        }
        if (fit[idx2]>fit[idx4]) {
            swapGene(idx2, idx4);
        }
        if (fit[idx1]>fit[idx2]) {
            swapGene(idx1, idx2);
        }
        int point1 = rand()%GENELENGTH;
        int point2 = rand()%GENELENGTH;
        if(point2<point1) { float t=point1; point1=point2; point2=t; }
        for(int i=point1; i<=point2; i++) {
            genoType[idx2].gene[i] = genoType[idx1].gene[i];
            
        }
        phenoType[idx2] = genoType[idx2].getPhenoType();
        
    }
    
    // mutation
    for (int i=0; i<NUMGENES/10; i++) {
        int idx = rand()%(NUMGENES-1);
        for(int j=0; j<GENELENGTH/10; j++) {
            int idx2 = 1+rand()%(GENELENGTH-1);
            genoType[idx].gene[idx2] = rand()%2;
        }
        phenoType[idx] = genoType[idx].getPhenoType();
    }
    
    
}

int main(int argc, const char * argv[]) {
    
    initializeGenes();
    for(int i=0;i<NUMGENERATIONS;i++) {
        computeFitness();
        printBestGenes();
        nextGeneration();
    }
}
