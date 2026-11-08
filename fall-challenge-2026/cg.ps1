param(
    [ValidateSet('test','replay','game','poll','submit','progress','battles','challenge')][string]$Action='progress',
    [int]$Seed=1,
    [long]$GameId=0,
    [string]$Source='bot.cpp',
    [string]$Opponent='boss',
    [switch]$Compact
)
$ErrorActionPreference='Stop'
$headers=@{Authorization='Bearer '+[IO.File]::ReadAllText("$env:USERPROFILE/.config/codingame/token").Trim()}
$api='https://www.codingame.com/api/v1'
$contest="$api/challenges/fall-challenge-2026"
if ($Opponent -eq 'me') { throw 'Use an explicit arena agent ID for baseline comparisons; me creates self-play.' }
$outDir=Join-Path $PSScriptRoot 'results'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$stamp=Get-Date -Format 'yyyyMMdd-HHmmss'
switch ($Action) {
    'test' {
        $body=@{language='C++';code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $Source));opponents=@($Opponent);game_options="seed=$Seed"} | ConvertTo-Json -Depth 10
        $result=Invoke-RestMethod -Method Post -Uri "$contest/test-games?wait=30" -Headers $headers -ContentType 'application/json' -Body ([Text.Encoding]::UTF8.GetBytes($body))
    }
    'replay' {
        $body=@{language='C++';code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $Source));replay_game_id=$GameId} | ConvertTo-Json -Depth 10
        $result=Invoke-RestMethod -Method Post -Uri "$contest/test-games?wait=30" -Headers $headers -ContentType 'application/json' -Body ([Text.Encoding]::UTF8.GetBytes($body))
    }
    'game' { $result=Invoke-RestMethod -Uri "$api/games/$GameId" -Headers $headers }
    'poll' { $result=Invoke-RestMethod -Uri "$api/test-games/${GameId}?wait=30" -Headers $headers }
    'submit' {
        $body=@{language='C++';code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $Source))} | ConvertTo-Json
        $result=Invoke-RestMethod -Method Post -Uri "$contest/submissions" -Headers $headers -ContentType 'application/json' -Body ([Text.Encoding]::UTF8.GetBytes($body))
    }
    'challenge' { $result=Invoke-RestMethod -Uri $contest -Headers $headers }
    default { $result=Invoke-RestMethod -Uri "$contest/$Action" -Headers $headers }
}
$label=[IO.Path]::GetFileNameWithoutExtension($Source)
$path=Join-Path $outDir "$stamp-$Action-$label-vs$Opponent-game$GameId-seed$Seed.json"
$result | ConvertTo-Json -Depth 50 | Set-Content -Encoding UTF8 $path
if ($Action -in @('test','replay','submit')) {
    @{action=$Action;source=$Source;source_sha256=(Get-FileHash -LiteralPath (Join-Path $PSScriptRoot $Source) -Algorithm SHA256).Hash;opponent=$Opponent;seed=$Seed;replay_game_id=$GameId} |
        ConvertTo-Json | Set-Content -Encoding UTF8 ([IO.Path]::ChangeExtension($path,'.meta.json'))
}
if ($result -is [string]) {
    [IO.File]::WriteAllText([IO.Path]::ChangeExtension($path,'.log'),$result)
    $result -split "`n" | Select-Object -Last 18
}
if ($Action -eq 'submit' -and $result.submission_id) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot $Source) -Destination (Join-Path $outDir "submission-$($result.submission_id).cpp")
}
if ($result.log) {
    $logPath=[IO.Path]::ChangeExtension($path,'.log')
    [IO.File]::WriteAllText($logPath,[string]$result.log)
    $result.log -split "`n" | Select-Object -Last 18
}
if ($Compact -and $Action -eq 'progress') {
    $result | Select-Object submission_id,in_progress,percentage,score,rank,league,league_rank,eligible_for_promotion,league_boss,limits | ConvertTo-Json -Depth 10
} else {
    $result | Select-Object -Property * -ExcludeProperty log,statement | ConvertTo-Json -Depth 20
}
Write-Output "Saved: $path"
