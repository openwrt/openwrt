#!/bin/sh
# 显示用户选择菜单
echo ""
echo "_____Smstrun+1.2 by:manper_____"
echo ""
echo "请选择短信转发程序的操作："
echo "_______________________________"
echo ""
echo "<<1. 立即测试短信转发程序"
echo "<<2. 设置token参数"
echo "<<3. 配置所有步骤并设置为开机启动"
echo "<<4. 查看短信转发程序状态"
echo "<<5. 设置此CPE短信转发标题"
echo "<<6. 关闭短信推送功能"
echo "_______________________________"
echo ""
read -p "请输入数字并回车进行选择：" choice
# 路径变量
CONF_PATH="/usr/bin/smstrun.conf"
PY_SCRIPT="/usr/bin/smstrun.py"
STARTUP_SCRIPT="/etc/rc.local"
TITLE_PATH="/usr/bin/smstrun-title.conf"

case $choice in
    1)
        # 检测配置文件是否存在
        if [ -f $CONF_PATH ]; then
            # 存在则运行Python脚本
            python $PY_SCRIPT &
        else
            # 不存在则提示用户输入token
            while true; do
                read -p "请输入PPS平台token并回车: " token
                read -p "确认你的PPS平台token是 $token 吗？(y/n): " confirm
                if [ "$confirm" = "y" ]; then
                    echo $token > $CONF_PATH
                    python3 $PY_SCRIPT &
                    break
                fi
            done
        fi
        ;;
    2)
        # 重新填写token
        while true; do
            read -p "请输入PPS平台token并回车: " token
            read -p "确认你的PPS平台token是 $token 吗？(y/n): " confirm
            if [ "$confirm" = "y" ]; then
                echo $token > $CONF_PATH
                break
            fi
        done
        ;;
    3)
        # 完成配置并设置为开机启动
        while true; do
            read -p "请输入PPS平台token并回车: " token1
            if [ -z "$token1" ]; then
                echo "未输入有效字符，请重新输入。"
                continue
            fi
            read -p "请再次输入PPS平台token并回车: " token2
            if [ "$token1" != "$token2" ]; then
                echo "两次输入的token不一致，请重新输入。"
                continue
            else
                echo "未设置短信转发标题,请查收新短信:" > $TITLE_PATH
                echo $token1 > $CONF_PATH
                # 设置脚本开机启动
                if grep -Fxq "$PY_SCRIPT" $STARTUP_SCRIPT
                then
                    echo "启动项已存在。"
                else
                    echo "(sleep 40; python3 $PY_SCRIPT)&" >> $STARTUP_SCRIPT
                fi
                
                read -p "如要完成开机启动项配置需要立刻重启，是否立刻重启？输入Y立刻重启，输入N稍后手动重启。" choice
                if [ "$choice" = "Y" ] || [ "$choice" = "y" ]; then
                    echo "正在重启系统..."
                    reboot
                else
                    echo "你已取消重启，请稍后自行手动完成重启动作。"
                    break
                fi
            fi
        done
        ;;
    4)
        # 查看短信转发状态
        if pgrep -f $PY_SCRIPT > /dev/null
        then
            echo "yes!当前短信转发程序已在运行中，如有问题请尝试重启CPE，或检查你的网络或参数。"
            # 输出PPS平台Token和短信转发标题
            if [ -f $CONF_PATH ]; then
                echo "你的PPS平台Token为: $(cat $CONF_PATH)"
            else
                echo "未设置PPS平台Token"
            fi
            
            TITLE_PATH="/usr/bin/smstrun-title.conf"
            if [ -f $TITLE_PATH ]; then
                echo "你的短信转发标题为: $(cat $TITLE_PATH)"
            else
                echo "未设置短信转发标题"
            fi
            echo "如果需要修改，请运行脚本选择2或5进行修改。"
        else
            echo "Err!当前短信转发程序未在运行，请重新执行本脚本选择1进行手动开启，或选择3尝试重新进行配置并设置为开机启动项。"
        fi
        ;;
    
    5)
        # 设置短信转发标题
        read -p "请输入短信转发标题并回车: " title
        if [ -z "$title" ]; then
            echo "你好像没有输入东西啊？"
            while true; do
                read -p "请重新输入短信转发标题: " title
                if [ -z "$title" ]; then
                    echo "你好像没有输入东西啊？"
                else
                    break
                fi
            done
        fi
        
        echo "确认你的短信转发标题是 $title 吗？(y/n)"
        read confirm
        if [ "$confirm" = "y" ]; then
            echo $title > $TITLE_PATH
            echo "短信转发标题已设置为: $title"
        else
            echo "" > $TITLE_PATH
            echo "短信转发标题已被清空"
        fi
        ;;


    6)
        # 关闭短信推送功能
        read -p "确认是否关闭短信转发程序？(y/n): " confirm
        if [ "$confirm" = "y" ]; then
            # 删除配置文件
            rm -f $CONF_PATH
            echo "你已经关闭短信转发功能，重启生效，如需再次开启请运行脚本选择3进行配置即可。"
            read -p "是否立即重启以生效？(Y/N): " choice
            if [ "$choice" = "Y" ] || [ "$choice" = "y" ]; then
                echo "正在重启系统..."
                reboot
            else
                echo "你已取消自动重启，短信转发功能已关闭，请手动重启生效。"
            fi
        else
            echo "取消操作。"
        fi
        ;;

    *)
        echo "你是在乱输吗？"
        ;;        

esac
