#!/usr/bin/env python3
"""Prepara uma fonte Markdown curada para publicação com Doxygen.

A fonte canônica em ``docs/`` permanece inalterada e conserva diagramas
Mermaid e documentos internos. Um manifesto explícito seleciona os arquivos e
diretórios públicos. Na cópia temporária, blocos Mermaid são substituídos por
uma nota com link para o GitHub, caminhos de galerias são adaptados, títulos
são normalizados e links Markdown internos tornam-se referências Doxygen.
"""

from __future__ import annotations

import argparse
import posixpath
import re
import shutil
import unicodedata
from dataclasses import dataclass
from pathlib import Path, PurePosixPath
from urllib.parse import unquote


MERMAID_OPENING = "```mermaid"
MERMAID_CLOSING = "```"
FENCE_PATTERN = re.compile(r"^[ \t]*(`{3,}|~{3,})")
HEADING_PATTERN = re.compile(r"^(#{1,6})[ \t]+(.+?)[ \t]*$")
EXPLICIT_ID_PATTERN = re.compile(r"[ \t]*\{#([A-Za-z_][A-Za-z0-9_:.-]*)\}[ \t]*$")
INLINE_CODE_PATTERN = re.compile(r"`([^`\r\n]+)`")
MARKDOWN_LINK_PATTERN = re.compile(r"(?<!!)\[([^\]\r\n]+)\]\(([^)\r\n]+)\)")
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


@dataclass(frozen=True)
class MarkdownDocument:
    source: PurePosixPath
    destination: PurePosixPath
    page_id: str
    anchors: dict[str, str]
    heading_ids: tuple[str, ...]


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
        "\n",
        "> **Diagrama Mermaid disponível na versão Markdown.**\n",
        ">\n",
        f"> Consulte [este documento no GitHub]({document_url}) para visualizar "
        "o diagrama renderizado.\n",
        "\n",
    ]


def identifier(value: str) -> str:
    normalized = unicodedata.normalize("NFKD", value)
    ascii_value = normalized.encode("ascii", "ignore").decode("ascii")
    clean = re.sub(r"[^A-Za-z0-9]+", "_", ascii_value).strip("_").lower()
    return clean or "section"


def page_identifier(destination: PurePosixPath) -> str:
    without_suffix = destination.with_suffix("").as_posix()
    return f"page_{identifier(without_suffix)}"


def heading_text_and_explicit_id(raw_text: str) -> tuple[str, str | None]:
    explicit_match = EXPLICIT_ID_PATTERN.search(raw_text)
    explicit_id = explicit_match.group(1) if explicit_match else None
    text = raw_text[: explicit_match.start()] if explicit_match else raw_text
    text = INLINE_CODE_PATTERN.sub(r"\1", text).strip()
    return text, explicit_id


def github_anchor(text: str) -> str:
    normalized = unicodedata.normalize("NFKD", text)
    normalized = "".join(char for char in normalized if not unicodedata.combining(char))
    normalized = normalized.lower().strip()
    normalized = re.sub(r"[^\w\- ]", "", normalized, flags=re.UNICODE)
    return re.sub(r"[ _]+", "-", normalized).strip("-")


def iter_manifest_files(
    source_root: Path, entries: list[ManifestEntry]
) -> list[tuple[Path, PurePosixPath, PurePosixPath]]:
    files: list[tuple[Path, PurePosixPath, PurePosixPath]] = []
    for entry in entries:
        source_path = source_root.joinpath(*entry.source.parts)
        if not source_path.exists():
            raise FileNotFoundError(f"Entrada do manifesto não encontrada: {source_path}")

        if source_path.is_dir():
            for nested_source in sorted(
                path for path in source_path.rglob("*") if path.is_file()
            ):
                nested_relative = nested_source.relative_to(source_path)
                files.append(
                    (
                        nested_source,
                        PurePosixPath(*nested_source.relative_to(source_root).parts),
                        entry.destination / PurePosixPath(*nested_relative.parts),
                    )
                )
        else:
            files.append((source_path, entry.source, entry.destination))
    return files


def unique_generated_id(candidate: str, used_ids: set[str]) -> str:
    """Retorna um identificador gerado único, com sufixo determinístico."""

    if candidate not in used_ids:
        used_ids.add(candidate)
        return candidate

    suffix = 2
    while f"{candidate}_{suffix}" in used_ids:
        suffix += 1
    unique_id = f"{candidate}_{suffix}"
    used_ids.add(unique_id)
    return unique_id


def reserve_explicit_id(explicit_id: str, used_ids: set[str]) -> str:
    """Reserva um identificador explícito e rejeita colisões editoriais."""

    if explicit_id in used_ids:
        raise ValueError(f"Identificador Doxygen explícito duplicado: {explicit_id}")
    used_ids.add(explicit_id)
    return explicit_id


def build_document_index(
    files: list[tuple[Path, PurePosixPath, PurePosixPath]]
) -> dict[PurePosixPath, MarkdownDocument]:
    documents: dict[PurePosixPath, MarkdownDocument] = {}
    used_ids: set[str] = set()

    for source_file, canonical_path, destination_path in files:
        if source_file.suffix.lower() != ".md":
            continue

        headings: list[tuple[int, str, str | None, str, str]] = []
        inside_fence: str | None = None
        github_anchor_counts: dict[str, int] = {}

        for line in source_file.read_text(encoding="utf-8").splitlines():
            fence_match = FENCE_PATTERN.match(line)
            if fence_match:
                marker = fence_match.group(1)[0]
                if inside_fence is None:
                    inside_fence = marker
                elif inside_fence == marker:
                    inside_fence = None
                continue
            if inside_fence is not None:
                continue

            heading_match = HEADING_PATTERN.match(line)
            if not heading_match:
                continue

            level = len(heading_match.group(1))
            heading_text, explicit_id = heading_text_and_explicit_id(
                heading_match.group(2)
            )
            anchor_base = github_anchor(heading_text)
            anchor_occurrence = github_anchor_counts.get(anchor_base, 0)
            github_anchor_counts[anchor_base] = anchor_occurrence + 1
            anchor = (
                anchor_base
                if anchor_occurrence == 0
                else f"{anchor_base}-{anchor_occurrence}"
            )
            slug = identifier(anchor_base or heading_text)
            headings.append((level, heading_text, explicit_id, anchor, slug))

        generated_page_id = page_identifier(destination_path)
        first_is_page = bool(headings and headings[0][0] == 1)
        explicit_page_id = headings[0][2] if first_is_page else None
        page_id = (
            reserve_explicit_id(explicit_page_id, used_ids)
            if explicit_page_id
            else unique_generated_id(generated_page_id, used_ids)
        )

        anchors: dict[str, str] = {}
        heading_ids: list[str] = []
        hierarchy: list[tuple[int, str]] = []

        for position, (level, _heading_text, explicit_id, anchor, slug) in enumerate(headings):
            if position == 0 and first_is_page:
                heading_id = page_id
                hierarchy = []
            else:
                while hierarchy and hierarchy[-1][0] >= level:
                    hierarchy.pop()
                hierarchy_slug = "_".join(
                    [parent_slug for _parent_level, parent_slug in hierarchy]
                    + [slug]
                )
                candidate = f"{page_id}_{hierarchy_slug}"
                heading_id = (
                    reserve_explicit_id(explicit_id, used_ids)
                    if explicit_id
                    else unique_generated_id(candidate, used_ids)
                )
                hierarchy.append((level, slug))

            heading_ids.append(heading_id)
            if anchor:
                anchors[anchor] = heading_id

        documents[destination_path] = MarkdownDocument(
            source=canonical_path,
            destination=destination_path,
            page_id=page_id,
            anchors=anchors,
            heading_ids=tuple(heading_ids),
        )

    return documents

def resolve_document_path(
    current_destination: PurePosixPath, target: str
) -> PurePosixPath | None:
    target_path = unquote(target).replace("\\", "/")
    if not target_path.lower().endswith(".md"):
        return None

    current_parent = current_destination.parent.as_posix()
    combined = posixpath.normpath(posixpath.join(current_parent, target_path))
    if combined == ".." or combined.startswith("../") or combined.startswith("/"):
        return None
    return PurePosixPath(combined)


def rewrite_internal_links(
    line: str,
    current_document: MarkdownDocument,
    documents: dict[PurePosixPath, MarkdownDocument],
) -> tuple[str, int]:
    rewritten_count = 0

    def replacement(match: re.Match[str]) -> str:
        nonlocal rewritten_count
        label = match.group(1)
        raw_target = match.group(2).strip()

        if raw_target.startswith(("#", "http://", "https://", "mailto:")):
            return match.group(0)

        target_without_title = raw_target.split(maxsplit=1)[0]
        path_part, separator, fragment = target_without_title.partition("#")
        destination_path = resolve_document_path(current_document.destination, path_part)
        if destination_path is None:
            return match.group(0)

        target_document = documents.get(destination_path)
        if target_document is None:
            return match.group(0)

        reference_id = target_document.page_id
        if separator and fragment:
            reference_id = target_document.anchors.get(
                unquote(fragment).lower(), reference_id
            )

        rewritten_count += 1
        return f'\\ref {reference_id} "{label}"'

    return MARKDOWN_LINK_PATTERN.sub(replacement, line), rewritten_count


def transform_markdown(
    source_file: Path,
    destination_file: Path,
    document: MarkdownDocument,
    repository_docs_url: str,
    documents: dict[PurePosixPath, MarkdownDocument],
) -> tuple[int, int, int]:
    lines = source_file.read_text(encoding="utf-8").splitlines(keepends=True)
    output: list[str] = []
    inside_mermaid = False
    inside_fence: str | None = None
    diagram_count = 0
    normalized_heading_count = 0
    rewritten_link_count = 0
    last_line_number = 0
    heading_id_index = 0

    for line_number, line in enumerate(lines, start=1):
        last_line_number = line_number
        stripped = line.strip()

        if not inside_mermaid and stripped == MERMAID_OPENING:
            inside_mermaid = True
            diagram_count += 1
            if output and output[-1].strip():
                output.append("\n")
            output.extend(mermaid_note(document.source, repository_docs_url))
            continue

        if inside_mermaid:
            if stripped == MERMAID_CLOSING:
                inside_mermaid = False
            continue

        fence_match = FENCE_PATTERN.match(line)
        if fence_match:
            marker = fence_match.group(1)[0]
            if inside_fence is None:
                inside_fence = marker
            elif inside_fence == marker:
                inside_fence = None
            output.append(line)
            continue

        if inside_fence is not None:
            output.append(line)
            continue

        transformed = line
        for original, replacement in IMAGE_PATH_REPLACEMENTS.items():
            transformed = transformed.replace(original, replacement)

        heading_match = HEADING_PATTERN.match(transformed.rstrip("\r\n"))
        if heading_match:
            heading_text, _explicit_id = heading_text_and_explicit_id(
                heading_match.group(2)
            )
            if heading_id_index >= len(document.heading_ids):
                raise ValueError(
                    f"Índice de títulos inconsistente ao transformar {source_file}."
                )
            heading_id = document.heading_ids[heading_id_index]
            heading_id_index += 1

            transformed = f"{heading_match.group(1)} {heading_text} {{#{heading_id}}}\n"
            if INLINE_CODE_PATTERN.search(heading_match.group(2)):
                normalized_heading_count += 1
        elif stripped == r"\[":
            transformed = "\\f[\n"
        elif stripped == r"\]":
            transformed = "\\f]\n"

        transformed, rewritten = rewrite_internal_links(
            transformed, document, documents
        )
        rewritten_link_count += rewritten
        output.append(transformed)

    if inside_mermaid:
        raise ValueError(
            f"Bloco Mermaid não terminado em {source_file} "
            f"(detectado até a linha {last_line_number})."
        )

    destination_file.parent.mkdir(parents=True, exist_ok=True)
    destination_file.write_text("".join(output), encoding="utf-8", newline="\n")
    return diagram_count, normalized_heading_count, rewritten_link_count


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
    files = iter_manifest_files(source, entries)
    documents = build_document_index(files)

    if destination.exists():
        shutil.rmtree(destination)
    destination.mkdir(parents=True)

    totals = {
        "manifest_entries": len(entries),
        "markdown_files": 0,
        "diagrams": 0,
        "headings": 0,
        "links": 0,
        "copied_files": 0,
    }

    for source_file, _canonical_path, destination_path in files:
        output_path = destination.joinpath(*destination_path.parts)
        if source_file.suffix.lower() == ".md":
            document = documents[destination_path]
            diagrams, headings, links = transform_markdown(
                source_file,
                output_path,
                document,
                repository_docs_url,
                documents,
            )
            totals["markdown_files"] += 1
            totals["diagrams"] += diagrams
            totals["headings"] += headings
            totals["links"] += links
        else:
            output_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source_file, output_path)
        totals["copied_files"] += 1

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
    print(f"Links internos reescritos: {totals['links']}")
    print(f"Arquivos copiados: {totals['copied_files']}")
    print(f"Fonte temporária: {arguments.destination.resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
