param(
    [ValidateSet('test','poll','submit','progress')][string]$Action='progress',
    [int]$Testcase=1,
    [long]$GameId=0,
    [string]$Source='bot.cpp'
)
$ErrorActionPreference='Stop'
$headers=@{Authorization='Bearer '+[IO.File]::ReadAllText("$env:USERPROFILE/.config/codingame/token").Trim()}
$api='https://www.codingame.com/api/v1'
$puzzle="$api/puzzles/selenia-city"
$outDir=Join-Path $PSScriptRoot 'results'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$stamp=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
switch ($Action) {
    'test' {
        $body=@{language='C++';code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $Source));testcase=$Testcase} | ConvertTo-Json
        $result=Invoke-RestMethod -Method Post -Uri "$puzzle/test-games?wait=30" -Headers $headers -ContentType 'application/json' -Body ([Text.Encoding]::UTF8.GetBytes($body))
    }
    'poll' { $result=Invoke-RestMethod -Uri "$api/test-games/${GameId}?wait=30" -Headers $headers }
    'submit' {
        $body=@{language='C++';code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $Source))} | ConvertTo-Json
        $result=Invoke-RestMethod -Method Post -Uri "$puzzle/submissions" -Headers $headers -ContentType 'application/json' -Body ([Text.Encoding]::UTF8.GetBytes($body))
    }
    'progress' { $result=Invoke-RestMethod -Uri "$puzzle/progress" -Headers $headers }
}
$path=Join-Path $outDir "$stamp-$Action-case$Testcase.json"
$result | ConvertTo-Json -Depth 60 | Set-Content -Encoding UTF8 $path
if($result -is [string]) { [IO.File]::WriteAllText([IO.Path]::ChangeExtension($path,'.log'),$result); $result -split "`n" | Select-Object -Last 10 }
if($result.log) { [IO.File]::WriteAllText([IO.Path]::ChangeExtension($path,'.log'),[string]$result.log); $result.log -split "`n" | Select-Object -Last 10 }
if($Action -eq 'submit' -and $result.submission_id) { Copy-Item (Join-Path $PSScriptRoot $Source) (Join-Path $outDir "submission-$($result.submission_id).cpp") }
$result | Select-Object -Property * -ExcludeProperty log | ConvertTo-Json -Depth 20
Write-Output "Saved: $path"
