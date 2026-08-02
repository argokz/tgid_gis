DECLARE @Database NVARCHAR(255)   
DECLARE @Table NVARCHAR(255)  
DECLARE @cmd NVARCHAR(1000)  
DECLARE DatabaseCursor CURSOR READ_ONLY FOR  
SELECT name FROM master.sys.databases   
--WHERE name NOT IN ('master','msdb','tempdb','model','distribution')  -- исключаемые базы данных
WHERE name IN ('$BAZA$') -- используется для выбора конкретных баз данных (раскомментируйте эту строку)
AND state = 0 -- база данных онлайн
AND is_in_standby = 0 -- база данных не читается только для log shipping
ORDER BY 1  
OPEN DatabaseCursor  
FETCH NEXT FROM DatabaseCursor INTO @Database  
WHILE @@FETCH_STATUS = 0  
BEGIN  
   SET @cmd = 'DECLARE TableCursor CURSOR READ_ONLY FOR SELECT ''['' + table_catalog + ''].['' + table_schema + ''].['' +  
   table_name + '']'' as tableName FROM [' + @Database + '].INFORMATION_SCHEMA.TABLES WHERE table_type = ''BASE TABLE'''   
   -- создаем курсор для таблиц
   EXEC (@cmd)  
   OPEN TableCursor   
   FETCH NEXT FROM TableCursor INTO @Table   
   WHILE @@FETCH_STATUS = 0   
   BEGIN
      BEGIN TRY   
         SET @cmd = 'ALTER INDEX ALL ON ' + @Table + ' REBUILD' 
         --PRINT @cmd -- раскомментируйте, если хотите увидеть команды
         EXEC (@cmd) 
      END TRY
      BEGIN CATCH
         PRINT '---'
         PRINT @cmd
         PRINT ERROR_MESSAGE() 
         PRINT '---'
      END CATCH
      FETCH NEXT FROM TableCursor INTO @Table   
   END   
   CLOSE TableCursor   
   DEALLOCATE TableCursor  
   FETCH NEXT FROM DatabaseCursor INTO @Database  
END  
CLOSE DatabaseCursor   
DEALLOCATE DatabaseCursor
