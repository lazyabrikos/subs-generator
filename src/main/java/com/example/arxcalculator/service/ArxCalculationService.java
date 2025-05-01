package com.example.arxcalculator.service;

import org.springframework.stereotype.Service;
import java.util.ArrayList;
import java.util.List;

@Service
public class ArxCalculationService {

    public List<Integer> calculate(List<List<Integer>> combinations) {
        List<Integer> result = new ArrayList<>();

        for (int x_l = 0; x_l < 16; x_l++) {
            for (int x_r = 0; x_r < 16; x_r++) {
                int x_l_cur = x_l;
                int x_r_cur = x_r;

                for (List<Integer> combination : combinations) {
                    int r = combination.get(0);
                    int s = combination.get(1);
                    int[] nextValues = arxIteration(x_l_cur, x_r_cur, r, s);
                    x_l_cur = nextValues[0];
                    x_r_cur = nextValues[1];
                }

                int resultNumber = (x_l_cur << 4) | x_r_cur;
                result.add(resultNumber);
            }
        }

        return result;
    }

    private int[] arxIteration(int X_l, int X_r, int r, int s) {
        int X_r_rotated = rotateBits(X_r, r);
        int z = (X_l + X_r_rotated) & 0x0F; // Addition modulo 16
        int z_rotated = rotateBits(z, s);
        int X_r_next = (z_rotated ^ X_r) & 0x0F; // XOR operation
        int X_l_next = z;
        
        return new int[]{X_l_next, X_r_next};
    }

    private int rotateBits(int x, int k) {
        x = x & 0x0F; // Use only the lower 4 bits
        return ((x << k) | (x >> (4 - k))) & 0x0F; // Rotate and keep only lower 4 bits
    }
} 