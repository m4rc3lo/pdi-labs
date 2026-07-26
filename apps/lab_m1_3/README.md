# Laboratório M1.3 — Aplicação integrada

## Uso

```bash
lab_m1_3 <entrada> <diretório-saída> \
    --operation <operação> \
    [--border <copy|replicate>] \
    [--factor <valor>] \
    [--show] \
    [--save-data]
```

Operações:

```text
mean3
weighted3
mean5
laplacian4
laplacian8
sobel
```

A estratégia padrão é `replicate`. O fator padrão do Laplaciano é `1.0`.
A opção `--show` é opcional; sem ela, a aplicação funciona em modo headless.
`--save-data` preserva parâmetros, kernels e matrizes numéricas em YAML.

Exemplos:

```bash
./build/ucrt64-debug/lab_m1_3.exe \
    images/synthetic/m1_3/vertical_edge.pgm \
    images/output/m1_3/sobel \
    --operation sobel \
    --border replicate
```

```bash
./build/ucrt64-debug/lab_m1_3.exe \
    images/synthetic/m1_3/impulse_noise.pgm \
    images/output/m1_3/mean5 \
    --operation mean5 \
    --border copy
```

```bash
./build/ucrt64-debug/lab_m1_3.exe \
    images/synthetic/m1_3/square.pgm \
    images/output/m1_3/laplacian \
    --operation laplacian8 \
    --factor 0.5 \
    --show
```


## Rastreabilidade YAML

```bash
./build/ucrt64-debug/lab_m1_3.exe \
    images/synthetic/m1_3/vertical_edge.pgm \
    images/output/m1_3/sobel_data \
    --operation sobel \
    --border replicate \
    --save-data
```

Além dos PNGs, o comando produz `sobel_result.yml`. Os gradientes e magnitudes
continuam em `CV_64FC1`; não são convertidos para `uchar`.

Os nomes dos arquivos são:

```text
mean_3x3_result.yml
weighted_mean_3x3_result.yml
mean_5x5_result.yml
laplacian_4_result.yml
laplacian_8_result.yml
sobel_result.yml
```

YAML é recomendado para imagens pequenas, inspeção numérica e rastreabilidade.
