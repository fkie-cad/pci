@echo off
setlocal enabledelayedexpansion

set prog_name=%~n0
set my_dir="%~dp0"
set "my_dir=%my_dir:~1,-2%"

:: ids 
set /a id=0
set /a CFG=%id%
set /a id+=1
set /a MEMIO=%id%
set /a id+=1
set /a PCI_CFG=%id%
set /a id+=1
REM set /a PORT_IO=%id%
REM set /a id+=1
set /a SPI=%id%
set /a id+=1
set /a "PROJ_ID_MAX=%id%-1"
set /a PROJ_COUNT=%id%

:: (sub) proj names
set projects[%CFG%]=Cfg
set projects[%MEMIO%]=MemIo
set projects[%PCI_CFG%]=PciCfg
REM set projects[%PORT_IO%]=PortIo
set projects[%SPI%]=Spi

:: compile activation flags
set /a cmpl[%CFG%]=0
set /a cmpl[%MEMIO%]=0
set /a cmpl[%PCI_CFG%]=0
REM set /a cmpl[%PORT_IO%]=0
set /a cmpl[%SPI%]=0

:: cmd param
set cmdparams[%CFG%]=/cfg
set cmdparams[%MEMIO%]=/mio
set cmdparams[%PCI_CFG%]=/pfg
REM set cmdparams[%PORT_IO%]=/pio
set cmdparams[%SPI%]=/spi

:: components flag: 1=driver, 2=user
set /a components[%CFG%]=2
set /a components[%MEMIO%]=1
set /a components[%PCI_CFG%]=2
REM set /a components[%PORT_IO%]=2
set /a components[%SPI%]=2

:: help description
set "description[%CFG%]=/cfg: Build pci cfgui exe. Requiring memio driver."
set "description[%MEMIO%]=/memio: Build memio utility driver (sys)"
set "description[%PCI_CFG%]=/pciCfg: Build pciCfg code converter tool (exe)"
REM set "description[%PORT_IO%]=/portio: Build port io user app (exe). Requiring memio driver."
set "description[%SPI%]=/spi: Build spiui command exe. Requiring memio driver."

set /a all=0
set /a sys=0
set /a exe=0
set /a cln=0

set /a debug=0
set /a release=0

set /a verbose=0

set /a DP_FLAG=1
set /a EP_FLAG=2
set /a LP_FLAG=4
set /a debug_print=%EP_FLAG%

:: add pdb
set /a OPT_FLAG_PDB=0x1
:: add rtl
set /a OPT_FLAG_RTL=0x2
:: test signing
set /a OPT_FLAG_TEST_SIGNING=0x4
SET /a "flags=%OPT_FLAG_PDB%|%OPT_FLAG_TEST_SIGNING%"

set /a bitness=64
set platform=x64
set configuration=Debug

set proj_dir=.
set proj_ftype=.vcxproj
set exe_proj=Exe.vcxproj
set sys_proj=Sys.vcxproj

set epts=WindowsApplicationForDrivers10.0
set spts=WindowsKernelModeDriver10.0


if [%1]==[] goto main

GOTO :ParseParams

:ParseParams

    REM IF "%~1"=="" GOTO Main
    if [%1]==[/?] goto help
    if [%1]==[/h] goto help
    if [%1]==[/help] goto help
    
    :: projects
    FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
        if /i [%~1] == [!cmdparams[%%i]!] (
            set /a cmpl[%%i]=1
            goto reParseParams
        )
    )

    IF /i "%~1"=="/all" (
        SET /a all=1
        goto reParseParams
    )
    IF /i "%~1"=="/sys" (
        SET /a sys=1
        goto reParseParams
    )
    IF /i "%~1"=="/exe" (
        SET /a exe=1
        goto reParseParams
    )
    
    IF /i "%~1"=="/cln" (
        SET /a cln=1
        goto reParseParams
    )

    :: debug / release
    IF /i "%~1"=="/d" (
        SET /a debug=1
        goto reParseParams
    )
    IF /i "%~1"=="/r" (
        SET /a release=1
        goto reParseParams
    )
    
    :: print flags
    IF /i "%~1"=="/dp" (
        SET /a "debug_print=%~2"
        SHIFT
        goto reParseParams
    )
    IF /i "%~1"=="/dpf" (
        SET /a "debug_print=%debug_print%|DP_FLAG"
        goto reParseParams
    )
    IF /i "%~1"=="/epf" (
        set /a "debug_print=%debug_print%|EP_FLAG"
        goto reParseParams
    )
    IF /i "%~1"=="/lpf" (
        set /a "debug_print=%debug_print%|LP_FLAG"
        goto reParseParams
    )
    
    
    
    :: options
    IF /i "%~1"=="/xpdb" (
        SET /a "flags=%flags%&~%OPT_FLAG_PDB%"
        goto reParseParams
    )
    IF /i "%~1"=="/rtl" (
        set /a "flags=%flags%|OPT_FLAG_RTL"
        goto reParseParams
    )
    IF /i "%~1"=="/xts" (
        SET /a "flags=%flags%&~%OPT_FLAG_TEST_SIGNING%"
        goto reParseParams
    )

    IF /i "%~1"=="/epts" (
        SET epts=%~2
        SHIFT
        goto reParseParams
    )
    IF /i "%~1"=="/spts" (
        SET spts=%~2
        SHIFT
        goto reParseParams
    )

    IF /i "%~1"=="/v" (
        SET /a verbose=1
        goto reParseParams
    ) ELSE (
        echo Unknown option : "%~1"
        goto help
    )
    
    :reParseParams
    SHIFT
    if [%1]==[] goto main

GOTO :ParseParams


:main

    :: check params
    set /a "s=%debug%+%release%"
    if %s% == 0 (
        set /a debug=0
        set /a release=1
    )
    
    set /a "s=%sys%+%exe%"
    if %s% == 0 (
        set /a sys=1
        set /a exe=1
    )
    if %all% == 1 (
        set /a sys=1
        set /a exe=1
    )

    if %all% == 1 (
        FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
            set /a cmpl[%%i]=1
        )
    )

    :: if all projects are zero, break
    set /a s=0
    FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
        if !cmpl[%%i]! == 1 (
            set /a s=1
            goto endLoop
        )
    )
    if %cln% == 1 (
        set /a s=1
    )
    :endLoop
    if %s% == 0 (
        echo [e] no target set
        echo.
        goto help
    )

    :: print verbose info
    if %verbose% == 1 (
        set /a "pdb=%flags%&OPT_FLAG_PDB"
        set /a "rtl=%flags%&OPT_FLAG_RTL"
        set /a "ts=%flags%&OPT_FLAG_TEST_SIGNING"

        FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
            echo !projects[%%i]!: !cmpl[%%i]!
        )
        echo.
        echo debug: %debug%
        echo release: %release%
        echo bitness: %bitness%
        echo dprint: %debug_print%
        echo flags: %flags%
        echo   pdb: !pdb!
        echo   rtl: !rtl!
        echo   ts: !ts!
        echo sys: %sys%
        echo exe: %exe%
        echo ts: %ts%
        echo epts: %epts%
        echo spts: %spts%
        echo cln: %cln%
        echo.
    )

    if %cln% == 1 (
        :: if %verbose% == 1 echo removing "%my_dir%\build" 
        echo removing "%my_dir%\build" 
        rmdir /s /q "%my_dir%\build" >nul 2>&1 
        echo.
    )

    :: build projects
    FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
        if !cmpl[%%i]! == 1 (
            call :build !projects[%%i]! %%i
            
            echo finished with code : !ERRORLEVEL!
            if NOT !ERRORLEVEL! == 0 (
                goto buildLoopEnd
            )
        )
    )
    :buildLoopEnd

    echo build finished with code : !ERRORLEVEL!
    endlocal
    exit /B !ERRORLEVEL!


:build
    SETLOCAL
        set proj_name=%~1
        set proj_id=%~2
        set /a "clSys=!components[%proj_id%]!&1"
        set /a "clExe=!components[%proj_id%]!&2"
        set proj=%proj_dir%\%sys_proj%

        set /a "clSys=%clSys%+%sys%"
        if %clSys% EQU 2 (
            if %debug% == 1 call :buildEx %proj% %proj_name% %platform% Debug %debug_print% %flags% %spts%
            if %release% == 1 call :buildEx %proj% %proj_name% %platform% Release %debug_print% %flags% %spts%
        )
        if NOT !ERRORLEVEL! == 0 (
            goto buildEnd
        )

        set /a "clExe=%clExe%+%exe%"
        if %clExe% EQU 3 (
            if %debug% == 1 call :buildEx %proj_dir%\%exe_proj% %proj_name% %platform% Debug %debug_print% %flags% %epts%
            if %release% == 1 call :buildEx %proj_dir%\%exe_proj% %proj_name% %platform% Release %debug_print% %flags% %epts%
        )
        if NOT !ERRORLEVEL! == 0 (
            goto buildEnd
        )

    :buildEnd
    ENDLOCAL
    EXIT /B %ERRORLEVEL%
    
:buildEx
    SETLOCAL
        set proj=%~1
        set pname=%~2
        set platform=%~3
        set conf=%~4
        set /a dpf=%~5
        set /a flags=%~6
        set pts=%~7
        
        :: print flags
        set /a "dp=%dpf%&~%EP_FLAG%"
        set /a "ep=%dpf%&%EP_FLAG%"
        if %ep% NEQ 0 ( set /a ep=1 )
        
        :: option flags
        set /a "pdb=%flags%&%OPT_FLAG_PDB%"
        set /a "rtl=%flags%&%OPT_FLAG_RTL%"
        set /a "ts=%flags%&%OPT_FLAG_TEST_SIGNING%"
        
        :: run time libs
        if %rtl% EQU 0 (
            set rtl=None
        ) else (
            set rtl=%conf%
        )

        :: pdbs
        if [%conf%] EQU [Debug] (
            set /a pdb=1
        )
        
        if %verbose% EQU 1 (
            echo build
            echo  - Project=%proj%
            echo  - Name=%pname%
            echo  - Platform=%platform%
            echo  - Configuration=%conf%
            echo  - DebugPrint=%dp%
            echo  - RuntimeLib=%rtl%
            echo  - PDB=%pdb%
            echo  - pts=%pts%
            echo  - ts=%ts%
            echo.
        )

        msbuild %proj% /p:Platform=%platform% /p:PlatformToolset=%pts% /p:Configuration=%conf% /p:DebugPrint=%dp% /p:ErrorPrint=%ep% /p:RuntimeLib=%rtl% /p:PDB=%pdb% /p:SubProj=%pname% /p:TestSign=%ts%
        echo.
        echo ----------------------------------------------------
        echo.
        echo.
    ENDLOCAL
    
    EXIT /B %ERRORLEVEL%


:usage
    echo|set /p="Usage: %prog_name% [/all] "
    FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
        echo|set /p="[!cmdparams[%%i]!] "
    )
    echo|set /p="[/d] [/r] [/dp <value>] [/dpf] [/epf] [/sys] [/exe] [/pts <value>] [/pdb] [/rtl] [/xts]"
    exit /B 0
    
:help
    call :usage
    echo.
    echo Targets:
    FOR /L %%i IN (0 1 %PROJ_ID_MAX%) DO  (
        echo !description[%%i]!
    )
    echo.
    echo Debug print:
    echo /dp: Debug print flag value.
    echo   /dpf: Debug print flag (%DP_FLAG%). Print debug stuff.
    echo   /epf: Error print flag (%EP_FLAG%). Print errors.
    echo.
    echo Options:
    echo /exe: Build the user mode app, if existent.
    echo /sys: Build the driver (default).
    echo /cln: Clean/delete build folder.
    echo /d: Build in debug mode.
    echo /r: Build in release mode.
    echo /b: Bitness of exe/lib. 32^|64. Default: 64.
    echo /xpdb: No pdbs.
    echo /rtl: Compile with RuntimeLibrary.
    echo /epts: Set the platform toolset for exe files. I.e. WindowsApplicationForDrivers10.0
    echo /spts: Set the platform toolset for sys files. I.e. WindowsKernelModeDriver10.0
    echo /xts: No test signing
    echo.
    echo /h: Print this.

    exit /B 0
