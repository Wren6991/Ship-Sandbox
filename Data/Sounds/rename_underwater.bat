@echo off

setlocal enabledelayedexpansion
for /F %%i in ('dir /b *flac') do (
	set FOO1=%%~ni
	set FOO2=!FOO1:~0,-2!
	set FOO3=!FOO1:~-2%!
	REM echo !FOO2! !FOO3!
	ren %%i !FOO2!_underwater!FOO3!.flac
)