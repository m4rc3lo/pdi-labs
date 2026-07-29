# Atribuições e licenças de imagens

Este documento registra autoria, origem, atribuições e observações de licença
para as imagens públicas do projeto `pdi-labs`.

Salvo indicação contrária, os arquivos **incluídos neste repositório** são
distribuídos sob a Apache License 2.0. Quando houver material de terceiros, a
attribuição e a ressalva correspondente serão informadas explicitamente.

Os endereços privados gerados por plataformas externas **não são publicados**
neste repositório.

## 1. Fotografias próprias

As fotografias listadas abaixo são de autoria de **Marcelo Dornbusch Lopes** e
são distribuídas sob a **Apache License 2.0** no contexto deste projeto.

Arquivos incluídos:

- `foto_borboleta_flor_01.jpg`;
- `foto_broto_planta_01.jpg`;
- `foto_cachorro_bola_01.jpg`;
- `foto_mosca_orquidea_01.jpg`;
- `foto_passaro_comedouro_01.png`;
- `foto_passaro_comedouro_pb_01.png`;
- `foto_passaro_fibra_01.jpg`;
- `foto_pimenteira_frutos_01.png`.

Observações:

- a fotografia `foto_passaro_comedouro_pb_01.png` é uma fotografia própria
  originalmente em preto e branco;
- fotografias próprias podem ser usadas como entradas oficiais, ilustrações
  didáticas e exemplos de resultados processados.

## 2. Imagens realistas geradas por IA

As imagens realistas geradas por IA foram produzidas com direção humana para
uso didático no projeto.

Metadados gerais do conjunto atual:

- **plataforma**: Genspark;
- **modelo**: nano-banana-2;
- **data de geração**: 2026-07-28;
- **finalidade**: compor entradas didáticas para os laboratórios M1.1 a M2.3.

Arquivos incluídos:

- `m1_1_a_rgb_quantizacao.png`;
- `m1_1_b_rgb_quantizacao.png`;
- `m1_2_a_dominio_valor.png`;
- `m1_2_b_dominio_valor.png`;
- `m1_3_a_espacial_bordas.png`;
- `m1_3_b_espacial_bordas.png`;
- `m2_1_a_segmentacao.png`;
- `m2_1_b_segmentacao.png`;
- `m2_2_a_componentes_conexos.png`;
- `m2_2_b_componentes_conexos.png`;
- `m2_3_a_morfologia.png`;
- `m2_3_b_morfologia.png`.

Observações:

- esses arquivos são distribuídos sob a Apache License 2.0 no contexto do
  repositório, salvo indicação contrária futura;
- os endereços privados vinculados à conta da plataforma de geração não são
  publicados;
- as imagens realistas geradas por IA **não** devem ser descritas como
  fotografias.

## 3. Imagens de terceiro — OpenCV

Os arquivos abaixo foram redistribuídos a partir de material público do
**OpenCV**, exclusivamente para fins didáticos. Eles **não** são de autoria do
projeto `pdi-labs`.

### `opencv_adaptive_threshold_sudoku.jpg`

- **categoria**: imagem de terceiro;
- **projeto de origem**: OpenCV;
- **página pública**: <https://docs.opencv.org/4.x/d7/d4d/tutorial_py_thresholding.html>;
- **licença declarada pelo projeto de origem**: Apache License 2.0;
- **atribuição**: contribuidores do OpenCV;
- **modificações locais**: renomeação e reorganização do arquivo no
  repositório; nenhuma alteração visual declarada.

### `opencv_morphology_j_binary.png`

- **categoria**: imagem de terceiro;
- **projeto de origem**: OpenCV;
- **página pública**: <https://docs.opencv.org/4.x/d9/d61/tutorial_py_morphological_ops.html>;
- **licença declarada pelo projeto de origem**: Apache License 2.0;
- **atribuição**: contribuidores do OpenCV;
- **modificações locais**: renomeação e reorganização do arquivo no
  repositório; nenhuma alteração visual declarada.

## 4. Resultados curados e imagens derivadas

Os resultados produzidos pelos executáveis do projeto podem ser selecionados e
versionados em `docs/images/results/` como **resultados curados**.

Regras:

- a seleção é manual;
- a finalidade é didática;
- a geração automatizada completa fica para incremento futuro;
- a existência de um resultado curado não altera a atribuição da imagem de
  entrada correspondente.

## 5. Ressalva geral

Este arquivo complementa o `NOTICE`, mas não substitui a necessidade de
respeitar licenças, avisos e atribuições de materiais de terceiros. Sempre que
novas imagens externas forem incorporadas, suas informações de origem deverão
ser registradas aqui.
