param([string]$Source = 'bot.vb', [int[]]$Cases = @(1), [string]$Label = 'candidate')
$ErrorActionPreference = 'Stop'
$headers = @{Authorization = 'Bearer ' + (Get-Content "$HOME/.config/codingame/token" -Raw).Trim()}
$api = 'https://www.codingame.com/api/v1'
foreach ($case in $Cases) {
    $body = @{language='VB.NET'; code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $Source)); testcase=$case} | ConvertTo-Json
    $r = Invoke-RestMethod "$api/puzzles/codingame-sponsored-contest/test-games?wait=30" -Headers $headers -Method Post -ContentType 'application/json' -Body $body
    while ($r.status -ne 'done' -and $r.status -ne 'failed') {
        $r = Invoke-RestMethod "$api/test-games/$($r.test_game_id)?wait=30" -Headers $headers
    }
    $r | ConvertTo-Json -Depth 30 | Set-Content (Join-Path $PSScriptRoot "$Label-$case.json")
    Write-Output "Case $case $($r.status) $([regex]::Match($r.log, 'metadata: score=([^\n]+)').Groups[1].Value) tests-left=$($r.limits.test_games.remaining)"
    if ($r.log -match 'error:') { Write-Output $r.log; break }
}
