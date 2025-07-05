CREATE FUNCTION utility.JsonVectorTable(@JsonText NVARCHAR(max))
RETURNS TABLE
AS
RETURN
(
	WITH AAA AS (
		SELECT value FROM OPENJSON(@JsonText)
	)
	SELECT 
	JSON_VALUE(value, '$[0]') AS CL001,
	JSON_VALUE(value, '$[1]') AS CL002,
	JSON_VALUE(value, '$[2]') AS CL003,
	JSON_VALUE(value, '$[3]') AS CL004,
	JSON_VALUE(value, '$[4]') AS CL005,
	JSON_VALUE(value, '$[5]') AS CL006,
	JSON_VALUE(value, '$[6]') AS CL007,
	JSON_VALUE(value, '$[7]') AS CL008,
	JSON_VALUE(value, '$[8]') AS CL009,
	JSON_VALUE(value, '$[9]') AS CL010,
	JSON_VALUE(value, '$[10]') AS CL011,
	JSON_VALUE(value, '$[11]') AS CL012,
	JSON_VALUE(value, '$[12]') AS CL013,
	JSON_VALUE(value, '$[13]') AS CL014,
	JSON_VALUE(value, '$[14]') AS CL015
	FROM AAA
);
GO
------------------------------------------------------------
DECLARE @csv NVARCHAR(MAX) = 'Alice,Bob,Charlie,David';

WITH Tally (n) AS (
    SELECT TOP (LEN(@csv)) ROW_NUMBER() OVER (ORDER BY (SELECT NULL)) FROM master.dbo.spt_values
),
SplitCTE AS (
    SELECT 
        SUBSTRING(@csv, n, CHARINDEX(',', @csv + ',', n) - n) AS value,
        ROW_NUMBER() OVER (ORDER BY n) AS ordinal
    FROM Tally
    WHERE n = 1 OR SUBSTRING(@csv, n - 1, 1) = ','
)
SELECT * FROM SplitCTE;

----------
SELECT * FROM utility.JsonVectorTable(
N'[["BWV 140 & 147/Harnoncourt","J.S.Bach Cantatas BWV 140 & 147","Wachet auf, ruft uns die Stimme","","1984","","J.S.Bach(Cantata)","17","01:00:12"],["後宮からの/Christie","後宮からの誘拐 K.384","William Christie/Les Arts Florissants","Jürg Löw, Alan Ewing, Sophie Lamotte d''Argy, Iain Paton, William Christie, Ian Bostridge, Christine Schäfer, Patricia Petibon","1997","W.A.Mozart","Opera","39","02:06:39"]]')
------------------------------------------------------------
基本的な PIVOT の構文
SELECT <列1>, <列2>, ..., <ピボット列の値> 
FROM 
(
    <元のクエリ>
) AS ソーステーブル
PIVOT (
    集約関数(<集約対象列>)
    FOR <ピボット列> IN (<ピボット列の値リスト>)
) AS ピボットテーブル;

--------------------------------------------
;WITH SRC AS (
	SELECT F2,
	ROW_NUMBER() OVER (ORDER BY (SELECT NULL)) AS [key]
	FROM [dbo].Table03
), SRC2 AS (
	SELECT	SRC.[key],
			SS.ordinal,
			SS.value
	FROM SRC
	CROSS APPLY (
		SELECT ordinal, value FROM STRING_SPLIT(SRC.F2,',',1)
	) AS SS
)
SELECT T.[key], VLS.*
FROM (SELECT [key] FROM SRC) AS T
CROSS APPLY (
	SELECT [1],[2],[3],[4],[5],[6],[7]
	FROM SRC2
	PIVOT (
		MAX(value) FOR ordinal IN ([1],[2],[3],[4],[5],[6],[7])
	) AS PV
	WHERE [key]=T.[key]
) AS VLS
--------------------------------------
CREATE FUNCTION [utility].[csv_extension] 
(
	@table_name nvarchar(128),
	@target_column nvarchar(128),
	@mumber_of_items int,
	@fieldterminator nvarchar(4),
	@key_column nvarchar(128),
	@where_clause nvarchar(128)
)
RETURNS nvarchar(max)
AS
BEGIN
	SET @key_column = COALESCE(@key_column, N'ROW_NUMBER() OVER (ORDER BY (SELECT NULL))')
	SET @where_clause = COALESCE(@where_clause, N'1=1')

	DECLARE @enum_expr nvarchar(max);

	WITH AAA AS (
		SELECT TOP(@mumber_of_items)
		ROW_NUMBER() OVER (ORDER BY (SELECT NULL)) AS rn
		FROM sys.all_objects
	)
	SELECT @enum_expr = CONCAT(N'[', STRING_AGG(rn, N'],[') WITHIN GROUP (ORDER BY rn ASC), N']')
	FROM AAA
	
	DECLARE @expr nvarchar(max) =
	CONCAT('WITH SRC AS (', CHAR(10),
	'    SELECT ', @target_column, ' AS tgt,', CHAR(10),
	'    ', @key_column, ' AS [key]', CHAR(10),
	'    FROM ', @table_name, CHAR(10),
	'    WHERE ', @where_clause, CHAR(10),
	'), SRC2 AS (', CHAR(10),
	'    SELECT SRC.[key], SS.ordinal, SS.value', CHAR(10),
	'    FROM SRC', CHAR(10),
	'    CROSS APPLY (', CHAR(10),
	'        SELECT ordinal, value FROM STRING_SPLIT(SRC.tgt,''', @fieldterminator, ''',1)', CHAR(10),
	'    ) AS SS', CHAR(10),
	')', CHAR(10),
	'SELECT T.[key], VLS.*', CHAR(10),
	'FROM (SELECT [key] FROM SRC) AS T', CHAR(10),
	'CROSS APPLY (', CHAR(10),
	'    SELECT ', @enum_expr, CHAR(10),
	'    FROM SRC2', CHAR(10),
	'    PIVOT (', CHAR(10),
	'        MAX(value) FOR ordinal IN (', @enum_expr, ')', CHAR(10),
	'    ) AS PV', CHAR(10),
	'    WHERE [key]=T.[key]', CHAR(10),
	') AS VLS', CHAR(10),
	'ORDER BY 1');

	RETURN @expr
END
######################################################################
CREATE TABLE json.table00(
	value nvarchar(max)
)

CREATE TABLE json.table_vec(
	num int,
	value nvarchar(max)
)
-----------------------------------
TRUNCATE TABLE json.table_vec;

INSERT INTO json.table_vec
SELECT CA.[key], CA.[value]
FROM json.table00 AS T
CROSS APPLY (
	SELECT [key], [value] FROM OPENJSON(T.value)
) AS CA;
-----------------------------------
SELECT T0.num,CA.[key],CA.[value]
FROM json.table_vec AS T0
CROSS APPLY (
	SELECT * FROM OPENJSON(T0.value, '$.person')
	WHERE [key] IN ('lastName','firstName')
) AS CA;
-----------------------------------
WITH SRC AS (
	SELECT T0.num,CA.[key],CA.[value]
	FROM json.table_vec AS T0
	CROSS APPLY (
		SELECT * FROM OPENJSON(T0.value, '$.person')
		WHERE [key] IN ('lastName','firstName')
	) AS CA
)
SELECT num,PV.[firstName],PV.[lastName]
FROM SRC
PIVOT (
	MAX(value) FOR [key] IN ([firstName],[lastName])
) AS PV
######################################################################
class myConverter(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, pyodbc.Row):
            return dict(enumerate(obj, start=1))
        elif isinstance(obj, datetime.datetime):
            return obj.strftime("%Y-%m-%d, %H:%M:%S")
        elif isinstance(obj, datetime.date):
            return obj.strftime("%Y-%m-%d")
        elif isinstance(obj, datetime.time):
            return obj.strftime("%H:%M:%S")
        elif isinstance(obj, decimal.Decimal):
            return format(obj.normalize(), 'f')
        else:
            return super().default(obj)


SELECT * FROM OPENJSON(@expr)
WITH (
  [1] nvarchar(50),
  [2] date,
  [3] date
)
----------
SELECT TABLE_NAME,ORDINAL_POSITION,COLUMN_NAME,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,NUMERIC_PRECISION,NUMERIC_SCALE,DATETIME_PRECISION
 FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_NAME='Table02'
-------------------------------------------
こっちの方が最適化が効きやすいかも？
SELECT T.col1, T.col2
FROM (
  SELECT DISTINCT col1 FROM tableA
) AS d
CROSS APPLY (
  SELECT TOP 1 col2 
  FROM tableA 
  WHERE col1 = d.col1 
  ORDER BY col2
) AS T

---------------------------------------------------
ALTER PROCEDURE [dbo].[try_truncate]
	@target		nvarchar(64),
	@wait		time(0),
	@retrycount int
AS
BEGIN
	SET NOCOUNT ON;
	DECLARE @truncate_stmt nvarchar(128)=CONCAT(N'TRUNCATE TABLE ', @target)
	DECLARE @wait_stmt nvarchar(64)=CONCAT(N'WAITFOR DELAY ''', @wait, '''')
	DECLARE @success BIT=0;

	PRINT @truncate_stmt
	PRINT @wait_stmt

	SET LOCK_TIMEOUT 2000;
	WHILE 0<@retrycount AND @success=0
	BEGIN
		BEGIN TRANSACTION
		BEGIN TRY
			EXECUTE sp_executesql @truncate_stmt
			COMMIT TRANSACTION
			SET @success=1
			PRINT 'TRUNCATEされました'
		END TRY
		BEGIN CATCH
			IF XACT_STATE() != 0
			BEGIN
				ROLLBACK TRANSACTION;
				PRINT 'ROLLBACKされました'
			END
			PRINT CONCAT(@retrycount, ': ちょっと待ちます')
			SET @retrycount -= 1;
			EXECUTE sp_executesql @wait_stmt
		END CATCH
	END

	IF @success=0
	BEGIN
		PRINT '諦めました'
		RETURN
	END
	
	PRINT '最後'
END
