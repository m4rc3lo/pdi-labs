# pdi-labs {#mainpage}

`pdi-labs` reúne implementações didáticas em C++ e OpenCV para os
laboratórios da disciplina de Processamento de Imagens.

A documentação combina:

- páginas narrativas em Markdown;
- referência das APIs públicas;
- código-fonte navegável;
- relações entre classes e arquivos;
- diagramas gerados pelo Graphviz, quando a ferramenta estiver disponível.

## Escopo atual

Nesta etapa, a documentação apresenta a infraestrutura do projeto e a classe
\ref pdi::core::ProjectInfo como exemplo do padrão documental. Os algoritmos
dos laboratórios serão adicionados em incrementos posteriores.

## Diretórios documentados

| Diretório | Conteúdo |
|---|---|
| `include/` | interfaces e tipos públicos |
| `src/` | implementações |
| `apps/` | programas executáveis e exemplos |
| `docs/` | documentação narrativa em Markdown |

O diretório `legacy/` permanece fora da documentação gerada porque representa
o código histórico anterior à reorganização do projeto.

## Geração da documentação

No MSYS2 UCRT64:

```bash
cmake -S . -B build/ucrt64-docs \
    -G Ninja \
    -DPDI_BUILD_DOCS=ON \
    -DPDI_BUILD_TESTS=OFF

cmake --build build/ucrt64-docs --target docs
```

No Linux:

```bash
cmake -S . -B build/linux-docs \
    -G Ninja \
    -DPDI_BUILD_DOCS=ON \
    -DPDI_BUILD_TESTS=OFF

cmake --build build/linux-docs --target docs
```

A página inicial será gerada em:

```text
build/<configuração>/docs/html/index.html
```

A geração exige Doxygen. Graphviz é opcional: quando `dot` não estiver
disponível, a documentação HTML ainda será gerada, mas sem os diagramas de
dependência e relacionamento.

## Padrão de comentários Doxygen

As APIs públicas devem usar blocos `/** ... */`. A documentação deve explicar
o contrato da interface, e não apenas repetir seu nome.

### Tags principais

| Tag | Uso |
|---|---|
| `@file` | identifica e descreve a finalidade de um arquivo |
| `@brief` | apresenta um resumo curto |
| `@details` | fornece explicação complementar |
| `@param` | documenta parâmetros |
| `@return` | documenta o valor retornado |
| `@throws` | registra exceções |
| `@pre` | informa pré-condições |
| `@post` | informa pós-condições |
| `@note` | destaca uma observação |
| `@warning` | destaca risco, restrição ou uso incorreto |
| `@see` | cria referência para elemento relacionado |
| `@code` e `@endcode` | delimitam um exemplo de código |

### Exemplo

```cpp
/**
 * @file project_info.hpp
 * @brief Declares access to build and dependency metadata.
 */

namespace pdi::core {

/**
 * @brief Provides immutable metadata about the configured project.
 *
 * @details The returned views refer to strings with static storage duration.
 * No allocation is performed by the accessor methods.
 *
 * @note This class is used as the initial documentation example.
 */
class ProjectInfo {
public:
    /**
     * @brief Returns the configured semantic version.
     *
     * @return Project version with static storage duration.
     *
     * @post The returned view is valid for the entire process lifetime.
     */
    [[nodiscard]] static std::string_view project_version() noexcept;
};

} // namespace pdi::core
```

## Convenções documentais

- escrever resumos objetivos em `@brief`;
- usar `@details` quando houver decisões técnicas relevantes;
- documentar todos os parâmetros e retornos da API pública;
- registrar exceções realmente lançadas;
- usar `@warning` apenas para riscos ou restrições importantes;
- evitar comentários que apenas traduzam literalmente o código;
- manter exemplos pequenos e compiláveis quando possível;
- atualizar a documentação junto com a alteração da API.
