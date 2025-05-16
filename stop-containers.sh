#!/bin/bash

# Останавливаем контейнеры
echo "Останавливаем контейнеры..."
sudo docker stop arx-calculator-frontend arx-calculator-cpp-backend 2>/dev/null || true

# Удаляем контейнеры
echo "Удаляем контейнеры..."
sudo docker rm arx-calculator-frontend arx-calculator-cpp-backend 2>/dev/null || true

# Удаляем сеть
echo "Удаляем сеть..."
sudo docker network rm subs-generator_default 2>/dev/null || true

echo "Все контейнеры остановлены и удалены."

# Проверка запущенных контейнеров
echo "Проверка запущенных контейнеров (должно быть пусто):"
sudo docker ps | grep -i "arx\|subs\|calculator\|backend\|frontend" 