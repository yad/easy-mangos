@echo off

@echo.
@echo Please ensure that you have at least 1300MB of free disk space
@pause

@ad.exe
@echo.
@del /F /Q Buildings\* > nul
@vmapExtractor3.exe
@echo.
@md vmaps > nul
@del /F /S /Q vmaps\* > nul
@echo.
@vmap_assembler.exe buildings vmaps

@echo.
rem the character at the end of the following line makes a beep on Windows, please leave it alone
@echo Your dbcs, maps, and vmaps are ready for use
@echo.
@pause