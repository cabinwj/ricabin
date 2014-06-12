#include "hc_cmysql.h"
#include <iostream>

enum {
    DB_SERVICE_SUCCESS = 0,
    DB_QUERY_RES_NOT_EXIST,
    DB_QUERY_RES_ROW_ILLOGIC,
    DB_QUERY_RES_FETCH_ROW_ERROR,
    DB_QUERY_RES_FETCH_FIELD_ERROR,
    DB_QUERY_RES_FIELD_ILLOGIC,
    DB_CMYSQL_EXCEPTION,
};

// 以下是 xcb 服务器 用到的枚举对应的 C++结构 。

enum {
    EContest_Level    = 1,            //等级排名
};

const int MAX_ROLE_LEVEL_CONTEST_NUMBER = 1000;

typedef struct _contestLevelSave
{
    UINT16 wNumber;
    UINT64 qwXCBID[MAX_ROLE_LEVEL_CONTEST_NUMBER];
    UINT16 wOldPlace[MAX_ROLE_LEVEL_CONTEST_NUMBER];
    UINT16 wLevel[MAX_ROLE_LEVEL_CONTEST_NUMBER];
    UINT16 wExpPersent[MAX_ROLE_LEVEL_CONTEST_NUMBER];
}contestLevelSave;



int main()
{
    // test db read
    CMysql cMysql;
    int levelType = (int)EContest_Level;
    try
    {
        cMysql.init("127.0.0.1", "root", "123456", 3306, "utf8");
        std::ostringstream ossSqlR;
        ossSqlR << "SELECT type,contestinfo FROM xcb.contestinfo WHERE type=" << levelType;

        cMysql << ossSqlR.str() << std::endl;
        std::cout << "SQL='" << ossSqlR.str() << "'" << std::endl;

        unsigned int nResultRows = cMysql.resultRows();
        // warning
        if (0 == nResultRows)
        {
            std::cout << "DB_QUERY_RES_NOT_EXIST " << std::endl;
            return DB_QUERY_RES_NOT_EXIST;
        }

        // if only rows
        if (1 == nResultRows)
        {
            if (!cMysql.nextRow())
            {
                std::cout << "DB_QUERY_RES_FETCH_ROW_ERROR " << std::endl;
                return DB_QUERY_RES_FETCH_ROW_ERROR;
            }

            contestLevelSave* contestLevelInfo = (contestLevelSave*)(cMysql.fieldValue("contestinfo"));
            std::ostringstream ossContestInfo;
            ossContestInfo << "读取等级排名（共" << contestLevelInfo->wNumber << " ）如下：\n"
                << "XCBID: \t LEVEL: \t EXPPERSENT: \t NEWPLACE: \t OLDPLACE: \t\n\n";
            for (int i = 0; i < contestLevelInfo->wNumber; i++ )
            {
                ossContestInfo << contestLevelInfo->qwXCBID << "\t"
                    << contestLevelInfo->wLevel << "\t"
                    << contestLevelInfo->wExpPersent << "\t"
                    << i << "\t"
                    << contestLevelInfo->wOldPlace << "\t\n";
            }

            std::cout << ossContestInfo.str() << std::endl;

			//// get contestinfo field
            //std::istringstream issContestinfo(cMysql.fieldString("contestinfo"));

			//// if string and return 
			//std::string sContestinfo(issContestinfo.str());

			//// if int
			//unsigned int unContestinfo = 0;
			//issContestinfo >> unContestinfo;
        }

   //     // warning (opt)
   //     if (1 != nResultRows)
   //     {
   //         std::cout << "DB_QUERY_RES_ILLOGIC " << std::endl;
   //         return DB_QUERY_RES_ROW_ILLOGIC;
   //     }

   //     // next row
   //     for ( unsigned int i = 0; i < nResultRows; i++ )
   //     {
   //         if (!cMysql.nextRow())
   //         {
   //             std::cout << "DB_QUERY_RES_FETCH_ROW_ERROR " << std::endl;
   //             return DB_QUERY_RES_FETCH_ROW_ERROR;
   //         }

			//// fetch row 
   //         std::cout << "" << std::endl;
   //     }
    }
    catch (CMysqlException& e)
    {
        std::cout << e.showMessage() << std::endl;
        return DB_CMYSQL_EXCEPTION;
    }

    //// test db write
    //try 
    //{
    //    std::string sContestinfo("wangxianming");
    //    cMysql.reInit();
    //    std::ostringstream ossSqlW;
    //    ossSqlW << "UPDATE xcb.contestinfo SET contestinfo='" << CMysql::escapeString(sContestinfo) << "' WHERE type=" << levelType;

    //    cMysql << ossSqlW.str() << std::endl;
    //    std::cout << "SQL='" << ossSqlW.str() << "'" << std::endl;

    //    if (1 != cMysql.affectedRows())
    //    {
    //        std::cout << "DB_QUERY_RES_FIELD_ILLOGIC " << std::endl;
    //        return DB_QUERY_RES_FIELD_ILLOGIC;
    //    }
    //}
    //catch (CMysqlException& e)
    //{
    //    std::cout << e.showMessage() << std::endl;
    //    return DB_CMYSQL_EXCEPTION;
    //}

    return DB_SERVICE_SUCCESS;
}
