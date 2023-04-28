#include <algorithm>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <random>
#include <vector>
#include <unordered_set>
#include <Windows.h>
#include <psapi.h>
#include <mpi.h>
#include <iostream>
#pragma comment(lib, "psapi.lib")

using namespace std;
using namespace chrono;


void radix_sort(vector<int>& arr) {
    int max_val = *max_element(arr.begin(), arr.end());
    vector<int> output(arr.size());
    vector<int> count(10);

    bool more_digits = true;
    for (int exp = 1; more_digits; exp *= 10) {
        fill(count.begin(), count.end(), 0);

        more_digits = false;
        for (int i = 0; i < arr.size(); i++) {
            int digit = (arr[i] / exp) % 10;
            count[digit]++;
            if (arr[i] >= exp * 10) {
                more_digits = true;
            }
        }

        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }

        for (int i = arr.size() - 1; i >= 0; i--) {
            int digit = (arr[i] / exp) % 10;
            output[count[digit] - 1] = arr[i];
            count[digit]--;
        }

        arr = output;
    }

    // Eliberam memoria alocata dinamic
    output.clear();
    vector<int>().swap(output);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const int n = 10;

    // Distribuim datele catre toate procesele
    vector<int> arr;
    if (world_rank == 0) {

        ifstream fin("inputs/input1.txt");
        arr.reserve(n);
        int num;
        while (fin >> num) {
            arr.push_back(num);
        }
        fin.close();
    }
    vector<int> local_arr(n / world_size);
    MPI_Scatter(arr.data(), n / world_size, MPI_INT, local_arr.data(), n / world_size, MPI_INT, 0, MPI_COMM_WORLD);

    auto start_time = high_resolution_clock::now();
    // Sortam vectorii locali folosind algoritmul radix sort
    radix_sort(local_arr);

    // Colectam datele sortate de la toate procesele
    vector<int> sorted_arr(n);
    MPI_Allgather(local_arr.data(), n / world_size, MPI_INT, sorted_arr.data(), n / world_size, MPI_INT, MPI_COMM_WORLD);

    if (world_rank == 0) {

        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end_time - start_time);
        double total_time = double(duration.count()) / 1000000;

        ofstream foutresult("outputs/output10.out");
        for (int i = 0; i < n; i++) {
            foutresult << sorted_arr[i] << endl;
            cout << sorted_arr[i] << endl;
        }
        foutresult.close();

        cout << "Timpul total de executie al functiei de sortare: " << fixed << setprecision(6) << total_time << " secunde" << endl;
    }

    MPI_Finalize();

    return 0;
}
