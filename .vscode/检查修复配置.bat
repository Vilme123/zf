@echo off
rem ============================================================================
rem  Launcher: check and restore .vscode configs (content + read-only lock)
rem
rem  See 检查修复配置.ps1 for what it does and why it is needed.
rem  (VS Code "C/C++ Runner" extension keeps rewriting these files.)
rem ============================================================================

cd /d "%~dp0"

echo Running 检查修复配置.ps1 ...
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0检查修复配置.ps1"

echo.
pause
