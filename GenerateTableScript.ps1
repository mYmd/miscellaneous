#Get-Module -ListAvailable -Name SqlServer
#Install-Module -Name SqlServer -Scope CurrentUser -Force

#$server = New-Object Microsoft.SqlServer.Management.Smo.Server "YourServerName"
#$scripter = New-Object Microsoft.SqlServer.Management.Smo.Scripter($server)
# オプションの確認
#$scripter.Options | Format-List
#.\GenerateTableScript.ps1 -serverName "" -databaseName "" -schemaName "" -tableName ""

param (
    [string]$serverName = "localhost",  # サーバー名
    [string]$databaseName = "YourDatabase",  # データベース名
    [string]$schemaName = "dbo",  # スキーマ名
    [string]$tableName = "YourTable"  # テーブル名
)

# SQL Server SMOのロード
Import-Module SqlServer

# サーバーへの接続
$server = New-Object Microsoft.SqlServer.Management.Smo.Server $serverName
$database = $server.Databases[$databaseName]

# テーブルの取得
$table = $database.Tables[$tableName, $schemaName]

# スクリプトオプションの設定
$scrp = New-Object Microsoft.SqlServer.Management.Smo.Scripter ($server)
$scrp.Options.ScriptDrops = $false
$scrp.Options.WithDependencies = $false
$scrp.Options.Indexes = $true
$scrp.Options.DriAllConstraints = $true
$scrp.Options.Triggers = $true
$scrp.Options.NoCollation = $false  # 照合順序を含める

# スクリプトの生成
$script = $scrp.Script($table)
$scriptText = $script -join "`n"

# スクリプトの出力
Write-Output $scriptText
