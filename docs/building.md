# Compilação

## Build padrão

O build padrão não inclui `pdi::ui`:

```bash
cmake --preset ucrt64-debug
cmake --build --preset ucrt64-debug
```

## Build interativo

```bash
cmake -S . -B build/ucrt64-debug-ui \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPDI_BUILD_INTERACTIVE_UI=ON \
    -DPDI_BUILD_INTERACTIVE_TESTS=ON
```

```bash
cmake --build build/ucrt64-debug-ui
```

`PDI_BUILD_INTERACTIVE_TESTS` só produz targets quando
`PDI_BUILD_INTERACTIVE_UI` também está habilitado.

O OpenCV precisa possuir um backend HighGUI para abrir janelas. Qt não é uma
dependência direta: botões e checkboxes são ativados apenas quando o backend
reporta suporte.


## Diagnóstico do ciclo de vida das janelas

Com os dois recursos interativos habilitados:

```bash
./build/ucrt64-debug-ui/pdi_ui_test_window_close.exe
```

Feche por `Esc`, `q`, `Q` ou pelo botão `X`. As teclas devem ser pressionadas
com a janela HighGUI em foco. O backend Qt é opcional.

## Documentação Doxygen local

A documentação pode ser configurada sem testes, exemplos ou interfaces
interativas. Isso reduz o escopo do build sem alterar o funcionamento dos builds
padrão e interativo:

```bash
cmake -S . -B build/ucrt64-docs \
    -G Ninja \
    -DPDI_BUILD_DOCS=ON \
    -DPDI_BUILD_TESTS=OFF \
    -DPDI_BUILD_EXAMPLES=OFF \
    -DPDI_BUILD_INTERACTIVE_UI=OFF \
    -DPDI_BUILD_INTERACTIVE_TESTS=OFF
```

```bash
cmake --build build/ucrt64-docs --target docs
```

O `Doxyfile` configurado pelo CMake define:

```text
OUTPUT_DIRECTORY = build/ucrt64-docs/docs
HTML_OUTPUT       = html
```

Portanto, a raiz publicável é:

```text
build/ucrt64-docs/docs/html/
```

Ela deve conter diretamente:

```text
build/ucrt64-docs/docs/html/index.html
```

Essa distinção é importante: o artefato do GitHub Pages deve receber o
**conteúdo de `html/` como raiz**, e não os diretórios `docs/` ou
`build/ucrt64-docs/` completos.

Doxygen é obrigatório. Graphviz é opcional no build local, mas deve estar
instalado para gerar os diagramas de dependência, colaboração e hierarquia.

## Publicação com GitHub Actions

O workflow `.github/workflows/pages.yml` automatiza exclusivamente a geração e
a publicação da documentação estática. Ele não compila exemplos, não executa
testes, não habilita interfaces gráficas, não depende de Qt e não versiona o
HTML gerado.

### Gatilhos

O workflow é executado:

- automaticamente, em `push` para `main`;
- manualmente, por `workflow_dispatch` na aba **Actions**.

### Build no runner Ubuntu

O job `build`:

1. obtém o repositório com `actions/checkout`;
2. instala CMake, Ninja, OpenCV, Doxygen e Graphviz;
3. configura os metadados do Pages com `actions/configure-pages`;
4. configura o CMake em `build/pages-docs/`;
5. gera somente o target `docs`;
6. verifica a existência de `build/pages-docs/docs/html/index.html`;
7. envia `build/pages-docs/docs/html/` com
   `actions/upload-pages-artifact`.

As opções utilizadas são:

```text
PDI_BUILD_DOCS=ON
PDI_BUILD_TESTS=OFF
PDI_BUILD_EXAMPLES=OFF
PDI_BUILD_INTERACTIVE_UI=OFF
PDI_BUILD_INTERACTIVE_TESTS=OFF
```

### Implantação

O job `deploy` depende do término do job `build`, utiliza o ambiente
`github-pages` e publica o artefato com `actions/deploy-pages`.

O token automático recebe somente:

```yaml
contents: read
pages: write
id-token: write
```

Permissões não declaradas ficam desabilitadas. O workflow não cria release,
tag, branch de publicação ou commit com arquivos HTML.

## Habilitação inicial do GitHub Pages

Depois que o workflow estiver no `main` remoto:

1. abra o repositório no GitHub;
2. selecione **Settings**;
3. no menu lateral, abra **Pages**, dentro de **Code and automation**;
4. em **Build and deployment**, selecione **GitHub Actions** em **Source**;
5. abra a aba **Actions**;
6. selecione **Publicar documentação Doxygen**;
7. aguarde a execução iniciada pelo `push` ou use **Run workflow** para uma
   execução manual;
8. confirme que os jobs **Gerar documentação** e **Publicar GitHub Pages**
   terminaram com sucesso;
9. consulte a URL fornecida pelo job de implantação ou por **Settings > Pages**.

A URL pública não deve ser presumida nem registrada antes da primeira
implantação bem-sucedida. Depois que ela existir, poderá ser adicionada ao
`README.md` e à página inicial em um incremento documental separado.

## Diagnóstico do workflow

Quando o build falhar, abra o job **Gerar documentação** e identifique a etapa
que falhou. Verificações especialmente úteis:

- **Instalar dependências**: disponibilidade dos pacotes do Ubuntu;
- **Configurar documentação com CMake**: localização do OpenCV e do Doxygen;
- **Gerar documentação Doxygen**: erros do Doxygen ou Graphviz;
- **Validar raiz do site**: ausência de `index.html` no caminho esperado;
- **Enviar artefato do GitHub Pages**: caminho ou conteúdo inválido;
- **Publicar documentação**: Pages ainda não habilitado ou restrição do
  ambiente `github-pages`.

O build local continua sendo a primeira forma de reproduzir problemas de
Doxygen antes de revisar o workflow.
