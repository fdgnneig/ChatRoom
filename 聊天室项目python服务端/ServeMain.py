import socket
import struct
import threading
import enum
import time
from MysqlMethod import *

class MsgType(enum.Enum):
    RegisterMessage = 0 #注册消息
    LoginMessage = 1 #登陆消息
    ChatMessage = 2#聊天消息
    FindUser = 3#获取用户消息
    GetHistoryMsg = 4#获得聊天记录消息
    ChatOne = 5 #私聊消息
    Friend=6 #加好友
    ShowFriendinfo = 7#显示好友

class QQServer(object):
    # 保存登录用户的socket和用户名，用户名作为主键检索用户的socket
    userlist = {}
    lock = threading.Lock()##创建一个锁,用于规范多个线程在同一时间只能由一个线程执行消息处理函数
    #构造函数，为网络连接做准备
    def __init__(self):
        # 初始化套接字
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 绑定端口，供客户端连接
        address = ("0.0.0.0", 12345)
        self.server.bind(address)
        # 监听
        self.server.listen()

    def MyAccept(self):
        # 创建用于接收连接的线程
        t = threading.Thread(target=self.Accept)
        t.start()

    #接收客户端连接，持续接收运行，函数会阻塞
    def Accept(self):
        while True:
            socketClient,addrClient = self.server.accept()
            socketThread = threading.Thread(target=self.Receive, args=(socketClient,))
            socketThread.start()

    def Receive(self, socketClient):
        while True:
            try:
                message = socketClient.recv(4500)
                # 获取发送过来的消息类型
                time.sleep(0.1)
                msgType, = struct.unpack("i", message[:4])
                self.func[msgType](self, socketClient, message)
            #这里如果客户离线要怎么处理？
            # 这里如果客户端关闭，解包函数会发生错误，此时需要错误处理，并关闭socket连接，将客户移出在线列表
            except Exception as error:
                print(error)
                name = QQServer.userlist.get(socketClient)
                if name == None:
                    return
                socketClient.close()
                name = QQServer.userlist.pop(socketClient)
                #这里需要重新更新在线客户显示
                return

    def Register(self,socketClient, message):
        hwnd, = struct.unpack("i", message[4:8])
        msgLen, = struct.unpack("i", message[8:12])  # 获得消息长度
        nameLen, pwdLen, name, pwd, sex = struct.unpack("ii40s40si", message[12:12 + msgLen])

        # 注意这里传入进来的是用户名和密码的字符数，因为在用户名和密码是wchar_t类型的，所以实际切片的字节数应该乘以2
        name = name[0:nameLen * 2]
        pwd = pwd[0:pwdLen * 2]
        name = name.decode('utf-16')
        pwd = pwd.decode('utf-16')
        print("%s,%s,%d" % (name, pwd, sex))
        ##################################################
        # 创建数据库连接
        con = CSqlForChat()
        if sex == 0:  #这里采用md5加密存储密码
            sql = """insert into user_table(user_name,user_pass,user_sex)values('%s',MD5('%s'),'%s');""" % (str(name), str(pwd), "female")
        else:
            sql = """insert into user_table(user_name,user_pass,user_sex)values('%s',MD5('%s'),'%s');""" % (str(name), str(pwd), "male")

        # 返回的res是一个元组
        res = con.insert(sql)
        print(res)
        if res == None:
            socketClient.send(struct.pack('iii20s', MsgType.LoginMessage.value, hwnd, 0, '注册失败'.encode('utf16')))
        else:
            socketClient.send(struct.pack('iii20s', MsgType.LoginMessage.value, hwnd, 0, '注册成功'.encode('utf16')))

    def Login(self, socketClient, message):
        hwnd,= struct.unpack("i", message[4:8])
        msgLen, = struct.unpack("i", message[8:12])  # 获得消息长度
        nameLen, pwdLen, name, pwd = struct.unpack("ii40s40s", message[12:12 + msgLen])

        # 注意这里传入进来的是用户名和密码的字符数，因为在用户名和密码是wchar_t类型的，所以实际切片的字节数应该乘以2
        name = name[0:nameLen * 2]
        pwd = pwd[0:pwdLen * 2]

        name = name.decode('utf-16')
        pwd = pwd.decode('utf-16')
        print("%s,%s" % (name, pwd))
        ##################################################
        # 创建数据库连接
        con = CSqlForChat()
        sql = """select user_id from user_table where user_name='%s' and user_pass=MD5('%s');""" % (str(name), str(pwd))

        # 返回的res是一个元组
        res = con.query(sql)
        print(res)
        # test = ([],0)

        if res[1] == 0:  # 如果结果元组为空，说明sql语句执行失败
            socketClient.send(struct.pack('iii20s', MsgType.LoginMessage.value, hwnd, 0, '用户名或密码错误'.encode('utf16')))
        else:
            QQServer.userlist[name] = socketClient#保存在线用户
            # 这里将长度数据包中的长度置为1作为登陆成功的标记
            socketClient.send(struct.pack('iii20s', MsgType.LoginMessage.value, hwnd, 1, '登陆成功'.encode('utf16')))

    def Chat(self,socketClient, message):

        #server.lock.acquire()  # 获取锁

        hwnd, = struct.unpack("i", message[4:8])
        msgLen, = struct.unpack("i", message[8:12])  # 获得聊天消息长度
        chatMsg, = struct.unpack("2048s", message[12:12 + 2048])#注意这里需要先将数据包整体切出来，然后再根据之前获得的消息长度，获得聊天消息的正文
        chatMsg = chatMsg[0:msgLen]
#        chatMsg = chatMsg.decode('utf16').rstrip('\x00')
        chatMsg = chatMsg.decode('utf16')#这里聊天字段已经被截取了，所以不需要用0进行截断

        ###############################################################
       #  #保存聊天记录
        con = CSqlForChat()
        sql = """insert into chat_message(chat_messageinfo)values('%s');""" % str(chatMsg)
        #sql = """insert into chat_chat (msg) values ('%s');"""%("fffffef")

        #sql = """insert into user_table(user_name,user_pass,user_sex)value('%s',MD5('%s'),'%s');""" % (str(name), str(pwd), "male")

        # 返回的res是一个元组
        res = con.insert(sql)
        #res = con.query("SELECT * from userinfo where name=%s and pwd=%s",(name,pwd))
        #res = con.query("insert into chat_message(chat_messageinfo)values(%s)", (chatMsg))

        print(res)
        ##################################################
        for csocket in QQServer.userlist.values():#遍历字典的值，向每一个上线的sockte发送该消息
            csocket.send(message)

        #server.lock.release()  # 释放锁


    def Usershow(self,socketClient, message):
        # # 查找当前用户的的所有好友
        # con = CSqlForChat()
        # sql = """select user_name from user_table;"""
        # # 返回的res列表，每一个元素是用户名指针，len是一个列表元素个数
        # res,len = con.query(sql)
        # print(res)
        # print(len)
        # for index in range(len):
        #      print(res[index])
        # for index in range(len):#将所有用户名发送到客户端
        #     socketClient.send(struct.pack('iii2048s', MsgType.FindUser.value, 0, 0, res[index][0].encode('UTF16')))
        #     time.sleep(0.5)#每隔一秒发送一个数据包给客户端，避免短时间发送多条数据造成窗口与运行错误

        for name in QQServer.userlist.keys():#遍历字典的值，向每一个上线的sockte发送该消息
            socketClient.send(struct.pack('iii2048s', MsgType.FindUser.value, 0, 0, name.encode('UTF16')))
            time.sleep(0.5)

    def GetHistory(self, socketClient, message):
        #  查找历史聊天记录
        con = CSqlForChat()
        sql = """select chat_messageinfo from chat_message;"""
        #  返回的res列表，每一个元素是用户名指针，len是一个列表元素个数
        res,len = con.query(sql)
        # print(res)
        # print(len)
        # for index in range(len):
        #     print(res[index])
        for index in range(len):#将所有用户名发送到客户端
            socketClient.send(struct.pack('iii2048s', MsgType.GetHistoryMsg.value, 0, 0, res[index][0].encode('UTF16')))
            time.sleep(0.5)#每隔一秒发送一个数据包给客户端，避免短时间发送多条数据造成窗口与运行错误

    def ChatOnePerson(self,socketClient, message):
        another, = struct.unpack("20s", message[4:24])
        another = another.decode('utf16').rstrip('\x00쳌')#获取私聊的对手名
        msgLen, = struct.unpack("i", message[24:28])  # 获得聊天消息长度
        chatMsg, = struct.unpack("2048s", message[28:28 + 2048])#注意这里需要先将数据包整体切出来，然后再根据之前获得的消息长度，获得聊天消息的正文
        chatMsg = chatMsg[0:msgLen]
        chatMsg = chatMsg.decode('utf16')
        print(another)
        print(msgLen)
        print(chatMsg)

        for csocket in QQServer.userlist.values():#遍历字典的值，向每一个上线的sockte发送该消息
            if csocket == socketClient or csocket == QQServer.userlist[another]:
                csocket.send(message)

    def AddFriend(self,socketClient, message):
        msgLen, = struct.unpack("i", message[8:12])  # 获得消息长度
        name1Len, name2Len, name1, name2,= struct.unpack("ii40s40s", message[12:12 + msgLen])
        name1 = name1[0:name1Len * 2]
        name2 = name2[0:name2Len * 2]
        name1 = name1.decode('utf-16')
        name2 = name2.decode('utf-16')
        print("%s,%s" % (name1, name2))
##############################################################
        ##################################################
        # 创建数据库连接
        con = CSqlForChat()
       #sql = """insert into user_table(user_name,user_pass,user_sex)values('%s',MD5('%s'),'%s');""" % (str(name), str(pwd), "female")
        sql = """insert into user_friend(user_name1,user_name2)values('%s','%s');""" % (str(name1), str(name2))
        # 返回的res是一个元组
        res = con.insert(sql)
        print(res)

        con = CSqlForChat()
        sql2 = """insert into user_friend(user_name1,user_name2)values('%s','%s');""" % (str(name2), str(name1))
        res2 = con.insert(sql2)
        print(res2)

        if res == None or res2==None:
            socketClient.send(struct.pack('iii20s', MsgType.Friend.value, 0, 0, '添加好友失败'.encode('utf16')))
        else:
            socketClient.send(struct.pack('iii20s', MsgType.Friend.value, 0, 0, '添加好友成功'.encode('utf16')))

    def FriendShow(self,socketClient, message):
        #  查找历史聊天记录
        # #找到当前用户的用户名
        for key, val in QQServer.userlist.items():
            if val == socketClient:
                name = key

        con = CSqlForChat()
        sql = """select user_name2 from user_friend where user_name1='%s';""" % (str(name))
        #  返回的res列表，每一个元素是用户名指针，len是一个列表元素个数
        res,len = con.query(sql)
        # print(res)
        # print(len)
        # for index in range(len):
        #     print(res[index])
        for index in range(len):#将所有用户名发送到客户端
            socketClient.send(struct.pack('iii2048s', MsgType.ShowFriendinfo.value, 0, 0, res[index][0].encode('UTF16')))
            time.sleep(0.5)#每隔一秒发送一个数据包给客户端，避免短时间发送多条数据造成窗口与运行错误



    #该字典用于以一种简便的方法，寻址函数
    func = {
        0: Register,
        1: Login,
        2: Chat,
        3: Usershow,
        4: GetHistory,
        5: ChatOnePerson,
        6: AddFriend,
        7: FriendShow,
    }

server = QQServer()
server.MyAccept()





