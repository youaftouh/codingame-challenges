param([string]$Language,[string]$File,[int]$Start=1,[int]$End=7)
for($case=$Start;$case -le $End;$case++){
 $r=(& "$PSScriptRoot/api.ps1" -Language $Language -File $File -Case $case)|ConvertFrom-Json
 while($r.status -notin @('done','failed')){$r=(& "$PSScriptRoot/api.ps1" -Action poll -Id $r.test_game_id)|ConvertFrom-Json}
 $ok=$r.success -eq $true -or $r.log -match 'scores: 0=1(?:\s|$)'
 @{language=$Language;case=$case;pass=$ok;limits=$r.limits.test_games}|ConvertTo-Json -Compress
 if(!$ok){$r|ConvertTo-Json -Depth 30;break}
 if($r.limits.test_games.remaining -eq 0){Start-Sleep -Seconds ([Math]::Max(1,$r.limits.test_games.resets_in_seconds))}
}
