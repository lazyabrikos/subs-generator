#!/bin/bash

# Цветовые коды для вывода
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Запуск приложения Subs Generator ===${NC}"

# Останавливаем существующие контейнеры (если есть)
echo -e "\n${BLUE}Останавливаем существующие контейнеры...${NC}"
sudo docker stop arx-calculator-cpp-backend arx-calculator-frontend 2>/dev/null || true
sudo docker rm arx-calculator-cpp-backend arx-calculator-frontend 2>/dev/null || true

# Создаем Docker сеть, если она не существует
if ! sudo docker network inspect subs-generator_default > /dev/null 2>&1; then
  echo -e "\n${BLUE}Создаем Docker сеть...${NC}"
  sudo docker network create subs-generator_default
fi

# Собираем и запускаем C++ бэкенд
echo -e "\n${BLUE}Собираем C++ бэкенд...${NC}"
sudo docker build -t subs-generator-cpp-backend -f Dockerfile.cpp_backend .

echo -e "\n${BLUE}Запускаем C++ бэкенд...${NC}"
sudo docker run -d -p 8081:8081 --name arx-calculator-cpp-backend --network subs-generator_default subs-generator-cpp-backend

# Собираем и запускаем Frontend
echo -e "\n${BLUE}Собираем Frontend...${NC}"
sudo docker build -t subs-generator-frontend -f Dockerfile.frontend .

echo -e "\n${BLUE}Запускаем Frontend...${NC}"
sudo docker run -d -p 80:80 --name arx-calculator-frontend --network subs-generator_default subs-generator-frontend

# Проверка запущенных контейнеров
echo -e "\n${BLUE}Проверка запущенных контейнеров:${NC}"
RUNNING_CONTAINERS=$(sudo docker ps | grep -E "arx-calculator-cpp-backend|arx-calculator-frontend")
echo "$RUNNING_CONTAINERS"

# Проверяем, запустились ли оба контейнера
if echo "$RUNNING_CONTAINERS" | grep -q "arx-calculator-cpp-backend" && echo "$RUNNING_CONTAINERS" | grep -q "arx-calculator-frontend"; then
  echo -e "\n${GREEN}✅ Приложение успешно запущено!${NC}"
  echo -e "${GREEN}Frontend: http://localhost${NC}"
  echo -e "${GREEN}C++ API: http://localhost:8081/api/calculate${NC}"
  
  echo -e "\n${BLUE}Для тестирования C++ API можно использовать curl:${NC}"
  echo "curl -X POST -H \"Content-Type: application/json\" -d '{\"combinations\":[[1,1],[1,1],[1,1],[1,1],[1,2]]}' http://localhost:8081/api/calculate"
else
  echo -e "\n${RED}❌ Ошибка при запуске приложения. Проверьте логи контейнеров:${NC}"
  echo "sudo docker logs arx-calculator-cpp-backend"
  echo "sudo docker logs arx-calculator-frontend"
fi 