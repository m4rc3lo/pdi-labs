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
