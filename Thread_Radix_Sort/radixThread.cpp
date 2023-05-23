#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <random>
#include <vector>
#include <unordered_set>
#include <iostream>

using namespace std;
using namespace chrono;


void radixSort(vector<int>& arr, int exp)
{
    const int size = arr.size();
    const int max = 10; // Baza 10

    vector<int> output(size, 0);
    vector<int> count(max, 0);


    for (int i = 0; i < size; i++)
        count[(arr[i] / exp) % 10]++;


    for (int i = 1; i < max; i++)
        count[i] += count[i - 1];


    for (int i = size - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }


    for (int i = 0; i < size; i++)
        arr[i] = output[i];
}

void radixSortThread(vector<int>& arr, int numThreads)
{
    vector<thread> threads(numThreads);
    vector<vector<int>> partialResults(numThreads);

    const int size = arr.size();
    int max = *max_element(arr.begin(), arr.end());


    for (int exp = 1; max / exp > 0; exp *= 10) {

        const int chunkSize = size / numThreads;
        for (int i = 0; i < numThreads; i++) {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? size : (start + chunkSize);
            vector<int> chunk(arr.begin() + start, arr.begin() + end);
            partialResults[i] = move(chunk);
        }

        for (int i = 0; i < numThreads; i++) {
            threads[i] = thread(radixSort, ref(partialResults[i]), exp);
        }


        for (int i = 0; i < numThreads; i++) {
            threads[i].join();
        }

        int index = 0;
        for (int i = 0; i < numThreads; i++) {
            for (int j = 0; j < partialResults[i].size(); j++) {
                arr[index++] = partialResults[i][j];
            }
        }
    }
}

int main() {
    const int n = 10000000;

    ifstream fin("inputs/input10.txt");
    vector<int> arr;
    arr.reserve(n);
    int num;
    while (fin >> num) {
        arr.push_back(num);
    }
    fin.close();

    auto start_time = high_resolution_clock::now();
    radixSortThread(arr,10);
    auto end_time = high_resolution_clock::now();


    radixSortThread(arr, 1);
    ofstream foutresult("outputs/output10.out");
    for (int i = 0; i < arr.size(); i++) {
        foutresult << arr[i] << endl;
    }
    foutresult.close();

    auto duration = duration_cast<microseconds>(end_time - start_time);
    double total_time = double(duration.count()) / 1000000;

    cout << "Execution time: " << fixed << setprecision(6) << total_time << " seconds\n";

    return 0;
}
