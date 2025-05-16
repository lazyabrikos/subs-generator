#!/bin/bash

# Останавливаем существующий контейнер (если есть)
echo "Останавливаем существующий C++ бэкенд (если есть)..."
sudo docker stop arx-calculator-cpp-backend 2>/dev/null || true
sudo docker rm arx-calculator-cpp-backend 2>/dev/null || true

# Создаем Docker сеть, если она не существует
if ! sudo docker network inspect subs-generator_default > /dev/null 2>&1; then
  echo "Создаем Docker сеть..."
  sudo docker network create subs-generator_default
fi

# Собираем образ
echo "Собираем C++ бэкенд..."
sudo docker build -t subs-generator-cpp-backend -f Dockerfile.cpp_backend .

# Запускаем контейнер
echo "Запускаем C++ бэкенд..."
sudo docker run -d -p 8081:8081 --name arx-calculator-cpp-backend --network subs-generator_default subs-generator-cpp-backend

# Проверка запущенных контейнеров
echo "Проверка запущенного контейнера:"
sudo docker ps | grep arx-calculator-cpp-backend

echo -e "\nC++ бэкенд запущен!"
echo "API: http://localhost:8081/api/calculate"
echo -e "\nДля тестирования можно использовать curl:"
echo "curl -X POST -H \"Content-Type: application/json\" -d '{\"combinations\":[[1,1],[1,1],[1,1],[1,1],[1,2]]}' http://localhost:8081/api/calculate" 