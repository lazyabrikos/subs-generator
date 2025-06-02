#include <iostream>
#include <vector>
#include <crow.h>
#include <nlohmann/json.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <set>
#include <bitset>
#include <algorithm>
#include <cstdlib>
#include <cstring>

using json = nlohmann::json;
using namespace std;

struct Characteristics {
    double diffCharacteristic;
    double linearCharacteristic;
    int nonlinearDegree;
};

vector<int> V_n = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};


unsigned char rotateBits(unsigned char x, int k) {

    x = (x & 0x0F);
    x = ((x << k) | (x >> (4 - k))) & 0x0F;
    return x;
}

void arxIteration(unsigned char X_l, unsigned char X_r, unsigned char &X_l_next, unsigned char &X_r_next, int r, int s) {
    unsigned char X_r_rotated = rotateBits(X_r, r);
    unsigned char z = (X_l + X_r_rotated) & 0x0F;
    unsigned char z_rotated = rotateBits(z, s);
    X_r_next = (z_rotated ^ X_r);
    X_l_next = z;
}

vector<int> fullArxIteration(const vector<pair<int, int>> &combinations) {

    vector<unsigned char> result;

    for (unsigned char x_l = 0; x_l < 16; x_l++) {
        for (unsigned char x_r = 0; x_r < 16; x_r++) {
            
            unsigned char x_l_cur = x_l;
            unsigned char x_r_cur = x_r;
            unsigned char start_number = (x_l_cur << 4) | x_r_cur;

            for (int i = 0; i < combinations.size(); i++) {
                unsigned char x_l_next = 0;
                unsigned char x_r_next = 0;

                arxIteration(x_l_cur, x_r_cur, x_l_next, x_r_next, combinations[i].first, combinations[i].second);

                x_l_cur = x_l_next;
                x_r_cur = x_r_next;
            }

            unsigned char result_number = (x_l_cur << 4) | x_r_cur;
            result.push_back(result_number);
        }
    }

    vector<int> int_result;
    for (int i = 0; i < result.size(); i++) {
        int_result.push_back(static_cast<int>(result[i]));
    }

    return int_result;

}

vector<vector<int>> calculateBitsetMatrix(const vector<int> & subs) {
    vector<vector<int>> bitsetSubs;


    for (int num : subs) {
        bitset<8> bits(num);
        vector<int> row;
        for (int i = 7; i >= 0; --i) {
            row.push_back(bits[i]);     
        }
        bitsetSubs.push_back(row);
    }


    return bitsetSubs;
}

vector<vector<int>> readMatrix(const string & filename) {

    ifstream file(filename);
    vector<vector<int>> matrix;

    string line;
    while (getline(file, line)) {
        vector<int> row;
        istringstream ss(line);
        int num;
        while (ss >> num) {
            row.push_back(num);
        }
        matrix.push_back(row);
    }
    file.close();

    return matrix;
}


double calculateDiffCharacteristic(const vector<int>& substitution) {
    vector<vector<int>> matrix(256, vector<int> (256, 0));
    for (int i = 0; i < V_n.size(); i++) {
        for (int j = 0; j < substitution.size(); j++) {
            int x_a = substitution[j] ^ V_n[i];
            int b = substitution[x_a] ^ substitution[i];
            matrix[V_n[i]][b] += 1;
        }
    }


    int max = -1;

    for (int i = 1; i < 256; i++) {
        for (int j = 1; j < 256; j++) {
            if (matrix[i][j] > max) {
                max = matrix[i][j];
            }
        }
    }

    return static_cast<double>(max)/256;
}

int compute_scalar_product(int x, int y) {
    int result = 0;
    while (x > 0 || y > 0) {
        result ^= (x & 1) & (y & 1);
        x >>= 1;
        y >>= 1;
    }
    return result;
}

vector<tuple<int, int, double>> compute_probability(const vector<int>& pi, int n) {
    vector<tuple<int, int, double>> probabilities;
    vector<vector<int>> matrix(256, vector<int>(256, 0));
    int max_value = 1 << n;

    for (int a = 1; a < max_value; ++a) {
        for (int b = 1; b < max_value; ++b) {
            int count = 0;
            for (int x = 0; x < max_value; ++x) {
                int scalar_x_a = compute_scalar_product(x, a);
                int scalar_pi_x_b = compute_scalar_product(pi[x], b);
                if (scalar_x_a == scalar_pi_x_b) {
                    ++count;
                }
            }
            double probability = static_cast<double>(count) / max_value;
            probabilities.emplace_back(a, b, probability);
            if (a < 256 && b < 256) {
                matrix[b][a] = count;
            }
        }
    }

    return probabilities;
}


double calculateLinearCharacteristic(const vector<int>& substitution) {
    vector<tuple<int, int, double>> probabilities = compute_probability(substitution, 8);
    double delta = 0.0;

    for (const auto& entry : probabilities) {
        double p = get<2>(entry);
        double deviation = abs(2 * p - 1);
        if (deviation > delta) {
            delta = deviation;
        }
    }
    
    return delta;
}

vector<vector<int>> calculateMatrix(const vector<vector<int>> & matrix, const vector<int> & subs) {
    vector<vector<int>> bitsetSubs = calculateBitsetMatrix(subs);
    
    int size = 256;

    vector<vector<int>> result_matrix(size, vector<int>(size, 0));

    for (int mask_num = 0; mask_num < size; ++mask_num) {
        bitset<8> mask(mask_num);

        for (int row = 0; row < size; ++row) {
            int xor_sum = 0;
            for (int col = 0; col < 8; ++col) {
                if (mask[7 - col]) { 
                    xor_sum ^= bitsetSubs[row][col];
                }
            }
            result_matrix[mask_num][row] = xor_sum;
        }
    }

    vector<vector<int>> ans(size, vector<int>(size, 0));
    for (int num = 0; num < size; num++){
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                ans[num][i] += matrix[i][j] * result_matrix[num][j];
            }
        }
    }
    return ans;
}

int calculateNoLinear(const vector<vector<int>> & matrix, const vector<int> & subs, const vector<int> & invSubs) {
    int size = 256;
    vector<vector<int>> ans = calculateMatrix(matrix, subs);
    vector<vector<int>> invAns = calculateMatrix(matrix, invSubs);
    vector<int> mins;
    for (int i = 0; i < size; i++) {
        int maxAns = 0;
        int invMax = 0;
        for (int j = 0; j < size; j++) {
            ans[i][j] = ans[i][j] % 2;
            invAns[i][j] = invAns[i][j] % 2;
            if (ans[i][j] == 1) {
                bitset<8> mask(j);
                int count = 0;
                for (int k = 0; k < 8; k++) {
                    if (mask[k] == 1) {
                        count++;
                    }
                }
                maxAns = max(count, maxAns);
            }

            if (invAns[i][j] == 1) {
                bitset<8> mask(j);
                int count = 0;
                for (int k = 0; k < 8; k++) {
                    if (mask[k] == 1) {
                        count++;
                    }
                }
                invMax = max(count, invMax);
            }
        }
        if (maxAns != 0 && invMax != 0) {
            mins.push_back(min(maxAns, invMax));
        } else if (maxAns == 0 && invMax != 0) {
            mins.push_back(invMax);
        } else if (invMax == 0 && maxAns != 0) {
            mins.push_back(maxAns);
        }
    }

    int min = 1000;
    for (int i = 0; i < mins.size(); i++) {
        if (mins[i] < min) {
            min = mins[i];
        }
    }

    return min;
}


bool checkFullSubstitution(const vector<int> & subs) {
    set<int> s;
    for (int i = 0; i < subs.size(); i++) {
        s.insert(subs[i]);
    }

    return s.size() == 256;
}

vector<int> madeInvSubs(const vector<int> & subs) {
    vector<int> invSubs(256, 0);
    for (int i = 0; i < 256; i++) {
        invSubs[subs[i]] = i;
    } 
    return invSubs;
} 

int calculateNonlinearDegree(const vector<int>& substitution) {
    vector<vector<int>> matrix = readMatrix("/app/D_matrix_nolinear.txt");
    if (matrix.empty()) {
        cerr << "Error: Could not read D_matrix_nolinear.txt" << endl;
        return 1;
    }
    if (!checkFullSubstitution(substitution)) {
        return -1;
    } else {
        vector<int> invSubs = madeInvSubs(substitution);
        int mn = calculateNoLinear(matrix, substitution, invSubs);
        return mn; 
    }
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/calculate").methods("POST"_method)
    ([](const crow::request& req) {
        crow::response response;
        response.add_header("Access-Control-Allow-Origin", "*");
        response.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        response.add_header("Access-Control-Allow-Headers", "Content-Type");
        
        try {
            auto reqJson = json::parse(req.body);
            vector<pair<int, int>> combinations;

            if (!reqJson.contains("combinations")) {
                response.code = 400;
                response.body = "Bad Request: Missing 'combinations' field";
                return response;
            }

            for (const auto& combination : reqJson["combinations"]) {
                if (combination.size() != 2) {
                    response.code = 400;
                    response.body = "Bad Request: Each combination must contain exactly 2 integers";
                    return response;
                }
                combinations.push_back({combination[0], combination[1]});
            }

            auto substitution = fullArxIteration(combinations);

            Characteristics characteristics;
            characteristics.diffCharacteristic = calculateDiffCharacteristic(substitution);
            characteristics.linearCharacteristic = calculateLinearCharacteristic(substitution);
            characteristics.nonlinearDegree = calculateNonlinearDegree(substitution);

            json jsonResponse = {
                {"substitution", substitution},
                {"characteristics", {
                    {"diffCharacteristic", characteristics.diffCharacteristic},
                    {"linearCharacteristic", characteristics.linearCharacteristic},
                    {"nonlinearDegree", characteristics.nonlinearDegree == -1 ? 
                        "Not a full substitution" : 
                        to_string(characteristics.nonlinearDegree)}
                }}
            };
            
            response.code = 200;
            response.body = jsonResponse.dump();
            response.set_header("Content-Type", "application/json");
            
            return response;
        } catch (const std::exception& e) {
            response.code = 500;
            response.body = string("Internal Server Error: ") + e.what();
            return response;
        }
    });

    CROW_ROUTE(app, "/api/calculate").methods("OPTIONS"_method)
    ([](const crow::request&) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.code = 200;
        return res;
    });

    CROW_ROUTE(app, "/")([]() {
        return "ARX Calculator Backend (C++)";
    });

    cout << "Starting C++ ARX Calculator Backend on port 8081..." << endl;
    app.port(8081).run();

    return 0;
} 