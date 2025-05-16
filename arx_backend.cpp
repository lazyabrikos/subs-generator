#include <iostream>
#include <vector>
#include <crow.h>
#include <nlohmann/json.hpp>
#include <bitset>

using json = nlohmann::json;
using namespace std;

unsigned char rotateBits(unsigned char x, int k) {
     x = (x & 0x0F);
    x = ((x << k) | (x >> (4 - k)));
    return x;
}

void arxIteration(unsigned char X_l, unsigned char X_r, unsigned char &X_l_next, unsigned char &X_r_next, int r, int s) {
unsigned char X_r_rotated = rotateBits(X_r, r);
    unsigned char z = (X_l + X_r_rotated);
    unsigned char z_rotated = rotateBits(z, s);
    X_r_next = (z_rotated ^ X_r);
    X_l_next = z;
}

vector<int> calculateARX(const vector<pair<int, int>>& combinations) {
    vector<unsigned char> result;

    for (unsigned char x_l = 0; x_l < 16; x_l++) {
        for (unsigned char x_r = 0; x_r < 16; x_r++) {
            
            unsigned char x_l_cur = x_l;
            unsigned char x_r_cur = x_r;
            unsigned char start_number = (x_l_cur << 4) | x_r_cur;

            for (int i = 0; i < 5; i++) {
                unsigned char x_l_next = 0;
                unsigned char x_r_next = 0;

                arxIteration(x_l_cur, x_r_cur, x_l_next, x_r_next, combinations[i].first, combinations[i].second);

                x_l_cur = x_l_next;
                x_r_cur = x_r_next;
            }

            unsigned char result_number = (x_l_cur << 4) | x_r_cur;
            result.push_back(result_number);
            
            // Output result_number as bit sequence
            
        }
    }
    
    vector<int> int_result;
    for (unsigned char val : result) {
        int_result.push_back(static_cast<int>(val));
    }
    
    return int_result;
}

int main() {
    // Создаем простой HTTP сервер
    crow::SimpleApp app;

    // Обработчик POST запроса для /api/calculate
    CROW_ROUTE(app, "/api/calculate").methods("POST"_method)
    ([](const crow::request& req) {
        // Добавляем CORS заголовки напрямую в ответ
        crow::response response;
        response.add_header("Access-Control-Allow-Origin", "*");
        response.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        response.add_header("Access-Control-Allow-Headers", "Content-Type");
        
        try {
            // Разбираем входящий JSON
            auto reqJson = json::parse(req.body);
            vector<pair<int, int>> combinations;

            // Проверяем наличие необходимого поля
            if (!reqJson.contains("combinations")) {
                response.code = 400;
                response.body = "Bad Request: Missing 'combinations' field";
                return response;
            }

            // Преобразуем JSON массив в вектор пар
            for (const auto& combination : reqJson["combinations"]) {
                if (combination.size() != 2) {
                    response.code = 400;
                    response.body = "Bad Request: Each combination must contain exactly 2 integers";
                    return response;
                }
                combinations.push_back({combination[0], combination[1]});
            }

            // Вызываем функцию для вычисления
            auto result = calculateARX(combinations);

            // Формируем ответ в формате JSON
            json jsonResponse = result;
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

    // Обработчик OPTIONS запроса для CORS preflight
    CROW_ROUTE(app, "/api/calculate").methods("OPTIONS"_method)
    ([](const crow::request&) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.code = 200;
        return res;
    });

    // Информационный маршрут для проверки работоспособности сервера
    CROW_ROUTE(app, "/")([]() {
        return "ARX Calculator Backend (C++)";
    });

    // Запускаем сервер на порту 8081
    cout << "Starting C++ ARX Calculator Backend on port 8081..." << endl;
    app.port(8081).run();

    return 0;
} 