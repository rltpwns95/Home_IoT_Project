@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM
@REM Read about available command line parameters in the C-SPY Debugging
@REM Guide. Hints about additional command line parameters that may be
@REM useful in specific cases:
@REM   --download_only   Downloads a code image without starting a debug
@REM                     session afterwards.
@REM   --silent          Omits the sign-on message.
@REM   --timeout         Limits the maximum allowed execution time.
@REM 


@echo off 

if not "%~1" == "" goto debugFile 

@echo on 

"C:\Program Files\IAR Systems\Embedded Workbench 9.1\common\bin\cspybat" -f "\\10.10.141.253\23 자율형 공유폴더\iot 프로젝트\기세준 김현경 손재원 스마트 가습기 시스템\project\settings\sampleUart2.Debug.general.xcl" --backend -f "\\10.10.141.253\23 자율형 공유폴더\iot 프로젝트\기세준 김현경 손재원 스마트 가습기 시스템\project\settings\sampleUart2.Debug.driver.xcl" 

@echo off 
goto end 

:debugFile 

@echo on 

"C:\Program Files\IAR Systems\Embedded Workbench 9.1\common\bin\cspybat" -f "\\10.10.141.253\23 자율형 공유폴더\iot 프로젝트\기세준 김현경 손재원 스마트 가습기 시스템\project\settings\sampleUart2.Debug.general.xcl" "--debug_file=%~1" --backend -f "\\10.10.141.253\23 자율형 공유폴더\iot 프로젝트\기세준 김현경 손재원 스마트 가습기 시스템\project\settings\sampleUart2.Debug.driver.xcl" 

@echo off 
:end