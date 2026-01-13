#include<iostream>
#include<string>
#include<vector>
#include<random>
#include<fstream>

struct Record
{
    int age;
    std :: string name;
};


std :: vector<std :: string> name_pool = {"Hemant", "Sonu", "Pranav", "Sagar", "Ravi",
                                          "Aniket", "Rahul", "Mohit", "Iradri", "Kartik",
                                          "Ashwin", "Sahil", "Aditya", "Gautam", "Anurag"};

static std :: random_device rd;
static std :: mt19937 gen(rd());

std :: string getRandomName(){
    //
    std :: uniform_int_distribution<std :: size_t> dist(0, name_pool.size() -1);
    return name_pool[dist(gen)];
    //
    //return name_pool[rand()%name_pool.size()];
}

/**
 * 
 */

int getRandomAge(){
    // static std :: random_device rd;
    // static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(15, 40);
    return dist(gen);
}

/**
 * 
 */

void generateRecords(){
    
    for(int n = 10; n < 101; n += 10){
        for(int dn = 1; dn < 11; ++dn){
            
            std :: string filename = "datasets/IS_HEMANTKUMAR_n" + std :: to_string(n) + "_dataset" + std :: to_string(dn) + ".txt";
            std :: ofstream outputFile(filename);

            for(int i = 0; i < n; i++){
                outputFile << getRandomName() << " " << getRandomAge() << "\n";
            }
        }
    }
}

/**
 * 
 */
/*
void insertionSort(std:: vector<int> &V){
    for(int i =1; i < V.size(); i++){
        int key = V[i];
        int j = i - 1;
        while(j >= 0 && V[j] > key){
            V[j+1] = V[j];
            j = j - 1;
        }
        V[j + 1] = key;
    }
}
*/

int insertionSortAge(std::vector<Record> &v){
    int comp = 0;
    for(int i = 1; i < v.size(); i++){
        Record key = v[i];
        int j = i - 1;
        while(j >= 0){
            comp++;
            if(v[j].age > key.age){
                v[j+1] = v[j];
                j--;
            } else break;
        }
        v[j+1] = key;
    }
    return comp;
}

int insertionSortName(std::vector<Record> &v){
    int comp = 0;
    for(int i = 1; i < v.size(); i++){
        Record key = v[i];
        int j = i - 1;
        while(j >= 0){
            comp++;
            if(v[j].name > key.name){
                v[j+1] = v[j];
                j--;
            } else break;
        }
        v[j+1] = key;
    }
    return comp;
}


int insertionSortNameAge(std::vector<Record> &v){
    int tcomp = 0;

    // First pass sort by name (stable)
    tcomp += insertionSortName(v);

    // Second pass stable: within equal names sort by age
    for(int i = 1; i < v.size(); i++){
        Record key = v[i];
        int j = i - 1;

        while(j >= 0){
            tcomp++;
            if(v[j].name == key.name && v[j].age > key.age){
                v[j+1] = v[j];
                j--;
            } else break;
        }
        v[j+1] = key;
    }
    return tcomp;
}

// read file
void readFile(const std::string &path, std::vector<Record> &v){
    std::ifstream in(path);
    Record r;
    while(in >> r.name >> r.age){
        v.push_back(r);
    }
}

// write file
void writeFile(const std::string &path, const std::vector<Record> &v){
    std::ofstream out(path);
    for(const auto &r : v){
        out << r.name << " " << r.age << "\n";
    }
}

void display(std :: vector<int> V){
    for(int i = 0; i < V.size(); i++){
        std :: cout << V[i] << " ";
    } std :: cout << "\n";
}

int main(){
    //generateRecords();
    
    std::ofstream avg("comparisons.csv");
    avg << "n,avg_age,avg_name,avg_name_then_age\n";

    for(int n = 10; n < 101; n += 10){
        int sumAge = 0, sumName = 0, sumBoth = 0;
        for(int dn = 1; dn < 11; dn++){
            std::string base =
                "IS_HEMANTKUMAR_n" + std::to_string(n) +
                "_dataset" + std::to_string(dn) + ".txt";

            std::string inPath  = "datasets/" + base;

            std::vector<Record> v;
            readFile(inPath, v);

            // A) AGE sort
            {
                std::vector<Record> temp = v;
                sumAge += insertionSortAge(temp);
                writeFile("sorted/age_" + base, temp);
            }
            // B) NAME sort
            {
                std::vector<Record> temp = v;
                sumName += insertionSortName(temp);
                writeFile("sorted/name_" + base, temp);
            }
            // C) NAME → AGE sort (stable)
            {
                std::vector<Record> temp = v;
                sumBoth += insertionSortNameAge(temp);
                writeFile("sorted/both_" + base, temp);
            }
        }

        double avgAge  = sumAge  / 10.0;
        double avgName = sumName / 10.0;
        double avgBoth = sumBoth / 10.0;

        avg << n << "," << avgAge << "," << avgName << "," << avgBoth << "\n";

        std::cout << "n=" << n
                  << "  avg_age=" << avgAge
                  << "  avg_name=" << avgName
                  << "  avg_name->age=" << avgBoth << "\n";
    }

    return 0;
}