@echo off
for /f "tokens=2-4 delims=/ " %%a in ('date /T') do set year=%%c
for /f "tokens=2-4 delims=/ " %%a in ('date /T') do set month=%%a
for /f "tokens=2-4 delims=/ " %%a in ('date /T') do set day=%%b
for /f "tokens=1 delims=: " %%h in ('time /T') do set hour=%%h
for /f "tokens=2 delims=: " %%m in ('time /T') do set minutes=%%m
set now=%month%_%day%_%year%_%hour%_%minutes%
c:\progra~1\7-zip\7z a -tzip c:/users/hk/documents/archive/Fractal2_%now%.zip source/*.* *.vcxproj *.sln *.h *.ico *.rc -xr!source/cShader.*



