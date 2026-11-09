param([string]$Action='test',[string]$Language='Bash',[string]$File='golf.sh',[int]$Case=1,[string]$Id='')
$ErrorActionPreference='Stop'
$headers=@{Authorization='Bearer '+(Get-Content -Raw (Join-Path $env:USERPROFILE '.config/codingame/token')).Trim()}
$base='https://www.codingame.com/api/v1'
$puzzle="$base/puzzles/don%27t-panic"
switch($Action){
 'test' {$url="$puzzle/test-games?wait=30"}
 'submit' {$url="$puzzle/submissions"}
 'poll' {$url="$base/test-games/${Id}?wait=30"}
 default {$url="$puzzle/$Action"}
}
if($Action -in @('test','submit')){
 $code=[IO.File]::ReadAllText((Join-Path $PSScriptRoot $File)).TrimEnd("`r","`n")
 $body=@{language=$Language;code=$code}
 if($Action -eq 'test'){$body.testcase=$Case}
 while($true){
  try{
   $result=Invoke-RestMethod -Uri $url -Headers $headers -Method Post -ContentType 'application/json' -Body (ConvertTo-Json -Compress $body)
   break
  }catch{
   $problem=$null
   try{$problem=$_.ErrorDetails.Message|ConvertFrom-Json}catch{}
   if($problem.status -ne 429){throw}
   $delay=[Math]::Max(1,[int]$problem.retry_after_seconds)
   Write-Warning "CodinGame rate limit: waiting $delay seconds."
   while($delay -gt 0){$part=[Math]::Min(30,$delay);Start-Sleep -Seconds $part;$delay-=$part}
  }
 }
 $result|Add-Member -NotePropertyName local_source -NotePropertyValue $File
 $result|Add-Member -NotePropertyName local_characters -NotePropertyValue $code.Length
 $result|Add-Member -NotePropertyName local_code -NotePropertyValue $code
}else{$result=Invoke-RestMethod -Uri $url -Headers $headers}
$dir=Join-Path $PSScriptRoot 'results'
[IO.Directory]::CreateDirectory($dir)|Out-Null
$stamp=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$result|ConvertTo-Json -Depth 50|Set-Content -Encoding utf8 (Join-Path $dir "$stamp-$Action-$Language-$Case.json")
$result|ConvertTo-Json -Depth 50
