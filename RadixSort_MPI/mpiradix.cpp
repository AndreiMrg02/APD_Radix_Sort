#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Functie de sortare a bucăților de date
void radix_sort(vector<int>& arr) {
    const int base = 10; // Baza de numărare
    int max_val = *max_element(arr.begin(), arr.end());
    for (int exp = 1; max_val / exp > 0; exp *= base) {
        vector<int> count(base, 0); // Contorizează frecvența cifrelor
        vector<int> output(arr.size(), 0); // Vector pentru elementele sortate
        // Contorizează frecvența cifrelor
        for (int i = 0; i < arr.size(); i++) {
            count[(arr[i] / exp) % base]++;
        }
        // Calculează pozițiile relative ale elementelor în vectorul sortat
        for (int i = 1; i < count.size(); i++) {
            count[i] += count[i - 1];
        }
        // Plasează elementele în vectorul sortat
        for (int i = arr.size() - 1; i >= 0; i--) {
            output[count[(arr[i] / exp) % base] - 1] = arr[i];
            count[(arr[i] / exp) % base]--;
        }
        // Copiază vectorul sortat în vectorul original
        for (int i = 0; i < arr.size(); i++) {
            arr[i] = output[i];
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Vectorul de sortat
    vector<int> arr = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5 };

    // Calculează dimensiunea bucăților
    int block_size = arr.size() / size;

    // Distribuie bucățile de date către procese
    vector<int> block(block_size);
    MPI_Scatter(arr.data(), block_size, MPI_INT, block.data(), block_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Sortează bucățile
    radix_sort(block);

    // Combinați bucățile de date sortate
    vector<int> sorted(arr.size());
    MPI_Gather(block.data(), block_size, MPI_INT, sorted.data(), block_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Procesul 0 afișează vectorul sortat
    if (rank == 0) {
        for (int i = 0; i < sorted.size(); i++) {
            cout << sorted[i] << " ";
        }
        cout << endl;
    }
    MPI_Finalize();
    return 0;
}
