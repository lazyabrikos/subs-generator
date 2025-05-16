#!/bin/bash

# Отправляем запрос к обоим API с одной простой комбинацией
echo "Тестируем одну комбинацию [1,1]"
curl -s -X POST -H "Content-Type: application/json" -d '{"combinations":[[1,1]]}' http://localhost:8080/api/calculate > java_simple.json
curl -s -X POST -H "Content-Type: application/json" -d '{"combinations":[[1,1]]}' http://localhost:8081/api/calculate > cpp_simple.json

# Выводим только первые 10 элементов
echo "Первые 10 элементов из Java API:"
cat java_simple.json | tr ',' '\n' | tr -d '[]' | head -n 10

echo "Первые 10 элементов из C++ API:"
cat cpp_simple.json | tr ',' '\n' | tr -d '[]' | head -n 10

# Проверяем, есть ли различия
if diff -q java_simple.json cpp_simple.json > /dev/null; then
    echo "✅ Результаты совпадают"
else
    echo "❌ Результаты различаются"
    # Попробуем найти первую разницу
    echo "Поиск первого различия..."
    
    # Конвертируем JSON в строки по числам
    ja=($(cat java_simple.json | tr -d '[]' | tr ',' ' '))
    cp=($(cat cpp_simple.json | tr -d '[]' | tr ',' ' '))
    
    # Проверяем каждый элемент
    for i in "${!ja[@]}"; do
        if [ "${ja[$i]}" != "${cp[$i]}" ]; then
            echo "Различие найдено в позиции $i: Java=${ja[$i]}, C++=${cp[$i]}"
            
            # Конвертируем в бинарное представление
            java_bin=$(echo "obase=2;${ja[$i]}" | bc)
            cpp_bin=$(echo "obase=2;${cp[$i]}" | bc)
            
            echo "Java в двоичном: $java_bin"
            echo "C++ в двоичном:  $cpp_bin"
            
            # Разбиваем на high и low части (по 4 бита каждая)
            java_high=$((${ja[$i]} >> 4 & 0xF))
            java_low=$((${ja[$i]} & 0xF))
            cpp_high=$((${cp[$i]} >> 4 & 0xF))
            cpp_low=$((${cp[$i]} & 0xF))
            
            echo "Java: старшие биты=$java_high, младшие биты=$java_low"
            echo "C++: старшие биты=$cpp_high, младшие биты=$cpp_low"
            
            break
        fi
    done
fi 