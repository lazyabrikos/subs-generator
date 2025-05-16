document.addEventListener('DOMContentLoaded', () => {
    const nSelect = document.getElementById('n-select');
    const pairsContainer = document.getElementById('pairs-container');
    const calculateBtn = document.getElementById('calculate-btn');
    const resultsContainer = document.getElementById('results-container');

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

        // Collect selected pairs
        const combinations = [];
        let pairsText = '';
        for (let i = 0; i < n; i++) {
            const first = parseInt(document.getElementById(`pair-${i}-first`).value);
            const second = parseInt(document.getElementById(`pair-${i}-second`).value);
            combinations.push([first, second]);
            pairsText += `(${first}, ${second})${i < n - 1 ? ', ' : ''}`;
        }

        try {
            // Show loading state
            calculateBtn.disabled = true;
            calculateBtn.textContent = 'Вычисление...';
            
            // Send POST request to C++ backend server on port 8081
            const apiUrl = `http://${window.location.hostname}:8081/api/calculate`;
            const response = await fetch(apiUrl, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    combinations: combinations
                })
            });

            if (!response.ok) {
                throw new Error('Ошибка сервера');
            }

            const results = await response.json();
            
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
            `;
            resultsContainer.classList.add('active');
        } catch (error) {
            resultsContainer.innerHTML = `
                <h3>Ошибка:</h3>
                <p>${error.message}</p>
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