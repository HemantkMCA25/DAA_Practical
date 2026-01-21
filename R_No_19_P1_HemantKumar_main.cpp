
#include<iostream>
#include<string>
#include<vector>
#include<random>
#include<fstream>
#include<direct.h>

// Represents a single dataset record: person's name and age.
struct Record
{
    int age;
    std::string name;
};

// Pool of candidate names for synthetic dataset generation.
std::vector<std::string> name_pool = {
    "Hemant", "Sonu", "Pranav", "Sagar", "Ravi", "Aniket", "Rahul",
    "Mohit", "Iradri", "Kartik", "Ashwin", "Sahil", "Aditya", "Gautam", "Anurag"
};

// RNG setup (Mersenne Twister seeded with random_device).
static std::random_device rd;
static std::mt19937 gen(rd());

// Returns a random name from name_pool
std::string getRandomName(){
    std::uniform_int_distribution<std::size_t> dist(0, name_pool.size() -1);
    return name_pool[dist(gen)];
}

// Returns a random age in [18, 35] (inclusive).
int getRandomAge(){
    static std::uniform_int_distribution<int> dist(18, 35);
    return dist(gen);
}

// Generates 10 datasets for each n ∈ {10,20,...,100}; each file has n "name age" rows.
// Files written under dir datasets/ with pattern: IS_HEMANTKUMAR_n{n}_dataset{dn}.txt
void generateRecords(){
    for(int n = 10; n < 101; n += 10){
        for(int dn = 1; dn < 11; ++dn)
        {    
            std::string filename = "datasets/R_No_19_P1_HEMANTKUMAR_n" + std::to_string(n) + "_dataset" + std::to_string(dn) + ".txt";
            std::ofstream outputFile(filename);
            for(int i = 0; i < n; i++) outputFile << getRandomName() << " " << getRandomAge() << "\n";
        }
    }
}

// Insertion sort by age (ascending).
// Tracks comparisons {comp} and assignments {assign} performed.
void insertionSortAge(std::vector<Record> &v, int &comp, int &assign){
    comp = 0, assign = 0;
    for(int i = 1; i < (int)v.size(); i++){
        Record key = v[i];      // key copy
        assign++;               // key assignment
        int j = i - 1;
        while(j >= 0){
            comp++;             // comparisons with key
            if(v[j].age > key.age){
                assign++;       // shift assignment
                v[j+1] = v[j];
                j--;
            } else break;
        }
        v[j+1] = key;           // insert key assignment
        assign++;
    }
}

// Insertion sort by name (lexicographic, ascending).
// Tracks comparisons (comp) and assignments (assign).
void insertionSortName(std::vector<Record> &v, int &comp, int &assign){
    comp = 0, assign = 0;
    for(int i = 1; i < (int)v.size(); i++){
        Record key = v[i];      // key copy
        assign++;               // key assignment
        int j = i - 1;
        while(j >= 0){
            comp++;             // comparisons with key
            if(v[j].name > key.name){
                assign++;       // shift assignment
                v[j+1] = v[j];
                j--;
            } else break;
        }
        assign++;               // insert key assignment
        v[j+1] = key;
    }
}

// Two-pass sort: first by age, then by name.
// Note: insertionSortName is stable, so within equal names, age order from pass 1 may be affected.
// This effectively prioritizes NAME (second pass) as primary key and AGE as secondary only if second pass preserves ties.
// Tracks total comparisons/assignments across both passes.
void insertionSortAgeName2(std::vector<Record> &v, int &tcomp, int &tassign){
    int c1=0,a1=0,c2=0,a2=0;

    insertionSortAge(v, c1, a1);
    insertionSortName(v, c2, a2);

    tcomp   = c1 + c2;
    tassign = a1 + a2;
}

// Reads "name age" lines from file at 'path' into vector v (appends).
void readFile(const std::string &path, std::vector<Record> &v){
    std::ifstream in(path);
    Record r;
    while(in >> r.name >> r.age){
        v.push_back(r);
    }
}

// Writes vector v as "name age" lines to file at 'path'.
void writeFile(const std::string &path, const std::vector<Record> &v){
    std::ofstream out(path);
    for(const auto &r : v){
        out << r.name << " " << r.age << "\n";
    }
}

int main()
{
    // Create output directories
    _mkdir("datasets");
    _mkdir("sorted");

    // Produce datasets.
    generateRecords();
    
    // CSVs for average counts across 10 datasets per n.
    std::ofstream avgass("assignments.csv");
    avgass << "n,avg_assignments_both,avg_assignments_age,avg_assignments_name\n";

    std::ofstream avg("comparisons.csv");
    avg << "n,avg_comparisons_both,avg_comparisons_age,avg_comparisons_name\n";

    for(int n = 10; n < 101; n += 10){

        // Accumulators over dn=1..10
        int compsumBoth = 0, assignsumBoth = 0;
        int compsumAge  = 0, assignsumAge  = 0;
        int compsumName = 0, assignsumName = 0;
        
        for(int dn = 1; dn < 11; dn++){

            std::string base =
                "R_No_19_P1_HEMANTKUMAR_n" + std::to_string(n) +
                "_dataset" + std::to_string(dn) + ".txt";

            std::string inPath  = "datasets/" + base;
            std::vector<Record> v;
            readFile(inPath, v);

            // AGE-only sort (measures only; result not saved)
            {
                std::vector<Record> temp = v;
                int tc = 0, ta = 0;
                insertionSortAge(temp, tc, ta);
                compsumAge   += tc;
                assignsumAge += ta;
                writeFile("sorted/age_" + base, temp);
            }

            // NAME-only sort (measures only; result not saved)
            {
                std::vector<Record> temp = v;
                int tc = 0, ta = 0;
                insertionSortName(temp, tc, ta);
                compsumName   += tc;
                assignsumName += ta;
                writeFile("sorted/name_" + base, temp);
            }

            // BOTH: AGE then NAME; writes final order to sorted/ (for inspection)
            {
                std::vector<Record> temp = v;
                int tc = 0, ta = 0;

                insertionSortAgeName2(temp, tc, ta);
                compsumBoth   += tc;
                assignsumBoth += ta;

                writeFile("sorted/both_" + base, temp);
            }
        }

        // Averages across the 10 datasets for this n.
        double avgComparisonsBoth = compsumBoth   / 10.0;
        double avgAssignmentsBoth = assignsumBoth / 10.0;

        double avgComparisonsAge = compsumAge   / 10.0;
        double avgAssignmentsAge = assignsumAge / 10.0;

        double avgComparisonsName = compsumName   / 10.0;
        double avgAssignmentsName = assignsumName / 10.0;

        // Write CSV rows.
        avg    << n << "," << avgComparisonsBoth << "," << avgComparisonsAge << "," << avgComparisonsName << "\n";
        avgass << n << "," << avgAssignmentsBoth << "," << avgAssignmentsAge << "," << avgAssignmentsName << "\n";

        // Console summary for quick feedback.
        std::cout << "n=" << n
                  << "  avg_comp(both|age|name) = " << avgComparisonsBoth << " | " << avgComparisonsAge << " | " << avgComparisonsName
                  << "  avg_assign(both|age|name) = " << avgAssignmentsBoth << " | " << avgAssignmentsAge << " | " << avgAssignmentsName
                  << "\n";
    }
    return 0;
}