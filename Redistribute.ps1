Function Find-MsBuild([int] $MaxVersion)
{
    $agentPath = "$Env:programfiles (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\msbuild.exe"
    $devPath = "$Env:programfiles (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\msbuild.exe"
    $proPath = "$Env:programfiles (x86)\Microsoft Visual Studio\2017\Professional\MSBuild\15.0\Bin\msbuild.exe"
    $communityPath = "$Env:programfiles (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"
    $fallback2015Path = "${Env:ProgramFiles(x86)}\MSBuild\14.0\Bin\MSBuild.exe"
    $fallback2013Path = "${Env:ProgramFiles(x86)}\MSBuild\12.0\Bin\MSBuild.exe"
    $fallbackPath = "C:\Windows\Microsoft.NET\Framework\v4.0.30319"
		
    If ((2017 -le $MaxVersion) -And (Test-Path $agentPath)) { return $agentPath } 
    If ((2017 -le $MaxVersion) -And (Test-Path $devPath)) { return $devPath } 
    If ((2017 -le $MaxVersion) -And (Test-Path $proPath)) { return $proPath } 
    If ((2017 -le $MaxVersion) -And (Test-Path $communityPath)) { return $communityPath } 
    If ((2015 -le $MaxVersion) -And (Test-Path $fallback2015Path)) { return $fallback2015Path } 
    If ((2013 -le $MaxVersion) -And (Test-Path $fallback2013Path)) { return $fallback2013Path } 
    If (Test-Path $fallbackPath) { return $fallbackPath } 
        
    throw "Unable to find msbuild"
}

Write-Host "Searcing for Visual Studio..."
$MsBuildPath = Find-MsBuild 2017

Write-Host "Cleaning..."
Invoke-Expression "& '$MsBuildPath' /t:clean /p:configuration=Debug /verbosity:minimal"
Invoke-Expression "& '$MsBuildPath' /t:clean /p:configuration=Release /verbosity:minimal"

Write-Host "Building..."
Invoke-Expression "& '$MsBuildPath' /t:build /p:configuration=Release /verbosity:minimal"

Write-Host "Packing..."
$EngineVersion = Get-Date -Format '1.0.yy.MM.dd'
compress-archive -path ('./bin/*.dll', './bin/*.exe', './bin/*.cso', './bin/Data/') -destinationpath "CLUSEK-$EngineVersion.zip" -compressionlevel optimal