document.addEventListener('DOMContentLoaded', () => {
    const nSelect = document.getElementById('n-select');
    const pairsContainer = document.getElementById('pairs-container');
    const calculateBtn = document.getElementById('calculate-btn');
    const resultsContainer = document.getElementById('results-container');
    const downloadCsvBtn = document.getElementById('download-csv-btn');

    let lastResultsData = null;

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

        const combinations = [];
        let pairsText = '';
        for (let i = 0; i < n; i++) {
            const first = parseInt(document.getElementById(`pair-${i}-first`).value);
            const second = parseInt(document.getElementById(`pair-${i}-second`).value);
            combinations.push([first, second]);
            pairsText += `(${first}, ${second})${i < n - 1 ? ', ' : ''}`;
        }

        try {
            calculateBtn.disabled = true;
            calculateBtn.textContent = 'Вычисление...';
            
            const apiUrl = '/api/calculate';
            
            const requestData = {
                combinations: combinations
            };
            
            const response = await fetch(apiUrl, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(requestData)
            });

            if (!response.ok) {
                throw new Error(`Ошибка сервера: ${response.status} ${response.statusText}`);
            }

            const responseText = await response.text();
            
            let responseData;
            try {
                responseData = JSON.parse(responseText);
            } catch (parseError) {
                throw new Error(`Ошибка при обработке ответа: ${parseError.message}`);
            }
            
            lastResultsData = responseData; // Store the results
            displayResults(responseData);
        } catch (error) {
            resultsContainer.innerHTML = `
                <h3>Ошибка:</h3>
                <p>${error.message}</p>
                <p>Проверьте работу сервера API и обновите страницу.</p>
            `;
            resultsContainer.classList.add('active');
        } finally {
            calculateBtn.disabled = false;
            calculateBtn.textContent = 'Вычислить';
        }
    });

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

    function downloadCSV() {
        if (!lastResultsData) {
            console.error("No results data available to download.");
            return;
        }

        const substitution = lastResultsData.substitution;
        const characteristics = lastResultsData.characteristics;
        
        const substitutionString = substitution.join(',');
        
        const diffChar = characteristics.diffCharacteristic;
        const linearChar = characteristics.linearCharacteristic;
        const nonlinearDeg = characteristics.nonlinearDegree;
        
        let csvContent = "Substitution;DiffCharacteristic;LinearCharacteristic;NonlinearDegree\n";
        csvContent += `${substitutionString};${diffChar};${linearChar};${nonlinearDeg}\n`;
        
        const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
        const link = document.createElement("a");
        const url = URL.createObjectURL(blob);
        
        link.setAttribute("href", url);
        link.setAttribute("download", "arx_substitution_characteristics.csv"); 
        link.style.visibility = 'hidden';
        
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
    }

    downloadCsvBtn.addEventListener('click', downloadCSV);
});

function displayResults(data) {
    const resultsContainer = document.getElementById('results-container');
    const downloadCsvBtn = document.getElementById('download-csv-btn');
    
    resultsContainer.innerHTML = '';
    
    const substitution = data.substitution;
    const characteristics = data.characteristics;
    
    const substitutionDiv = document.createElement('div');
    substitutionDiv.innerHTML = '<h3>Подстановка:</h3>';
    
    const grid = document.createElement('div');
    grid.className = 'results-grid';
    
    substitution.forEach(value => {
        const span = document.createElement('span');
        span.textContent = value;
        grid.appendChild(span);
    });
    
    substitutionDiv.appendChild(grid);
    resultsContainer.appendChild(substitutionDiv);
    
    const characteristicsDiv = document.createElement('div');
    characteristicsDiv.innerHTML = `
        <h3>Характеристики:</h3>
        <p>Разностная характеристика: ${characteristics.diffCharacteristic}</p>
        <p>Линейная характеристика: ${characteristics.linearCharacteristic}</p>
        <p>Степень нелинейности: ${characteristics.nonlinearDegree}</p>
    `;
    
    resultsContainer.appendChild(characteristicsDiv);
    resultsContainer.style.display = 'block';
    
    downloadCsvBtn.style.display = 'inline-block';
} 