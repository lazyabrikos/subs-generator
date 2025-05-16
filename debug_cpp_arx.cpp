#include <iostream>
#include <bitset>
#include <vector>
#include <iomanip>

using namespace std;

// Подробная версия функции для поворота битов
unsigned char rotateBits(unsigned char x, int k) {
    cout << "rotateBits: x=" << (int)x << " (0x" << hex << (int)x << "), k=" << dec << k << endl;
    
    // Step 1: Mask to 4 bits
    unsigned char masked = (x & 0x0F);
    cout << "  After mask: " << (int)masked << " (0x" << hex << (int)masked << ")" << endl;
    
    // Step 2: Shift left by k
    unsigned char leftShift = (masked << k);
    cout << "  Left shift by " << dec << k << ": " << (int)leftShift << " (0x" << hex << (int)leftShift << ")" << endl;
    
    // Step 3: Shift right by (4-k)
    unsigned char rightShift = (masked >> (4 - k));
    cout << "  Right shift by " << dec << (4-k) << ": " << (int)rightShift << " (0x" << hex << (int)rightShift << ")" << endl;
    
    // Step 4: Combine with OR
    unsigned char combined = (leftShift | rightShift);
    cout << "  After OR: " << (int)combined << " (0x" << hex << (int)combined << ")" << endl;
    
    // Mask the result to 4 bits
    unsigned char result = combined & 0x0F;
    cout << "  Final result (masked): " << (int)result << " (0x" << hex << (int)result << ")" << endl;
    
    return result;
}

// Подробная версия функции для одной ARX итерации
void arxIteration(unsigned char X_l, unsigned char X_r, unsigned char &X_l_next, unsigned char &X_r_next, int r, int s) {
    cout << "arxIteration: X_l=" << (int)X_l << " (0x" << hex << (int)X_l 
         << "), X_r=" << (int)X_r << " (0x" << hex << (int)X_r 
         << "), r=" << dec << r << ", s=" << s << endl;
    
    // Поворот X_r на r бит
    unsigned char X_r_rotated = rotateBits(X_r, r);
    cout << "  X_r_rotated = " << (int)X_r_rotated << endl;
    
    // Сложение с беззнаковым переполнением
    unsigned char z = (X_l + X_r_rotated) & 0xFF;
    cout << "  z = (X_l + X_r_rotated) & 0xFF = (" << (int)X_l << " + " << (int)X_r_rotated << ") & 0xFF = " << (int)z << endl;
    
    // Поворот z на s бит
    unsigned char z_rotated = rotateBits(z, s);
    cout << "  z_rotated = " << (int)z_rotated << endl;
    
    // XOR и присваивание
    X_r_next = (z_rotated ^ X_r) & 0xFF;
    cout << "  X_r_next = (z_rotated ^ X_r) & 0xFF = (" << (int)z_rotated << " ^ " << (int)X_r << ") & 0xFF = " << (int)X_r_next << endl;
    
    X_l_next = z;
    cout << "  X_l_next = z = " << (int)X_l_next << endl;
    
    cout << "  Result: X_l_next=" << (int)X_l_next << ", X_r_next=" << (int)X_r_next << endl;
}

// Функция для тестирования ARX с указанной комбинацией
vector<int> testARX(const vector<pair<int, int>>& combinations) {
    vector<int> result;
    
    // Test only with specific values to simplify
    for (unsigned char x_l = 0; x_l < 3; x_l++) {
        for (unsigned char x_r = 0; x_r < 3; x_r++) {
            cout << "\n--- Initial values: x_l=" << (int)x_l << ", x_r=" << (int)x_r << " ---" << endl;
            
            unsigned char x_l_cur = x_l;
            unsigned char x_r_cur = x_r;
            
            // Применяем каждую комбинацию последовательно
            for (const auto& combination : combinations) {
                cout << "Applying combination r=" << combination.first << ", s=" << combination.second 
                     << " to x_l=" << (int)x_l_cur << ", x_r=" << (int)x_r_cur << endl;
                
                unsigned char x_l_next = 0;
                unsigned char x_r_next = 0;
                
                arxIteration(x_l_cur, x_r_cur, x_l_next, x_r_next, combination.first, combination.second);
                
                x_l_cur = x_l_next;
                x_r_cur = x_r_next;
                
                cout << "After iteration: x_l_cur=" << (int)x_l_cur << ", x_r_cur=" << (int)x_r_cur << endl;
            }
            
            // Маскируем x_l_cur и x_r_cur и собираем результат
            unsigned char x_l_masked = x_l_cur & 0x0F;
            unsigned char x_r_masked = x_r_cur & 0x0F;
            
            cout << "Final masked: x_l_masked=" << (int)x_l_masked << " (0x" << hex << (int)x_l_masked 
                 << "), x_r_masked=" << (int)x_r_masked << " (0x" << hex << (int)x_r_masked << ")" << endl;
            
            unsigned char result_number = (x_l_masked << 4) | x_r_masked;
            cout << "Result number: " << dec << (int)result_number << " (0x" << hex << (int)result_number << ")" << endl;
            
            result.push_back(static_cast<int>(result_number));
        }
    }
    
    return result;
}

int main() {
    cout << "=== ARX Algorithm Debug ===" << endl;
    
    // Test with combination [1,1]
    vector<pair<int, int>> combinations = {{1, 1}};
    
    cout << "Testing with combination: [1,1]" << endl;
    auto results = testARX(combinations);
    
    // Display results
    cout << "\nResults:" << endl;
    for (size_t i = 0; i < results.size(); i++) {
        cout << "Result[" << i << "] = " << results[i] << endl;
    }
    
    return 0;
} 