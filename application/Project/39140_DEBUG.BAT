SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;E:\CodeBuddy CN\resources\app\extensions\genie\out\vendor\shim\safe-bin;C:\Program Files\Eclipse Adoptium\jdk-25.0.5.8-hotspot\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\Git\cmd;C:\Program Files\TortoiseGit\bin;C:\Program Files (x86)\Microsoft SQL Server\160\Tools\Binn\;C:\Program Files\Microsoft SQL Server\160\Tools\Binn\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Program Files\Microsoft SQL Server\160\DTS\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\dotnet\;C:\Users\86158\AppData\Local\Microsoft\WindowsApps;E:\Microsoft VS Code\bin;C:\Program Files\Graphviz\bin;E:\CodeBuddy CN\bin;E:\Microsoft VS Code\bin\copilot-shim
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=OB90A64M1B
SET CPU_VENDOR=On-Bright
SET UV2_TARGET=39140_debug
SET CPU_CLOCK=0x02FAF080
python ..\..\tools\gen_ide_config.py
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\build\objects\system_ob90a64m1.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\build\objects\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\build\objects\adc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\build\objects\gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\Build\Objects\ota_app.lnp"
