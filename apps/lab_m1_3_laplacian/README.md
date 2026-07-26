# Laboratório M1.3 — Laplaciano e realce

Execução:

```bash
./build/ucrt64-debug/lab_m1_3_laplacian.exe \
    images/input/example.png \
    images/output/m1_3_laplacian \
    1.0
```

O terceiro argumento é o fator de realce.

Saídas:

```text
laplacian_4_response_visualization.png
laplacian_4_enhanced.png
laplacian_8_response_visualization.png
laplacian_8_enhanced.png
```

As imagens de resposta são normalizações min-max para visualização. Elas não
substituem a resposta numérica `CV_64FC1`, que preserva valores negativos.
