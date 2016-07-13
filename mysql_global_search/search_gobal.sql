# 0 0 * * * /usr/local/bin/python3 /home/bag/res/conf/schedule_bag.py
# invoking by python...
# search resource gobal...
CREATE DEFINER=`root`@`%` PROCEDURE `test_res_in_db`(IN `in_search` VARCHAR(50))
	LANGUAGE SQL
	NOT DETERMINISTIC
	CONTAINS SQL
	SQL SECURITY DEFINER
	COMMENT ''
BEGIN
DECLARE done INT DEFAULT 0;
DECLARE COUNTER INT;
DECLARE search_string VARCHAR(500);

DECLARE table_cur CURSOR FOR
SELECT concat('SELECT COUNT(*) INTO @CNT_VALUE FROM `',table_schema,'`.`',table_name,'` WHERE `', column_name,'` REGEXP "',in_search,'"') 
FROM information_schema.COLUMNS WHERE TABLE_SCHEMA IN ('bagdb', 'bagtest') and column_type like 'varchar%';
DECLARE CONTINUE HANDLER FOR NOT FOUND SET done=1;
OPEN table_cur;
table_loop:LOOP
FETCH table_cur INTO search_string;
SET @search_string = search_string;
PREPARE search_string FROM @search_string;
EXECUTE search_string;
SET COUNTER = @CNT_VALUE;

IF COUNTER>0 THEN
# terminating loop if res exists...
LEAVE table_loop;
END IF;
# exit looping if exception occur....
IF done=1 THEN
LEAVE table_loop;
END IF;
END LOOP;

CLOSE table_cur;

SELECT COUNTER;


END

