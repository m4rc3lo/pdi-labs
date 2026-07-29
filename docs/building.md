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
4. executa `scripts/prepare_doxygen_sources.py` e cria
   `build/pages-source/docs/`;
5. confirma que `docs/` ainda contém Mermaid e que a cópia temporária não
   contém blocos Mermaid;
6. configura o CMake em `build/pages-docs/`, apontando
   `PDI_DOCUMENTATION_SOURCE_DIR` para a cópia temporária;
7. gera somente o target `docs`;
8. verifica `index.html`, imagens publicáveis e caminhos relativos;
9. envia `build/pages-docs/docs/html/` com
   `actions/upload-pages-artifact`.

As opções utilizadas são:

```text
PDI_BUILD_DOCS=ON
PDI_BUILD_TESTS=OFF
PDI_BUILD_EXAMPLES=OFF
PDI_BUILD_INTERACTIVE_UI=OFF
PDI_BUILD_INTERACTIVE_TESTS=OFF
PDI_DOCUMENTATION_SOURCE_DIR=<repositório>/build/pages-source/docs
```

### Duas representações documentais

`docs/` é a fonte canônica. Seus blocos Mermaid continuam versionados e são
renderizados diretamente pelo GitHub e por visualizadores Markdown
compatíveis. O site Doxygen usa uma representação derivada e efêmera dentro de
`build/`:

```text
docs/                         fonte canônica com Mermaid
build/pages-source/docs/      cópia temporária sem Mermaid cru
build/pages-docs/docs/html/   site estático publicável
```

O script substitui cada bloco Mermaid por uma nota que aponta para o documento
original no GitHub e troca somente os prefixos de galeria necessários ao
artefato HTML. Nenhum arquivo da fonte canônica é reescrito. A variável CMake
`PDI_DOCUMENTATION_SOURCE_DIR` permite escolher explicitamente qual conjunto de
Markdown será processado pelo Doxygen; quando ela não é informada, o build local
continua usando `docs/`.

Para reproduzir localmente a variante do Pages:

```bash
python3 scripts/prepare_doxygen_sources.py \
    --source docs \
    --destination build/pages-source/docs \
    --repository-docs-url \
    https://github.com/m4rc3lo/pdi-labs/blob/main/docs
```

```bash
cmake -S . -B build/pages-docs \
    -G Ninja \
    -DPDI_BUILD_DOCS=ON \
    -DPDI_BUILD_TESTS=OFF \
    -DPDI_BUILD_EXAMPLES=OFF \
    -DPDI_BUILD_INTERACTIVE_UI=OFF \
    -DPDI_BUILD_INTERACTIVE_TESTS=OFF \
    -DPDI_DOCUMENTATION_SOURCE_DIR="$PWD/build/pages-source/docs"
cmake --build build/pages-docs --target docs
```

As imagens são copiadas tanto para a estrutura histórica usada pelo build local
quanto para `docs/html/images/`, que pertence à raiz do artefato publicado. A
cópia temporária usa caminhos `images/input/...` e `images/results/...`, sem
subir para fora de `/pdi-labs/`.

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

A implantação validada está disponível em
[https://m4rc3lo.github.io/pdi-labs/](https://m4rc3lo.github.io/pdi-labs/).

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
