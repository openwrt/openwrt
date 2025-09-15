import json
import os
import time
import requests

def read_token_from_config():
    config_path = "/usr/bin/smstrun.conf"
    try:
        with open(config_path, 'r') as file:
            token = file.read().strip()  
            if not token:  
                print("未填写token，程序已退出！")
                exit()
            return token
    except FileNotFoundError:
        print("未找到配置文件，程序已退出！")
        exit()

def read_title_from_config():
    title_path = "/usr/bin/smstrun-title.conf"
    try:
        with open(title_path, 'r') as file:
            title = file.read().strip()  
            if not title:
                title = "CPE短信转发标题未定义"
            return title
    except FileNotFoundError:
        print("未找到标题配置文件，默认设置为“CPE短信转发标题未定义”")
        return "CPE短信转发标题未定义"

def forward():
    token = read_token_from_config()  
    print("enjoy！短信转发程序运行成功，你可以设置为开机启动了.")
    while True:
        out = os.popen('/usr/bin/sms_tool2 -ju recv').read()
        if out.strip():  
            try:
                json_txt = json.loads(out)
                msgs = json_txt['msg']
                if msgs:
                    for msg in msgs:
                        title = read_title_from_config()
                        push_pushplus(msg, token, title)
            except json.JSONDecodeError as e:
                print("Error decoding JSON: ", str(e))
        else:
            print("未获取到短信,将在5秒后进行重新读取。")
        time.sleep(5)

def push_pushplus(msg, token, title):
    url = "http://www.pushplus.plus/send"
    data = {
        "token": token,
        "title": title,
        "content": msg['content']
    }
    try:
        response = requests.post(url, json=data)
        result = response.json()
        print("Response:\n", result)
    except Exception as e:
        print("Error occurred: ", str(e))

if __name__ == '__main__':
    forward()