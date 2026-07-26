# Testes

## Suíte padrão

```bash
ctest --test-dir build/ucrt64-debug --output-on-failure
```

A suíte padrão não abre janelas nem aguarda interação.

## Testes opcionais de estado e eventos

Com o build interativo configurado:

```bash
ctest \
    --test-dir build/ucrt64-debug-ui \
    --tests-regex 'ui.unit' \
    --output-on-failure
```

Esses testes validam estado, limites, teclado, mouse, capacidades e a política
universal de fechamento sem criar janelas. Também verificam que o fechamento
ocorre antes do callback e que a ausência de callback não impede a saída.

## Diagnósticos manuais

Os executáveis `pdi_ui_test_*` devem ser iniciados manualmente. Eles não são
registrados no CTest e podem exigir um desktop local e um backend HighGUI.

Use `pdi_ui_test_window_close` para verificar `Esc`, `q`, `Q` e o botão `X`.
O processo deve terminar sem `Ctrl+C` ou reinicialização do terminal.
