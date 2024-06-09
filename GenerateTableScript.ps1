#Get-Module -ListAvailable -Name SqlServer
#Install-Module -Name SqlServer -Scope CurrentUser -Force

#$server = New-Object Microsoft.SqlServer.Management.Smo.Server "YourServerName"
#$scripter = New-Object Microsoft.SqlServer.Management.Smo.Scripter($server)
# オプションの確認
#$scripter.Options | Format-List
#.\GenerateTableScript.ps1 -serverName "" -databaseName "" -schemaName "" -entityName "" -is_table $true

param (
    [string]$serverName,  # サーバー名
    [string]$databaseName,  # データベース名
    [string]$schemaName,  # スキーマ名
    [string]$entityName,  # テーブル名
    [bool]$is_table
)

# SQL Server SMOのロード
Import-Module SqlServer

# サーバーへの接続
$server = New-Object Microsoft.SqlServer.Management.Smo.Server $serverName
$database = $server.Databases[$databaseName]

# テーブルの取得
if ($is_table)
{
	$entity = $database.Tables[$entityName, $schemaName]
}
else
{
	$entity = $database.StoredProcedures[$entityName, $schemaName]
}
# スクリプトオプションの設定
$scrp = New-Object Microsoft.SqlServer.Management.Smo.Scripter ($server)
$scrp.Options.DriAll  = $true
<#
#$scrp.Options.ScriptDrops = $true
$scrp.Options.WithDependencies = $false
$scrp.Options.Indexes = $true
$scrp.Options.DriAllConstraints = $true
$scrp.Options.Triggers = $true
$scrp.Options.NoCollation = $false  # 照合順序を含める
#>
# スクリプトの生成
$script = $scrp.Script($entity)
$scriptText = $script -join "`n"

# スクリプトの出力
Write-Output $scriptText
