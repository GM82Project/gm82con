call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

msbuild dll\gm82con.sln /property:Configuration=Release /p:Platform=x86

del gm82con.dll

copy dll\Release\gm82con.dll gm82con.dll

if not exist gm82con.dll pause && exit

echo building extension...

py gm82gex.py gm82con.gej

echo all done.

pause
