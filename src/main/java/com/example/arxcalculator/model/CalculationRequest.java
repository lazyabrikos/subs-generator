package com.example.arxcalculator.model;

import lombok.Data;
import java.util.List;

@Data
public class CalculationRequest {
    private List<List<Integer>> combinations;
    
    public List<List<Integer>> getCombinations() {
        return combinations;
    }
} 