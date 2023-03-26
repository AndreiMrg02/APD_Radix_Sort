#include <iostream>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <random>
#include <vector>
#include <unordered_set>

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


int main() {
    const int n = 100;

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(1, 5000000);

    ofstream fout("numbers.txt");
    unordered_set<int> values;
    while (values.size() < n) {
        int num = distribution(generator);
        values.insert(num);
        fout << num << " ";
    }
    fout.close();

    ifstream fin("numbers.txt");
    vector<int> arr;
    arr.reserve(n);
    int num;
    while (fin >> num) {
        arr.push_back(num);
    }
    fin.close();

    auto start_time = high_resolution_clock::now();
    radix_sort(arr);
    auto end_time = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end_time - start_time);
    double total_time = double(duration.count()) / 1000000;

    ofstream foutresult("out.out");
    for (int i = 0; i < n; i++) {
        foutresult << arr[i] << endl;
    }
    foutresult.close();

    cout << "Timpul total de executie al functiei de sortare: " << fixed << setprecision(6) << total_time << " secunde" << endl;

    return 0;
}
