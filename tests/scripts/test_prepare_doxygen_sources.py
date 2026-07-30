from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "prepare_doxygen_sources.py"
)
SPEC = importlib.util.spec_from_file_location("prepare_doxygen_sources", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
MODULE = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = MODULE
SPEC.loader.exec_module(MODULE)


class PrepareDoxygenSourcesTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_directory = tempfile.TemporaryDirectory()
        self.root = Path(self.temp_directory.name)
        self.source = self.root / "docs"
        self.destination = self.root / "build" / "docs"
        self.source.mkdir()
        self.manifest = self.root / "manifest.txt"
        self.repository_url = "https://example.test/repo/blob/main/docs"

    def tearDown(self) -> None:
        self.temp_directory.cleanup()

    def write(self, relative_path: str, content: str) -> Path:
        path = self.source / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8", newline="\n")
        return path

    def prepare(self) -> dict[str, int]:
        return MODULE.prepare_sources(
            self.source,
            self.destination,
            self.manifest,
            self.repository_url,
        )

    def test_manifest_ignores_comments_and_maps_public_index(self) -> None:
        self.write("site/index.md", "# Página\n")
        self.manifest.write_text(
            "# comentário\n\nsite/index.md -> index.md\n", encoding="utf-8"
        )
        totals = self.prepare()
        self.assertEqual(
            "# Página {#page_index}\n",
            (self.destination / "index.md").read_text(encoding="utf-8"),
        )
        self.assertEqual(1, totals["manifest_entries"])


    def test_rejects_duplicate_source(self) -> None:
        self.write("index.md", "# Página\n")
        self.manifest.write_text("index.md\nindex.md -> copy.md\n", encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "Origem duplicada"):
            self.prepare()

    def test_rejects_malformed_mapping(self) -> None:
        self.write("index.md", "# Página\n")
        self.manifest.write_text(
            "index.md -> copy.md -> other.md\n", encoding="utf-8"
        )
        with self.assertRaisesRegex(ValueError, "Mapeamento inválido"):
            self.prepare()

    def test_requires_public_index_in_destination(self) -> None:
        self.write("page.md", "# Página\n")
        self.manifest.write_text("page.md\n", encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "produzir index.md"):
            self.prepare()

    def test_copies_explicit_non_markdown_file(self) -> None:
        self.write("index.md", "# Página\n")
        asset = self.source / "asset.txt"
        asset.write_text("conteúdo", encoding="utf-8")
        self.manifest.write_text("index.md\nasset.txt\n", encoding="utf-8")
        self.prepare()
        self.assertEqual(
            "conteúdo",
            (self.destination / "asset.txt").read_text(encoding="utf-8"),
        )

    def test_rejects_missing_source(self) -> None:
        self.manifest.write_text("missing.md -> index.md\n", encoding="utf-8")
        with self.assertRaises(FileNotFoundError):
            self.prepare()

    def test_rejects_duplicate_destination(self) -> None:
        self.write("a.md", "# A\n")
        self.write("b.md", "# B\n")
        self.manifest.write_text(
            "a.md -> index.md\nb.md -> index.md\n", encoding="utf-8"
        )
        with self.assertRaisesRegex(ValueError, "Destino duplicado"):
            self.prepare()

    def test_rejects_parent_destination(self) -> None:
        self.write("index.md", "# A\n")
        self.manifest.write_text("index.md -> ../index.md\n", encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "Destino inválido"):
            self.prepare()

    def test_replaces_mermaid_and_uses_canonical_source_url(self) -> None:
        self.write(
            "site/index.md",
            "# Página\n\n```mermaid\nflowchart LR\nA --> B\n```\n",
        )
        self.manifest.write_text("site/index.md -> index.md\n", encoding="utf-8")
        totals = self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertNotIn("```mermaid", output)
        self.assertIn("/docs/site/index.md", output)
        self.assertEqual(1, totals["diagrams"])

    def test_rejects_unterminated_mermaid(self) -> None:
        self.write("index.md", "# Página\n```mermaid\nA --> B\n")
        self.manifest.write_text("index.md\n", encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "não terminado"):
            self.prepare()

    def test_adjusts_image_paths(self) -> None:
        self.write(
            "index.md",
            '<img src="../images/input/example.png">\n'
            '<a href="../../images/results/result.png">resultado</a>\n',
        )
        self.manifest.write_text("index.md\n", encoding="utf-8")
        self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertIn('src="images/input/example.png"', output)
        self.assertIn('href="images/results/result.png"', output)

    def test_normalizes_heading_formed_only_by_inline_code(self) -> None:
        self.write("index.md", "### `opencv_morphology_j_binary.png`\n")
        self.manifest.write_text("index.md\n", encoding="utf-8")
        totals = self.prepare()
        self.assertEqual(
            "### opencv_morphology_j_binary.png "
            "{#page_index_opencv_morphology_j_binarypng}\n",
            (self.destination / "index.md").read_text(encoding="utf-8"),
        )
        self.assertEqual(1, totals["headings"])

    def test_normalizes_inline_code_in_mixed_heading_only(self) -> None:
        self.write(
            "index.md",
            "# Página\n\n"
            "### C++: `cv::Mat`\n\n"
            "O tipo usado é `cv::Mat`.\n",
        )
        self.manifest.write_text("index.md\n", encoding="utf-8")
        totals = self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertIn("### C++: cv::Mat {#page_index_c_cvmat}\n", output)
        self.assertIn("O tipo usado é `cv::Mat`.\n", output)
        self.assertEqual(1, totals["headings"])

    def test_rewrites_internal_markdown_page_link(self) -> None:
        self.write(
            "index.md",
            "# Início\n\n[Catálogo](image-catalog.md)\n",
        )
        self.write("image-catalog.md", "# Catálogo de imagens\n")
        self.manifest.write_text(
            "index.md\nimage-catalog.md\n", encoding="utf-8"
        )
        totals = self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertIn('\\ref page_image_catalog "Catálogo"', output)
        self.assertNotIn("image-catalog.md", output)
        self.assertEqual(1, totals["links"])

    def test_rewrites_internal_markdown_section_link(self) -> None:
        self.write(
            "index.md",
            "# Início\n\n[Resultados](image-catalog.md#resultados-curados)\n",
        )
        self.write(
            "image-catalog.md",
            "# Catálogo de imagens\n\n## Resultados curados\n",
        )
        self.manifest.write_text(
            "index.md\nimage-catalog.md\n", encoding="utf-8"
        )
        self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertIn(
            '\\ref page_image_catalog_resultados_curados "Resultados"',
            output,
        )
        catalog = (self.destination / "image-catalog.md").read_text(
            encoding="utf-8"
        )
        self.assertIn(
            "## Resultados curados "
            "{#page_image_catalog_resultados_curados}\n",
            catalog,
        )

    def test_resolves_relative_link_between_nested_documents(self) -> None:
        self.write(
            "labs/m1-1.md",
            "# Laboratório\n\n[Arquitetura](../architecture.md)\n",
        )
        self.write("architecture.md", "# Arquitetura\n")
        self.write("site/index.md", "# Início\n")
        self.manifest.write_text(
            "site/index.md -> index.md\n"
            "labs/m1-1.md\n"
            "architecture.md\n",
            encoding="utf-8",
        )
        self.prepare()
        output = (self.destination / "labs/m1-1.md").read_text(
            encoding="utf-8"
        )
        self.assertIn('\\ref page_architecture "Arquitetura"', output)

    def test_preserves_external_links_and_image_links(self) -> None:
        self.write(
            "index.md",
            "# Página\n\n"
            "[OpenCV](https://opencv.org/)\n"
            "![Imagem](images/input/example.png)\n",
        )
        self.manifest.write_text("index.md\n", encoding="utf-8")
        self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertIn("[OpenCV](https://opencv.org/)", output)
        self.assertIn("![Imagem](images/input/example.png)", output)

    def test_preserves_existing_explicit_page_identifier(self) -> None:
        self.write(
            "index.md",
            "# Comparação {#language_comparison}\n\n"
            "## Tipos numéricos\n",
        )
        self.manifest.write_text("index.md\n", encoding="utf-8")
        self.prepare()
        output = (self.destination / "index.md").read_text(encoding="utf-8")
        self.assertIn("# Comparação {#language_comparison}\n", output)
        self.assertIn(
            "## Tipos numéricos {#language_comparison_tipos_numericos}\n",
            output,
        )

    def test_copies_only_manifest_entries_and_cleans_destination(self) -> None:
        self.write("index.md", "# Página\n")
        self.write("private.md", "# Interno\n")
        self.destination.mkdir(parents=True)
        (self.destination / "stale.md").write_text("antigo", encoding="utf-8")
        self.manifest.write_text("index.md\n", encoding="utf-8")
        self.prepare()
        self.assertTrue((self.destination / "index.md").is_file())
        self.assertFalse((self.destination / "private.md").exists())
        self.assertFalse((self.destination / "stale.md").exists())

    def test_copies_directory_entries_recursively(self) -> None:
        self.write("index.md", "# Página\n")
        binary = self.source / "images" / "result.bin"
        binary.parent.mkdir(parents=True)
        binary.write_bytes(b"result")
        self.manifest.write_text("index.md\nimages\n", encoding="utf-8")
        totals = self.prepare()
        self.assertEqual(b"result", (self.destination / "images/result.bin").read_bytes())
        self.assertEqual(2, totals["copied_files"])


if __name__ == "__main__":
    unittest.main()
