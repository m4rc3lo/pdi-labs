#!/usr/bin/env python3
"""Prepara uma cópia temporária dos Markdown para publicação com Doxygen.

A fonte canônica em docs/ permanece inalterada e conserva diagramas Mermaid.
Na cópia temporária, blocos Mermaid são substituídos por uma nota com link para
uma versão renderizada pelo GitHub. Caminhos de galerias também são ajustados
para que os recursos fiquem dentro da raiz HTML publicada.
"""

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


MERMAID_OPENING = "```mermaid"
MERMAID_CLOSING = "```"
IMAGE_PATH_REPLACEMENTS = {
    "../../images/input/": "images/input/",
    "../../images/results/": "images/results/",
    "../images/input/": "images/input/",
    "../images/results/": "images/results/",
}


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Prepara fontes Markdown compatíveis com o site Doxygen."
    )
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--destination", required=True, type=Path)
    parser.add_argument(
        "--repository-docs-url",
        required=True,
        help="URL-base de docs/ no GitHub, sem barra final.",
    )
    return parser.parse_args()


def mermaid_note(relative_path: Path, repository_docs_url: str) -> list[str]:
    document_url = f"{repository_docs_url.rstrip('/')}/{relative_path.as_posix()}"
    return [
        "> **Diagrama Mermaid disponível na versão Markdown.**\n",
        f"> Consulte [este documento no GitHub]({document_url}) para visualizar "
        "o diagrama renderizado.\n",
    ]


def transform_markdown(
    source_file: Path,
    destination_file: Path,
    relative_path: Path,
    repository_docs_url: str,
) -> int:
    lines = source_file.read_text(encoding="utf-8").splitlines(keepends=True)
    output: list[str] = []
    inside_mermaid = False
    diagram_count = 0

    for line_number, line in enumerate(lines, start=1):
        stripped = line.strip()

        if not inside_mermaid and stripped == MERMAID_OPENING:
            inside_mermaid = True
            diagram_count += 1
            output.extend(mermaid_note(relative_path, repository_docs_url))
            continue

        if inside_mermaid:
            if stripped == MERMAID_CLOSING:
                inside_mermaid = False
            continue

        transformed = line
        for original, replacement in IMAGE_PATH_REPLACEMENTS.items():
            transformed = transformed.replace(original, replacement)
        output.append(transformed)

    if inside_mermaid:
        raise ValueError(
            f"Bloco Mermaid não terminado em {source_file} "
            f"(detectado até a linha {line_number})."
        )

    destination_file.parent.mkdir(parents=True, exist_ok=True)
    destination_file.write_text("".join(output), encoding="utf-8", newline="\n")
    return diagram_count


def main() -> int:
    arguments = parse_arguments()
    source = arguments.source.resolve()
    destination = arguments.destination.resolve()

    if not source.is_dir():
        raise FileNotFoundError(f"Diretório-fonte não encontrado: {source}")
    if destination == source or source in destination.parents:
        raise ValueError("O destino não pode ser igual ou interno ao diretório-fonte.")

    if destination.exists():
        shutil.rmtree(destination)
    shutil.copytree(source, destination)

    total_diagrams = 0
    markdown_files = sorted(source.rglob("*.md"))
    for source_file in markdown_files:
        relative_path = source_file.relative_to(source)
        destination_file = destination / relative_path
        total_diagrams += transform_markdown(
            source_file,
            destination_file,
            relative_path,
            arguments.repository_docs_url,
        )

    print(f"Markdown processados: {len(markdown_files)}")
    print(f"Diagramas Mermaid substituídos: {total_diagrams}")
    print(f"Fonte temporária: {destination}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
