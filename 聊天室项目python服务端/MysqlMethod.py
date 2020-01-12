
import mysql.connector
class CSqlForChat():
    def __init__(self):
        #1.连接数据库
        config = {'host':'192.168.8.129',
                  'user':'root',
                  'password':'root',
                  'port':3306,
                  'database':'projectchatroom',
                  #'charset':'utf16'
                  }
        print("正在连接数据库...")
        self.conn = mysql.connector.connect(**config)
        print("数据库连接成功!")
    #解除sql连接
    def __del__(self):
        self.conn.close()

    #在查询与插入sql语句的函数中，使用错误处理（try except）结合 #发生错误时回滚函数，获知sql是否正确执行
    #查询sql
    def query(self,szSql,param=None):
        cursor = self.conn.cursor()
        try:
            cursor.execute(szSql, param)
            result = (cursor.fetchall(), cursor.rowcount)
            cursor.close()
            return result
        except Exception as Msg:
            print(Msg)
            cursor.close()
            #发生错误时回滚
            self.conn.rollback()
            return None

    #插入sql
    def insert(self,szSql,param = None):
        cursor = self.conn.cursor()
        try:
            cursor.execute(szSql,param)
            result = cursor.rowcount
            self.conn.commit()
            cursor.close()
            return result
        except:
            cursor.close()
            #发生错误时回滚
            self.conn.rollback()
            return None

