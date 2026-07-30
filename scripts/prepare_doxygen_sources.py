#!/usr/bin/env python3
"""Prepara uma fonte Markdown curada para publicação com Doxygen.

A fonte canônica em ``docs/`` permanece inalterada e conserva diagramas
Mermaid e documentos internos. Um manifesto explícito seleciona os arquivos e
diretórios públicos. Na cópia temporária, blocos Mermaid são substituídos por
uma nota com link para o GitHub, caminhos de galerias são adaptados e títulos
formados apenas por código inline são normalizados para o Doxygen.
"""

from __future__ import annotations

import argparse
import re
import shutil
from dataclasses import dataclass
from pathlib import Path, PurePosixPath


MERMAID_OPENING = "```mermaid"
MERMAID_CLOSING = "```"
HEADING_CODE_PATTERN = re.compile(r"^(#{1,6})[ \t]+`([^`\r\n]+)`[ \t]*$")
IMAGE_PATH_REPLACEMENTS = {
    "../../images/input/": "images/input/",
    "../../images/results/": "images/results/",
    "../images/input/": "images/input/",
    "../images/results/": "images/results/",
}


@dataclass(frozen=True)
class ManifestEntry:
    source: PurePosixPath
    destination: PurePosixPath


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Prepara fontes públicas compatíveis com o site Doxygen."
    )
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--destination", required=True, type=Path)
    parser.add_argument("--manifest", required=True, type=Path)
    parser.add_argument(
        "--repository-docs-url",
        required=True,
        help="URL-base de docs/ no GitHub, sem barra final.",
    )
    return parser.parse_args()


def validate_relative_path(value: str, *, field_name: str, line_number: int) -> PurePosixPath:
    path = PurePosixPath(value.strip())
    if not value.strip() or path.is_absolute() or ".." in path.parts:
        raise ValueError(
            f"{field_name} inválido na linha {line_number} do manifesto: {value!r}"
        )
    return path


def load_manifest(manifest_file: Path) -> list[ManifestEntry]:
    entries: list[ManifestEntry] = []
    seen_sources: set[PurePosixPath] = set()
    seen_destinations: set[PurePosixPath] = set()

    for line_number, raw_line in enumerate(
        manifest_file.read_text(encoding="utf-8").splitlines(), start=1
    ):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue

        parts = line.split("->")
        if len(parts) > 2:
            raise ValueError(
                f"Mapeamento inválido na linha {line_number} do manifesto: {raw_line!r}"
            )

        source_path = validate_relative_path(
            parts[0], field_name="Origem", line_number=line_number
        )
        destination_path = validate_relative_path(
            parts[1] if len(parts) == 2 else parts[0],
            field_name="Destino",
            line_number=line_number,
        )

        if source_path in seen_sources:
            raise ValueError(f"Origem duplicada no manifesto: {source_path}")
        if destination_path in seen_destinations:
            raise ValueError(f"Destino duplicado no manifesto: {destination_path}")

        seen_sources.add(source_path)
        seen_destinations.add(destination_path)
        entries.append(ManifestEntry(source_path, destination_path))

    if not entries:
        raise ValueError(f"Manifesto sem entradas públicas: {manifest_file}")
    return entries


def mermaid_note(relative_path: PurePosixPath, repository_docs_url: str) -> list[str]:
    document_url = f"{repository_docs_url.rstrip('/')}/{relative_path.as_posix()}"
    return [
        "> **Diagrama Mermaid disponível na versão Markdown.**\n",
        f"> Consulte [este documento no GitHub]({document_url}) para visualizar "
        "o diagrama renderizado.\n",
    ]


def transform_markdown(
    source_file: Path,
    destination_file: Path,
    source_relative_path: PurePosixPath,
    repository_docs_url: str,
) -> tuple[int, int]:
    lines = source_file.read_text(encoding="utf-8").splitlines(keepends=True)
    output: list[str] = []
    inside_mermaid = False
    diagram_count = 0
    normalized_heading_count = 0
    last_line_number = 0

    for line_number, line in enumerate(lines, start=1):
        last_line_number = line_number
        stripped = line.strip()

        if not inside_mermaid and stripped == MERMAID_OPENING:
            inside_mermaid = True
            diagram_count += 1
            output.extend(mermaid_note(source_relative_path, repository_docs_url))
            continue

        if inside_mermaid:
            if stripped == MERMAID_CLOSING:
                inside_mermaid = False
            continue

        transformed = line
        for original, replacement in IMAGE_PATH_REPLACEMENTS.items():
            transformed = transformed.replace(original, replacement)

        heading_match = HEADING_CODE_PATTERN.match(transformed.rstrip("\r\n"))
        if heading_match:
            transformed = f"{heading_match.group(1)} {heading_match.group(2)}\n"
            normalized_heading_count += 1

        output.append(transformed)

    if inside_mermaid:
        raise ValueError(
            f"Bloco Mermaid não terminado em {source_file} "
            f"(detectado até a linha {last_line_number})."
        )

    destination_file.parent.mkdir(parents=True, exist_ok=True)
    destination_file.write_text("".join(output), encoding="utf-8", newline="\n")
    return diagram_count, normalized_heading_count


def copy_manifest_entry(
    source_root: Path,
    destination_root: Path,
    entry: ManifestEntry,
    repository_docs_url: str,
) -> tuple[int, int, int, int]:
    source_path = source_root.joinpath(*entry.source.parts)
    destination_path = destination_root.joinpath(*entry.destination.parts)

    if not source_path.exists():
        raise FileNotFoundError(f"Entrada do manifesto não encontrada: {source_path}")

    markdown_count = 0
    diagram_count = 0
    heading_count = 0
    copied_count = 0

    if source_path.is_dir():
        files = sorted(path for path in source_path.rglob("*") if path.is_file())
        for nested_source in files:
            nested_relative = nested_source.relative_to(source_path)
            nested_destination = destination_path / nested_relative
            canonical_relative = PurePosixPath(
                *nested_source.relative_to(source_root).parts
            )
            if nested_source.suffix.lower() == ".md":
                diagrams, headings = transform_markdown(
                    nested_source,
                    nested_destination,
                    canonical_relative,
                    repository_docs_url,
                )
                markdown_count += 1
                diagram_count += diagrams
                heading_count += headings
            else:
                nested_destination.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(nested_source, nested_destination)
            copied_count += 1
    elif source_path.suffix.lower() == ".md":
        diagrams, headings = transform_markdown(
            source_path,
            destination_path,
            entry.source,
            repository_docs_url,
        )
        markdown_count = 1
        diagram_count = diagrams
        heading_count = headings
        copied_count = 1
    else:
        destination_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source_path, destination_path)
        copied_count = 1

    return markdown_count, diagram_count, heading_count, copied_count


def prepare_sources(
    source: Path,
    destination: Path,
    manifest: Path,
    repository_docs_url: str,
) -> dict[str, int]:
    source = source.resolve()
    destination = destination.resolve()
    manifest = manifest.resolve()

    if not source.is_dir():
        raise FileNotFoundError(f"Diretório-fonte não encontrado: {source}")
    if not manifest.is_file():
        raise FileNotFoundError(f"Manifesto não encontrado: {manifest}")
    if destination == source or source in destination.parents:
        raise ValueError("O destino não pode ser igual ou interno ao diretório-fonte.")

    entries = load_manifest(manifest)
    if destination.exists():
        shutil.rmtree(destination)
    destination.mkdir(parents=True)

    totals = {
        "manifest_entries": len(entries),
        "markdown_files": 0,
        "diagrams": 0,
        "headings": 0,
        "copied_files": 0,
    }
    for entry in entries:
        markdown, diagrams, headings, copied = copy_manifest_entry(
            source, destination, entry, repository_docs_url
        )
        totals["markdown_files"] += markdown
        totals["diagrams"] += diagrams
        totals["headings"] += headings
        totals["copied_files"] += copied

    if not (destination / "index.md").is_file():
        raise ValueError("O manifesto público deve produzir index.md no destino.")
    return totals


def main() -> int:
    arguments = parse_arguments()
    totals = prepare_sources(
        arguments.source,
        arguments.destination,
        arguments.manifest,
        arguments.repository_docs_url,
    )
    print(f"Entradas do manifesto: {totals['manifest_entries']}")
    print(f"Arquivos Markdown processados: {totals['markdown_files']}")
    print(f"Diagramas Mermaid substituídos: {totals['diagrams']}")
    print(f"Títulos normalizados: {totals['headings']}")
    print(f"Arquivos copiados: {totals['copied_files']}")
    print(f"Fonte temporária: {arguments.destination.resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
