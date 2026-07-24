# Laboratório M1.1 — Representação e acesso a pixels

Este executável integra todas as atividades do Laboratório M1.1:

- inspeção estrutural e estatística;
- cópia manual;
- separação dos canais B, G e R;
- conversão para níveis de cinza por média simples;
- conversão para níveis de cinza por média ponderada;
- quantização da imagem ponderada para 16, 8, 4 e 2 níveis;
- persistência dos resultados;
- exibição opcional com OpenCV HighGUI.

## Compilação no MSYS2 UCRT64

Na raiz do repositório:

```bash
cmake --preset ucrt64-debug
cmake --build --preset ucrt64-debug
```

## Execução headless

```bash
./build/ucrt64-debug/lab_m1_1.exe \
    images/input/sample.png \
    images/output/m1_1
```

O modo padrão não abre janelas, sendo adequado para terminais remotos e
integração contínua.

## Execução com janelas

```bash
./build/ucrt64-debug/lab_m1_1.exe \
    images/input/sample.png \
    images/output/m1_1 \
    --show
```

Todas as janelas permanecem abertas até que uma tecla seja pressionada.

## Arquivos produzidos

```text
inspection_report.csv
manual_copy.png
channel_blue.png
channel_green.png
channel_red.png
grayscale_average.png
grayscale_weighted.png
quantized_016_levels.png
quantized_008_levels.png
quantized_004_levels.png
quantized_002_levels.png
```

A quantização é aplicada sobre `grayscale_weighted.png`, pois esse método
aproxima melhor a luminância percebida que a média aritmética.
