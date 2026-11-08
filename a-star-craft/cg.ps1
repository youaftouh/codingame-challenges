param(
    [ValidateSet('test', 'submit', 'progress')][string]$Action = 'test',
    [int]$Case = 1
)
$ErrorActionPreference = 'Stop'
$headers = @{ Authorization = 'Bearer ' + (Get-Content -Raw (Join-Path $HOME '.config/codingame/token')).Trim() }
$api = 'https://www.codingame.com/api/v1'
$root = "$api/puzzles/a-star-craft"
New-Item -ItemType Directory -Force "$PSScriptRoot/results" | Out-Null
if ($Action -eq 'progress') {
    $result = Invoke-RestMethod "$root/progress" -Headers $headers
} else {
    $body = @{language = 'TypeScript'; code = [string](Get-Content -Raw "$PSScriptRoot/bot.ts")}
    if ($Action -eq 'test') { $body.testcase = $Case; $url = "$root/test-games?wait=30" }
    else { $url = "$root/submissions" }
    $result = Invoke-RestMethod $url -Method Post -Headers $headers -ContentType 'application/json' -Body ($body | ConvertTo-Json -Compress)
    while ($result.status -and $result.status -notin @('done', 'failed')) {
        $result = Invoke-RestMethod "$api/test-games/$($result.test_game_id)?wait=30" -Headers $headers
    }
}
$stamp = Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$result | ConvertTo-Json -Depth 40 | Set-Content "$PSScriptRoot/results/$stamp-$Action-$Case.json"
if ($result.log) { $result.log | Set-Content "$PSScriptRoot/results/$stamp-$Action-$Case.log"; $result.log }
$result | Select-Object -Property * -ExcludeProperty log | ConvertTo-Json -Depth 30
