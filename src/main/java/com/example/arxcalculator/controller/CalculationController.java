package com.example.arxcalculator.controller;

import com.example.arxcalculator.model.CalculationRequest;
import com.example.arxcalculator.service.ArxCalculationService;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*") // Разрешаем CORS для всех источников
public class CalculationController {

    private final ArxCalculationService calculationService;

    public CalculationController(ArxCalculationService calculationService) {
        this.calculationService = calculationService;
    }

    @PostMapping("/calculate")
    public List<Integer> calculate(@RequestBody CalculationRequest request) {
        return calculationService.calculate(request.getCombinations());
    }
} 