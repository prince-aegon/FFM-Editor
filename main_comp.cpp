#include <fstream>
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

int main()
{
    fstream file1;
    fstream file2;
    string word1, word2, t, q, filename1, filename2;

    // filename of the file
    filename1 = "encode.txt";
    filename2 = "encode1.txt";

    // opening file
    file1.open(filename1.c_str());
    file2.open(filename2.c_str());

    // extracting words from the file
    int counter = 0;
    int i = 0;
    auto start = high_resolution_clock::now();
    while (file1 >> word1 && file2 >> word2)
    {
        if (word1 != word2)
        {
            // cout << i << " " << word1 << " " << word2 << endl;
            counter++;
        }
        i++;
    }
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    cout << counter << endl;
}