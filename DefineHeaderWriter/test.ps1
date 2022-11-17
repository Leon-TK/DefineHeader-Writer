$exePath = "C:\Users\Mining1\Desktop\SOURCE\DefineHeaderWriter\x64\Debug\DefineHeaderWriter.exe"
$filePath = "C:\Users\Mining1\Desktop\SOURCE\DefineHeaderWriter\DefineHeaderWriter\DefineHeaderWriter.txt"
$fileSize = (Get-Item $filePath).length
start $exePath $filePath, $fileSize