# Laboratório M1.3 — Filtros de suavização

O exemplo aplica três filtros sobre uma imagem em níveis de cinza:

- média uniforme `3 x 3`;
- média ponderada `3 x 3`;
- média uniforme `5 x 5`.

Todos usam `SpatialConvolution` e `ReplicateBorder`.

## Compilação

```bash
cmake --preset ucrt64-debug
cmake --build --preset ucrt64-debug
```

## Execução reproduzível

```bash
./build/ucrt64-debug/lab_m1_3_smoothing.exe \
    images/input/example.png \
    images/output/m1_3_smoothing
```

Com janelas:

```bash
./build/ucrt64-debug/lab_m1_3_smoothing.exe \
    images/input/example.png \
    images/output/m1_3_smoothing \
    --show
```

Saídas:

```text
mean_3x3.png
weighted_mean_3x3.png
mean_5x5.png
```
