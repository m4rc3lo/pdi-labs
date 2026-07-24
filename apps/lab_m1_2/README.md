# Laboratório M1.2 — Estudo preliminar aplicado

Este exemplo apresenta uma proposta preliminar para simplificar zonas de
iluminação em imagens de uma cena controlada.

O objetivo não é construir um sistema completo de segmentação. O exemplo serve
para verificar se técnicas já estudadas na M1 conseguem preservar regiões
escuras, intermediárias e claras em uma representação reduzida.

## Entrada e saída

Entrada:

- imagem colorida BGR;
- preferencialmente uma das imagens sintéticas em
  `images/synthetic/m1_2/`.

Saídas:

```text
grayscale_weighted.png
quantized_008_levels.png
quantized_004_levels.png
study_report.csv
```

## Critério preliminar de sucesso

O resultado de quatro níveis deve apresentar pelo menos três intensidades
distintas. Esse critério simples verifica se ainda existem zonas escuras,
intermediárias e claras após a redução.

## Compilação no MSYS2 UCRT64

```bash
cmake --preset ucrt64-debug
cmake --build --preset ucrt64-debug
```

## Execução headless

```bash
./build/ucrt64-debug/lab_m1_2.exe \
    images/synthetic/m1_2/illumination_shadow.ppm \
    images/output/m1_2
```

## Execução com janelas

```bash
./build/ucrt64-debug/lab_m1_2.exe \
    images/synthetic/m1_2/illumination_shadow.ppm \
    images/output/m1_2 \
    --show
```

O código de retorno é `0` quando o critério preliminar é atendido, `2` quando o
processamento termina mas o critério não é atendido, e `1` em caso de erro.
