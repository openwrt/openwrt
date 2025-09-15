import json
import os
import time
import requests
#by:manper 20240318
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
        #print("未找到标题配置文件，默认设置为“CPE短信转发标题未定义”")
        return "CPE短信转发标题未定义"
def forward():
    token = read_token_from_config()
    print("Enjoy! 已完成测试并开启转发功能，重启后完成开机自启。")
    while True:
        out = os.popen('/usr/bin/sms_tool2 -ju recv').read()
        if out.strip():
            try:
                json_txt = json.loads(out)
                msgs = json_txt['msg']
                if msgs:
                    for msg in msgs:
                        if 'reference' not in msg or 'index' not in msg:
                            sender_info = msg.get('sender', '未知发件人')
                            timestamp = msg.get('timestamp', '未知时间') 
                            content = msg['content']
                            message_with_sender_and_time = f"发件人: {sender_info}\n时间: {timestamp}\n{content}"
                            title = read_title_from_config()
                            #push_pushplus(message_with_sender_and_time, token, title)
                        else:
                            filtered_msgs = [msg for msg in msgs if 'reference' in msg and 'index' in msg]
                            filtered_msgs.sort(key=lambda x: (x['reference'], x['index']))
                            current_reference = None
                            full_message = ''
                            sender_info = '未知发件人'  
                            timestamp = '未知时间'  
                            for msg in filtered_msgs:
                                if current_reference != msg['reference']:
                                    if full_message:  
                                        message_with_sender_and_time = f"发件人: {sender_info}\n时间: {timestamp}\n{full_message}"
                                        title = read_title_from_config()
                                        #push_pushplus(message_with_sender_and_time, token, title)
                                        full_message = '' 
                                    current_reference = msg['reference']
                                    sender_info = msg.get('sender', '未知发件人')  
                                    timestamp = msg.get('timestamp', '未知时间') 
                                full_message += msg['content']
                            if full_message: 
                                message_with_sender_and_time = f"发件人: {sender_info}\n收件时间: {timestamp}\n{full_message}" 
                                title = read_title_from_config()
                                pass
                    push_pushplus(message_with_sender_and_time, token, title)
            except json.JSONDecodeError as e:
                print("Error decoding JSON: ", str(e))
        #else:
            #print("未获取到短信, 将在5秒后进行重新读取。")
        time.sleep(5)

def push_pushplus(message, token, title):
    url = "http://www.pushplus.plus/send"
    data = {
        "token": token,
        "title": title,
        "content": message
    }
    try:
        response = requests.post(url, json=data)
        result = response.json()
        print("Response:\n", result)
    except Exception as e:
        print("Error occurred: ", str(e))

if __name__ == '__main__':
    forward()
