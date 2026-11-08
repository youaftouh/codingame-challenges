param(
    [ValidateSet('test', 'submit', 'progress')][string]$Action = 'test',
    [int]$Case = 1,
    [ValidateSet('Python3','C++')][string]$Language = 'C++'
)
$ErrorActionPreference = 'Stop'
$headers = @{ Authorization = 'Bearer ' + (Get-Content -Raw (Join-Path $HOME '.config/codingame/token')).Trim() }
$api = 'https://www.codingame.com/api/v1'
$root = "$api/puzzles/code-vs-zombies"
New-Item -ItemType Directory -Force "$PSScriptRoot/results" | Out-Null
if ($Action -eq 'progress') {
    $result = Invoke-RestMethod "$root/progress" -Headers $headers
} else {
    $source = if ($Language -eq 'C++') { 'bot.cpp' } else { 'bot.py' }
    $body = @{language = $Language; code = [string](Get-Content -Raw "$PSScriptRoot/$source")}
    if ($Action -eq 'test') { $body.testcase = $Case; $url = "$root/test-games?wait=30" }
    else { $url = "$root/submissions" }
    $result = Invoke-RestMethod $url -Method Post -Headers $headers -ContentType 'application/json' -Body ($body | ConvertTo-Json -Compress)
    while ($result.status -and $result.status -notin @('done', 'failed')) {
        $result = Invoke-RestMethod "$api/test-games/$($result.test_game_id)?wait=30" -Headers $headers
    }
}
$stamp = Get-Date -Format 'yyyyMMdd-HHmmss-fff'
if ($Action -ne 'progress') {
    $result | Add-Member -NotePropertyName local_language -NotePropertyValue $Language
    if ($Action -eq 'test') { $result | Add-Member -NotePropertyName local_testcase -NotePropertyValue $Case }
    $result | Add-Member -NotePropertyName local_source_sha256 -NotePropertyValue (Get-FileHash "$PSScriptRoot/$source" -Algorithm SHA256).Hash
}
if ($Action -eq 'submit' -and $result.submission_id) {
    Copy-Item -LiteralPath "$PSScriptRoot/$source" -Destination "$PSScriptRoot/results/submission-$($result.submission_id)-$source"
}
$result | ConvertTo-Json -Depth 40 | Set-Content "$PSScriptRoot/results/$stamp-$Action-$Case.json"
if ($result.log) { $result.log | Set-Content "$PSScriptRoot/results/$stamp-$Action-$Case.log"; $result.log }
$result | Select-Object -Property * -ExcludeProperty log | ConvertTo-Json -Depth 30

