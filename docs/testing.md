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

Esses testes validam estado, limites, teclado, mouse e capacidades sem criar
janelas.

## Diagnósticos manuais

Os executáveis `pdi_ui_test_*` devem ser iniciados manualmente. Eles não são
registrados no CTest e podem exigir um desktop local e um backend HighGUI.
