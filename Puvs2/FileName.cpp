#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Vertausche Zahlen an Positionen i und j im Array v

void swap(float* v, int i, int j)
{
    float t = v[i];
    v[i] = v[j];
    v[j] = t;
}

// ---------------------------------------------------------------------------
// Serielle Version von Quicksort (Wirth) 
void quicksort(float* v, int start, int end) //ohne paraleliesierung
{
    int i = start, j = end;
    float pivot;

    if (start >= end) return;

    pivot = v[(start + end) / 2];         // mittleres Element (=Pivotelement)

    do {
        while (v[i] < pivot) i++;
        while (pivot < v[j]) j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j); i++; j--;
        }
    } while (i <= j);

    // Teile und herrsche


    quicksort(v, start, j); // rekursiver Aufruf fuer linke Teilliste


    quicksort(v, i, end);// rekursiver Aufruf fuer rechte Teilliste

}
int compare_arrays(float* a, float* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            printf("Unterschied bei Index %d: a[%d] = %f, b[%d] = %f\n", i, i, a[i], i, b[i]);
            return 0; // Arrays sind unterschiedlich
        }
    }
    printf(" arrays sind gleich");
    return 1; // Arrays sind gleich
}
void quicksortv(float* v, int start, int end) //mit parraleliessierung
{
    int i = start, j = end;
    float pivot;

    if (start >= end) return;

    pivot = v[(start + end) / 2];         // mittleres Element (=Pivotelement)

    do {
        while (v[i] < pivot) i++;
        while (pivot < v[j]) j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j); i++; j--;
        }
    } while (i <= j);

    // Teile und herrsche

#pragma omp task  if((end - start) > 1000)
    quicksort(v, start, j); // rekursiver Aufruf fuer linke Teilliste

#pragma omp task  if((end - start) > 1000)
    quicksort(v, i, end);// rekursiver Aufruf fuer rechte Teilliste
#pragma omp taskwait
}



// ---------------------------------------------------------------------------
// Hauptprogramm

int main(int argc, char* argv[])
{
    double start, end;
    start = omp_get_wtime();
    float* v;      // Pointer (auf Startposition des Arrays) 
    float* testv; // pointer (auf startposition des refferenz arrays)
    int num_elements;                                        //  Elementanzahl 


    if (argc != 2) {                                      // Konsolenanweisung
        num_elements = 1000000; // eine Null mehr
    }
    else {
        num_elements = atoi(argv[1]);
    }

    v = (float*)calloc(num_elements, sizeof(float));  // Speicher reservieren
    for (int j = 0; j < num_elements; j++) {
        v[j] = (float)rand();                  // initialisiere mit Zufallszahl
    }
    testv = (float*)calloc(num_elements, sizeof(float));
    if (testv == NULL) {
        printf("Speicher konnte nicht für die Kopie reserviert werden.\n");
        free(v); // Ursprünglichen Speicher freigeben
        return -1;
    }
    memcpy(testv, v, num_elements * sizeof(float)); // kopiert inhalt von v nach test v
    

    omp_set_num_threads(12);
#pragma omp parallel
    {
#pragma omp single 
        quicksortv(v, 0, num_elements - 1);  // Aufruf Sortieralgorithmus mit parraleliesierung
    }
   
    
    printf("\n Done sorting all %d numbers.\n", num_elements);
    end = omp_get_wtime();
    printf("Benötigte Zeit: %f Sekunden \n", end - start);
    
    quicksort(testv, 0, num_elements - 1); // Aufruf des originalen Quicksort ohne paraleliesierung mit vergleichsarray

    compare_arrays(v, testv, num_elements); // vergleicht die arrays nach sortierung 

    
    return 0;
}