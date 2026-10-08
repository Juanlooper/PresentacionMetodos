@echo off
setlocal
cd /d "%~dp0"
title Laboratorio 2 - Metodos numericos
echo ============================================================
echo   LABORATORIO 2 - COMPILAR Y PROBAR
echo ============================================================
where gcc >nul 2>nul
if errorlevel 1 (
    echo No se encontro GCC en el PATH.
    echo Instala un compilador C11 o abre laboratorio.c en tu IDE.
    echo Consulta LEEME-C.txt para los comandos.
    pause
    exit /b 1
)
gcc -std=c11 -Wall -Wextra -pedantic laboratorio.c -lm -o laboratorio.exe
if errorlevel 1 (
    echo La compilacion fallo. Revisa el mensaje anterior.
    pause
    exit /b 1
)
echo Compilacion correcta.
echo.
echo 1. Ejecutar el ejemplo de los tres talleres
echo 2. Introducir otro sistema
choice /c 12 /n /m "Elige 1 o 2: "
if errorlevel 2 (
    laboratorio.exe
) else (
    laboratorio.exe --ejemplo
)
echo.
pause
