@echo off
set geDir=%1GameEngine\
set phRelDir=vendor\PhysX\physx\bin\win.x86_64.vc143.mt\debug
set libRelDir=Engine\Libs\x64\Debug


copy %geDir%%phRelDir%\PhysX_64.lib %geDir%%libRelDir%
echo PhysX_64.lib copied from %geDir%%phRelDir% to %geDir%%libRelDir%
copy %geDir%%phRelDir%\PhysX_64.dll %geDir%
echo PhysX_64.dll copied from %geDir%%phRelDir% to %geDir%

copy %geDir%%phRelDir%\PhysXCommon_64.lib %geDir%%libRelDir%
echo PhysXCommon_64.lib copied from %geDir%%phRelDir% to %geDir%%libRelDir%
copy %geDir%%phRelDir%\PhysXCommon_64.dll %geDir%
echo PhysXCommon_64.dll copied from %geDir%%phRelDir% to %geDir%

copy %geDir%%phRelDir%\PhysXFoundation_64.lib %geDir%%libRelDir%
echo PhysXFoundation_64.lib copied from %geDir%%phRelDir% to %geDir%%libRelDir%
copy %geDir%%phRelDir%\PhysXFoundation_64.dll %geDir%
echo PhysXFoundation_64.dll copied from %geDir%%phRelDir% to %geDir%

copy %geDir%%phRelDir%\PhysXExtensions_static_64.lib %geDir%%libRelDir%
echo PhysXExtensions_static_64.lib copied from %geDir%%phRelDir% to %geDir%%libRelDir%


set monoDllsDir=%1GameEngine\vendor\mono\dlls

copy %monoDllsDir%\mono-2.0-sgen.dll %geDir%
copy %monoDllsDir%\MonoPosixHelper.dll %geDir%