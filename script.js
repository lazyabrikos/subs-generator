document.addEventListener('DOMContentLoaded', () => {
    const nSelect = document.getElementById('n-select');
    const pairsContainer = document.getElementById('pairs-container');
    const calculateBtn = document.getElementById('calculate-btn');
    const resultsContainer = document.getElementById('results-container');

    // Добавляем элемент для логов
    const debugLogContainer = document.createElement('div');
    debugLogContainer.id = 'debug-log';
    debugLogContainer.style.display = 'none';
    document.body.appendChild(debugLogContainer);

    // Функция для логирования
    function logDebug(message) {
        console.log(message);
        const logEntry = document.createElement('div');
        logEntry.textContent = `${new Date().toISOString()}: ${message}`;
        debugLogContainer.appendChild(logEntry);
    }

    // Включить логи для отладки
    window.showDebugLogs = function() {
        debugLogContainer.style.display = 'block';
    }

    // Array of possible rotations as defined in the C++ code
    const ROTATES = [
        [1, 1],
        [1, 2],
        [1, 3],
        [2, 1],
        [2, 2],
        [2, 3],
        [3, 1],
        [3, 2],
        [3, 3]
    ];

    nSelect.addEventListener('change', (e) => {
        const n = parseInt(e.target.value);
        if (!n) {
            pairsContainer.innerHTML = '';
            calculateBtn.disabled = true;
            return;
        }

        // Generate pairs
        let pairsHTML = '';
        for (let i = 0; i < n; i++) {
            pairsHTML += `
                <div class="pair-item">
                    <label>Пара ${i + 1}:</label>
                    <div class="pair-selects">
                        <select id="pair-${i}-first" onchange="validatePairs()">
                            <option value="1">1</option>
                            <option value="2">2</option>
                            <option value="3">3</option>
                        </select>
                        <select id="pair-${i}-second" onchange="validatePairs()">
                            <option value="1">1</option>
                            <option value="2">2</option>
                            <option value="3">3</option>
                        </select>
                    </div>
                </div>
            `;
        }
        pairsContainer.innerHTML = pairsHTML;
        validatePairs();
    });

    calculateBtn.addEventListener('click', async () => {
        const n = parseInt(nSelect.value);
        if (!n) return;

        logDebug(`Начало расчета для ${n} пар`);

        // Collect selected pairs
        const combinations = [];
        let pairsText = '';
        for (let i = 0; i < n; i++) {
            const first = parseInt(document.getElementById(`pair-${i}-first`).value);
            const second = parseInt(document.getElementById(`pair-${i}-second`).value);
            combinations.push([first, second]);
            pairsText += `(${first}, ${second})${i < n - 1 ? ', ' : ''}`;
        }

        logDebug(`Собраны пары: ${pairsText}`);

        try {
            // Show loading state
            calculateBtn.disabled = true;
            calculateBtn.textContent = 'Вычисление...';
            
            // Используем относительный URL для API (NGINX проксирует запросы)
            const apiUrl = '/api/calculate';
            logDebug(`Отправка запроса на API: ${apiUrl}`);
            
            // Подготавливаем данные для отправки
            const requestData = {
                combinations: combinations
            };
            logDebug(`Данные для отправки: ${JSON.stringify(requestData)}`);
            
            // Отправляем запрос на API
            const response = await fetch(apiUrl, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(requestData)
            });

            logDebug(`Статус ответа: ${response.status} ${response.statusText}`);

            if (!response.ok) {
                throw new Error(`Ошибка сервера: ${response.status} ${response.statusText}`);
            }

            // Получаем данные ответа
            const responseText = await response.text();
            logDebug(`Текст ответа: ${responseText.substring(0, 100)}...`);
            
            let results;
            try {
                results = JSON.parse(responseText);
                logDebug(`Успешно распарсили JSON: ${results.length} элементов`);
            } catch (parseError) {
                logDebug(`Ошибка парсинга JSON: ${parseError}`);
                throw new Error(`Ошибка при обработке ответа: ${parseError.message}`);
            }
            
            // Display results
            resultsContainer.innerHTML = `
                <h3>Результаты:</h3>
                <p>Используемые пары: ${pairsText}</p>
                <p>Количество итераций: ${n}</p>
                <p>Сгенерированная подстановка (${results.length} чисел):</p>
                <div class="results-grid">
                    ${results.map((val, idx) => {
                        return `<span title="Индекс: ${idx}, Значение: ${val}">${val}</span>`;
                    }).join('')}
                </div>
                <div class="debug-info">
                    <button onclick="window.showDebugLogs()">Показать отладочную информацию</button>
                </div>
            `;
            resultsContainer.classList.add('active');
        } catch (error) {
            logDebug(`Произошла ошибка: ${error.toString()}`);
            
            resultsContainer.innerHTML = `
                <h3>Ошибка:</h3>
                <p>${error.message}</p>
                <p>Проверьте работу сервера API и обновите страницу.</p>
                <div class="debug-info">
                    <button onclick="window.showDebugLogs()">Показать отладочную информацию</button>
                </div>
            `;
            resultsContainer.classList.add('active');
        } finally {
            // Reset button state
            calculateBtn.disabled = false;
            calculateBtn.textContent = 'Вычислить';
        }
    });
});

// Validate all pairs and enable/disable the calculate button
function validatePairs() {
    const nSelect = document.getElementById('n-select');
    const calculateBtn = document.getElementById('calculate-btn');
    
    const n = parseInt(nSelect.value);
    if (!n) {
        calculateBtn.disabled = true;
        return;
    }
    
    let allValid = true;
    for (let i = 0; i < n; i++) {
        const firstElement = document.getElementById(`pair-${i}-first`);
        const secondElement = document.getElementById(`pair-${i}-second`);
        
        if (!firstElement || !secondElement) {
            allValid = false;
            break;
        }
    }
    
    calculateBtn.disabled = !allValid;
} 