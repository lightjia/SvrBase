xcopy /S /Y SvrBase export\svrbase 
xcopy /Y x64\Debug\SvrBase.lib export\svrbase
cd export\svrbase
rd /S /Q base\src
rd /S /Q cfg\src
rd /S /Q libuv\src
rd /S /Q x64
del *.h *.txt *.cpp *oj *rs