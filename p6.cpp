#include <iostream>
#include <fstream>
#include <omp.h>

using namespace std;

string words[10] = {"the", "around", "grAphics", "from", "by", "be", "any", "mount", "HeLlo", "word"};
int count[10];

bool compare(string a, string b)
{
    if (a.length()==b.length())
        {
            for(int i = 0; i < a.length(); i++)
                {
                    if(tolower(a[i]) != tolower(b[i]))
                        return false;
                }
            return true;
        }
    return false;
}

int word_count(string file_name, string word) {
    int count = 0;
    string str;
    ifstream input_file(file_name.c_str());

    while (input_file) {
        input_file >> str;
        if (compare(str, word))
            count++;
    }
    input_file.close();
    return count;
}

int main() {

    string file_name;
    printf("Enter file name : ");
    cin >> file_name;
    
    for(int num_threads = 1; num_threads <= 8; num_threads = num_threads*2)
    {
        double t1 = omp_get_wtime();

        #pragma omp parallel for num_threads(num_threads)
        for (int i=0; i<10; i++)
            count[i] = word_count(file_name, words[i]);

        double t2 = omp_get_wtime();

        cout << num_threads << " Threads Time : " << (t2-t1) * 1000 << "ms\n";
    }
    cout<<"\n";
    for (int i=0; i<10; i++)
            cout << words[i] << " : " << count[i] << "\n";
}
