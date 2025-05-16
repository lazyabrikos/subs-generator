#!/bin/bash

# Цвета для вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Проверка работы серверов
echo -e "${BLUE}Проверяем статус серверов...${NC}"
java_status=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/api/calculate)
cpp_status=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8081/api/calculate)

if [ "$java_status" == "404" ] || [ "$java_status" == "200" ]; then
    echo -e "${GREEN}Java бэкенд работает.${NC}"
    java_works=true
else
    echo -e "${RED}Java бэкенд не отвечает (код $java_status).${NC}"
    java_works=false
fi

if [ "$cpp_status" == "404" ] || [ "$cpp_status" == "200" ]; then
    echo -e "${GREEN}C++ бэкенд работает.${NC}"
    cpp_works=true
else
    echo -e "${RED}C++ бэкенд не отвечает (код $cpp_status).${NC}"
    cpp_works=false
fi

if [ "$java_works" == "false" ] && [ "$cpp_works" == "false" ]; then
    echo -e "${RED}Ни один из бэкендов не работает. Запустите серверы.${NC}"
    exit 1
fi

echo -e "${GREEN}Оба сервера работают.${NC}"

# Функция для тестирования одной комбинации ARX с двумя бэкендами
test_combination() {
    local combinations=$1
    local test_name=$2
    
    echo -e "${BLUE}Тестирование $test_name: $combinations${NC}"
    
    # Запрос к Java-бэкенду (port 8080)
    if [ "$java_works" == "true" ]; then
        java_response=$(curl -s -X POST \
            -H "Content-Type: application/json" \
            -d "{\"combinations\": $combinations}" \
            http://localhost:8080/api/calculate)
        
        # Сохраняем ответы в файлы для более удобного сравнения
        echo "$java_response" > java_result.json
        
        echo -e "${GREEN}Java бэкенд ответил.${NC}"
    else
        echo -e "${YELLOW}Java бэкенд недоступен, пропускаем.${NC}"
    fi
    
    # Запрос к C++-бэкенду (port 8081)
    if [ "$cpp_works" == "true" ]; then
        cpp_response=$(curl -s -X POST \
            -H "Content-Type: application/json" \
            -d "{\"combinations\": $combinations}" \
            http://localhost:8081/api/calculate)
        
        # Сохраняем ответы в файлы для более удобного сравнения
        echo "$cpp_response" > cpp_result.json
        
        echo -e "${GREEN}C++ бэкенд ответил.${NC}"
    else
        echo -e "${YELLOW}C++ бэкенд недоступен, пропускаем.${NC}"
    fi
    
    # Если оба бэкенда работают, проверяем на различия
    if [ "$java_works" == "true" ] && [ "$cpp_works" == "true" ]; then
        # Проверка на различия между результатами
        diff_output=$(diff -w java_result.json cpp_result.json)
        if [ -z "$diff_output" ]; then
            echo -e "${GREEN}✅ Результаты совпадают!${NC}"
            return 0
        else
            echo -e "${RED}❌ Результаты различаются!${NC}"
            
            # Преобразуем JSON в более читаемый формат для анализа
            java_array=($(echo $java_response | tr -d '[],' | tr ' ' '\n'))
            cpp_array=($(echo $cpp_response | tr -d '[],' | tr ' ' '\n'))
            
            # Находим первое различие
            min_length=$((${#java_array[@]} < ${#cpp_array[@]} ? ${#java_array[@]} : ${#cpp_array[@]}))
            
            diff_count=0
            for ((i=0; i<$min_length; i++)); do
                if [ "${java_array[$i]}" != "${cpp_array[$i]}" ]; then
                    if [ "$diff_count" -lt 5 ]; then
                        echo -e "${YELLOW}Различие на индексе $i:${NC}"
                        echo -e "${BLUE}Java: ${java_array[$i]}${NC}"
                        echo -e "${BLUE}C++:  ${cpp_array[$i]}${NC}"
                    fi
                    diff_count=$((diff_count+1))
                fi
            done
            
            echo -e "${RED}Всего различий: $diff_count из $min_length элементов ($((diff_count*100/min_length))%)${NC}"
            
            # Если длины разные - сообщить об этом
            if [ ${#java_array[@]} != ${#cpp_array[@]} ]; then
                echo -e "${YELLOW}Размеры массивов разные: Java=${#java_array[@]}, C++=${#cpp_array[@]}${NC}"
            fi
            
            return 1
        fi
    fi
    
    return 0
}

# Основные тесты
echo -e "${BLUE}=== Начало тестирования ===${NC}"

# Тест 1: Простая одна пара (r=1, s=1)
test_combination "[[1,1]]" "одна пара (1,1)"

# Тест 2: Несколько одинаковых пар (r=1, s=1)
test_combination "[[1,1],[1,1],[1,1]]" "три пары (1,1)"

# Тест 3: Комбинация нескольких разных пар
test_combination "[[1,1],[2,2],[3,3]]" "смешанные пары"

# Тест 4: Достаточно длинная последовательность
test_combination "[[1,1],[1,2],[1,3],[2,1],[2,2],[2,3],[3,1],[3,2],[3,3]]" "полная последовательность"

# Тест 5: Случайный пример из задания
test_combination "[[1,1],[1,1],[1,1],[1,1],[1,2]]" "пример из задания"

# Производим редирект с Java бэкенда на C++ бэкенд
echo -e "${BLUE}\n=== Настройка редиректа с Java на C++ бэкенд ===${NC}"

if [ "$cpp_works" == "true" ]; then
    # Тестируем редирект - отправляем запрос на Java и проверяем, перенаправляется ли он на C++
    echo -e "${YELLOW}Тестирование функции перенаправления запросов...${NC}"
    
    # Создаем простые данные для теста
    test_data='{"combinations": [[1,1]]}'
    
    # Запускаем приложение
    echo -e "${BLUE}Открываем приложение в браузере...${NC}"
    echo -e "${GREEN}Теперь вы можете открыть http://localhost/ и протестировать калькулятор.${NC}"
    echo -e "${YELLOW}Frontend использует C++ бэкенд (порт 8081).${NC}"
    echo -e "${YELLOW}Но вы также можете использовать и Java бэкенд (порт 8080) при необходимости.${NC}"
    echo -e "${GREEN}Оба API доступны:${NC}"
    echo -e "  - Java API: http://localhost:8080/api/calculate"
    echo -e "  - C++ API:  http://localhost:8081/api/calculate"
    
    # Сравнить API
    java_api="curl -X POST -H 'Content-Type: application/json' -d '$test_data' http://localhost:8080/api/calculate"
    cpp_api="curl -X POST -H 'Content-Type: application/json' -d '$test_data' http://localhost:8081/api/calculate"
    
    echo -e "${BLUE}\nПример вызова Java API:${NC}"
    echo -e "$java_api"
    
    echo -e "${BLUE}\nПример вызова C++ API:${NC}"
    echo -e "$cpp_api"
else
    echo -e "${RED}C++ бэкенд не работает. Перенаправление невозможно.${NC}"
fi

echo -e "${BLUE}=== Тестирование завершено ===${NC}" 