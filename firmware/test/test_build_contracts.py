"""Offline build regressions. These do not validate Windows or an Xtensa link."""
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
COMP = ROOT / "firmware/components"

def run(*args, **kwargs):
    return subprocess.run(list(map(str, args)), text=True, capture_output=True, timeout=90, **kwargs)

class BuildContracts(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp(prefix="build-contract-", dir=ROOT / ".buildtmp"))

    def manifests(self):
        # Evaluate the actual CMake lists (not guessed filenames) via a boundary register.
        script = self.tmp / "manifest.cmake"
        script.write_text('function(idf_component_register)\n'
                          'cmake_parse_arguments(C "" "" "SRCS;REQUIRES;PRIV_REQUIRES;INCLUDE_DIRS" ${ARGN})\n'
                          'message("MANIFEST|${CMAKE_CURRENT_LIST_DIR}|${C_SRCS}|${C_REQUIRES}")\nendfunction()\n' +
                          # Only the local app's dependency graph is under test;
                          # unrelated components may contain non-scriptable target commands.
                          '\n'.join(f'include("{(COMP / name / "CMakeLists.txt").as_posix()}")'
                                    for name in ("jalak_app", "jalak_hal", "jalak_local", "jalak_logic")))
        p = run("cmake", "-P", script)
        self.assertEqual(p.returncode, 0, p.stdout + p.stderr)
        return {Path(d).name: (Path(d), s.split(";"), deps.split(";"))
                for d, s, deps in re.findall(r"MANIFEST\|([^|\n]+)\|([^|\n]*)\|([^\n]*)", p.stderr)}

    def test_manifest_sources_and_public_dependencies(self):
        manifest = self.manifests()
        for directory, sources, _ in manifest.values():
            for source in sources:
                self.assertTrue((directory / source).is_file(), str(directory / source))
        self.assertEqual(set(manifest["jalak_local"][1]), {"src/demo.cpp", "src/sounds.cpp", "src/clip.cpp"})
        self.assertNotIn("src/api_client.cpp", manifest["jalak_logic"][1])
        for dependency in ("esp_driver_i2c", "esp_driver_i2s", "esp_driver_spi", "sdmmc"):
            self.assertIn(dependency, manifest["jalak_hal"][2])

    def test_actual_local_component_link_without_exceptions(self):
        if not shutil.which("c++"):
            self.skipTest("GNU-compatible host C++ unavailable; IDF manifest compile not run")
        manifest = self.manifests()
        main = self.tmp / "main.cpp"
        main.write_text('#include "jalak/local/sounds.h"\n#include "jalak/local/clip.h"\n'
                        'int main() { return jalak::local::render_tone(jalak::local::Tone::UiTap).empty() || '
                        '!jalak::local::unpack_frames({}, 0).empty(); }\n')
        cmd = ["c++", "-std=c++17", "-fno-exceptions", main, "-o", self.tmp / "local-link"]
        for name in ("jalak_logic", "jalak_local"):
            directory, sources, _ = manifest[name]
            cmd += ["-I", directory / "include", *(directory / source for source in sources)]
        p = run(*cmd)
        self.assertEqual(p.returncode, 0, p.stdout + p.stderr)
        self.assertEqual(run(self.tmp / "local-link").returncode, 0)

    def test_root_probe_compiles_archives_but_final_link_still_fails(self):
        if os.name == "nt" or not shutil.which("ar"):
            self.skipTest("GNU host linker/ar required for deliberate-invalid-link experiment")
        idf = self.tmp / "idf/tools/cmake"
        idf.mkdir(parents=True)
        (idf / "project.cmake").write_text("")
        main = self.tmp / "main.c"
        main.write_text("int main(void) { return 0; }\n")
        include = self.tmp / "final.cmake"
        include.write_text(f'add_executable(final "{main.as_posix()}")\n')
        env = dict(os.environ, IDF_PATH=str(self.tmp / "idf"))
        p = run("cmake", "-S", ROOT / "firmware", "-B", self.tmp / "build", "-G", "Ninja",
                "--debug-trycompile", "-DCMAKE_EXE_LINKER_FLAGS=-Wl,--jalak-deliberately-invalid",
                f"-DCMAKE_PROJECT_sijalak_bedas_INCLUDE={include}", env=env)
        self.assertEqual(p.returncode, 0, p.stdout + p.stderr)
        archives = list((self.tmp / "build/CMakeFiles").rglob("*.a"))
        self.assertTrue(archives, "compiler checks must actually compile/archive")
        for archive in archives:
            self.assertEqual(run("ar", "t", archive).returncode, 0)
        p = run("cmake", "--build", self.tmp / "build")
        self.assertNotEqual(p.returncode, 0, "STATIC_LIBRARY must not bypass final linking")
        self.assertIn("jalak-deliberately-invalid", p.stdout + p.stderr)
        # Invalid C input must still fail its compiler check (no COMPILER_WORKS forcing).
        p = run("cmake", "-S", ROOT / "firmware", "-B", self.tmp / "bad-compile", "-G", "Ninja",
                "-DCMAKE_C_FLAGS=-fjalak-deliberately-invalid", env=env)
        self.assertNotEqual(p.returncode, 0)

    def test_required_tree_present_and_remote_inventory(self):
        required = {p.relative_to(ROOT) for p in (ROOT / "firmware").glob("components/**/include/**/*.h")}
        required.update(Path("firmware") / p for p in ("CMakeLists.txt", "sdkconfig.defaults", "test/CMakeLists.txt",
                         "components/jalak_app/Kconfig.projbuild", "preview/local_runner.cpp", "preview/test_preview.py", "preview/serve.py"))
        host_cmake = (ROOT / "firmware/test/CMakeLists.txt").read_text()
        for source in re.findall(r'[\w./-]+\.cpp', host_cmake):
            required.add((ROOT / "firmware/test" / source).resolve().relative_to(ROOT))
        required.update(p.relative_to(ROOT) for p in (ROOT / "firmware/test").rglob("*.h"))
        required.update(p.relative_to(ROOT) for p in (ROOT / "firmware/test").glob("test*"))
        for directory, sources, _ in self.manifests().values():
            required.update((directory / s).relative_to(ROOT) for s in sources)
        required.update(p.relative_to(ROOT) for p in (ROOT / "tools").glob("*windows*"))
        for path in required:
            self.assertTrue((ROOT / path).is_file(), str(path))
        if not shutil.which("git"):
            print("Required files present; remote inventory skipped (git unavailable)")
            return
        p = run("git", "ls-files", cwd=ROOT)
        self.assertEqual(p.returncode, 0, p.stderr)
        tracked = set(p.stdout.splitlines())
        untracked = sorted(str(path) for path in required if path.as_posix() not in tracked)
        print("Required but untracked (remote incomplete): " + ", ".join(untracked))

    def test_windows_parser_and_mock_workflows_if_available(self):
        shell = shutil.which("pwsh") or shutil.which("powershell")
        if not shell:
            self.skipTest("PowerShell not installed: parser and runtime mocks NOT validated")
        p = run(shell, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
                ROOT / "tools/test-windows.ps1")
        self.assertEqual(p.returncode, 0, p.stdout + p.stderr)

    def test_windows_launcher_source_contract(self):
        # Structural regressions only. NOT PowerShell parse/runtime assurance.
        for name in ("flash-windows.ps1", "install-windows.ps1", "build-host-tests.ps1"):
            text = (ROOT / "tools" / name).read_text()
            executable = re.sub(r"(?m)^\s*#.*$", "", text)
            self.assertNotRegex(executable, r"(?m)^\s*idf\.py\s")
            self.assertNotIn("set-target", executable)
            self.assertNotIn("fullclean", executable)
            self.assertNotIn("Remove-Item", executable)
            self.assertNotIn("Tee-Object", executable)
            self.assertNotIn("AutoInstall", executable)
            self.assertIn("Invoke-NativeChecked", executable)
        flash = (ROOT / "tools/flash-windows.ps1").read_text()
        self.assertNotRegex(flash, r"Get-CimInstance|Get-PnpDevice|git clone")
        self.assertIn("Assert-JalakArtifacts", flash)
        self.assertIn("$sdk.Python", flash)
        batch = (ROOT / "tools/flash-windows.bat").read_text()
        self.assertNotIn("set /p", batch.lower())
        self.assertIn("choice /C 1234", batch)
        self.assertIn("if errorlevel 5 exit /b 1", batch)

if __name__ == "__main__":
    unittest.main(verbosity=2)
