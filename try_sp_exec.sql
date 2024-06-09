CREATE PROCEDURE dbo.try_sp_exec
@expr nvarchar(2000)
AS
BEGIN
DECLARE @timestamp datetime2(0)
SET @timestamp = GETUTCDATE() AT TIME ZONE 'UTC' AT TIME ZONE 'Tokyo Standard Time'
	BEGIN TRY
		execute sp_executesql @expr;
		INSERT INTO dbo.splog VALUES (@timestamp, @expr, 1);
	END TRY
	BEGIN CATCH
		INSERT INTO dbo.splog VALUES (@timestamp, @expr, 0);
	END CATCH
END
--------------------------------------------------------
CREATE TABLE [dbo].[splog](
	[timestamp] [datetime2](0) NOT NULL,
	[expr] [nchar](2000) COLLATE Japanese_CI_AS NOT NULL,
	[result] BIT NOT NULL
) ON [PRIMARY]
--------------------------------------------------------
DECLARE @expr NVARCHAR(2000)
SELECT @expr = STRING_AGG(CONCAT('exec dbo.try_sp_exec ''',value, '''' ), ';')
FROM STRING_SPLIT('SELECT 2+3_32423', '_')

exec sp_executesql @expr