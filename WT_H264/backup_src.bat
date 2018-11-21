@echo off

set vardate=%date:~0,4%-%date:~5,2%-%date:~8,2%
set vartime=%time:~0,2%-%time:~3,2%-%time:~6,2%

if "%vartime:~0,1%"==" " set vartime=0%time:~1,1%-%time:~3,2%-%time:~6,2%

setlocal enabledelayedexpansion
set PWD=%~p0
set PWD=%PWD:~1,-1%
for /l %%a in (1,1,100) do (
   if "!PWD:~%%a,1!"=="\" set i=%%a
)
set /a i+=1
set PWD=!PWD:~%i%!

set varfile=bak\%PWD%_src[%vardate%_%vartime%].zip
set varrar=tool\rar.exe
set varfilelist=sln src

echo.
echo ���������ļ� %varfile%

if not exist bak mkdir bak

if exist %varfile% goto BACK_FILE_EXIST


:FILE_BACKUP

%varrar% A "%varfile%" %varfilelist%

echo.
echo %cd%\%varfile%
echo �ļ����ݽ���!
echo.
pause

goto EXIT_BAT

:BACK_FILE_EXIST

set /P OK= Ŀ���ļ� %varfile% �Ѿ����ڣ�ȷ��Ҫ����ǰ�α��ݵ��ļ���(Y/N): 

IF /I "%OK:~0,1%"=="Y" (

	del /f /s /q %varfile%
	goto FILE_BACKUP

) ELSE (

	goto EXIT_BAT

)

:EXIT_BAT

@echo on
