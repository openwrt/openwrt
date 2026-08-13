#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <glob.h>
#include <time.h>
#include <stdbool.h>

#define MAX_LENGTH 200
#define MAX_TEMP 110
#define CURVE_POINTS 19 // 20~110每5度共19个点

// 定义全局变量
char thermal_file[MAX_LENGTH] = {0};      // -T
char fan_file[MAX_LENGTH] = {0};   // -F
char fan_rpm_file[MAX_LENGTH] = {0};   // -R
char cpu_thermal_file[MAX_LENGTH] = {0};
char wifi_temp_cmd[MAX_LENGTH] = {0};

int start_speed = 35;   // -s
int start_temp = 45;    // -t
int max_speed = 255;    // -m
int support_max_speed = 255;
int temp_div = 1000;    // -d
int debug_mode = 0;     // -D
int mode = 1;  // 默认均衡模式 0=静音 1=均衡 2=极速
int change = 0;
int enable = 1;

typedef struct {
    int temp;
    int speed;
} CurvePoint;

CurvePoint curve_table[CURVE_POINTS];
int curve_table_size = 0;

// 设备型号配置结构体
typedef struct {
    const char* model;
    const char* thermal_file;
    const char* fan_file;
    const char* fan_rpm_file;
    const char* wifi_temp_cmd;
    int support_max_speed;
} DeviceConfig;

DeviceConfig device_configs[] = {
    {
        "Airpi-AP3000M,Airpi EMMC 16G",
        NULL,
        NULL,
        NULL,
        "iwpriv rax0 stat | grep Temperature | awk '{print $3}'",
        0
    },
    {
        "Hiveton H5000M",
        NULL,
        NULL,
        NULL,
        "iwpriv rai0 stat | grep Temperature | awk '{print $3}'",
        0
    },
    {
        "Raspberry Pi 5 Model B Rev 1.0",
        NULL,
        NULL,
        NULL,
        NULL,
        174
    }
    // 可继续添加其他型号
};
const int device_configs_count = sizeof(device_configs) / sizeof(device_configs[0]);

/**
 * 底层读文件
 */
static int read_file(const char* path ,char* result ,size_t size) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(path ,"r");
    if (fp == NULL)
        return -1;

    if (( read = getline(&line ,&len ,fp) ) != -1) {
        if (size != 0)
            memcpy(result ,line ,size);
        else
            memcpy(result ,line ,read - 1);
    }

    fclose(fp);
    if (line)
        free(line);
    return 0;
}

/**
 * 底层写文件
 */
static size_t write_file(const char* path ,char* buf ,size_t len) {
    FILE* fp = NULL;
    size_t size = 0;
    fp = fopen(path ,"w+");
    if (fp == NULL) {
        return 0;
    }
    size = fwrite(buf ,len ,1 ,fp);
    fclose(fp);
    return size;
}

/**
 * 判断文件是否存在方法
 */
static int file_exist(const char* name) {
    struct stat buffer;
    return stat(name ,&buffer);
}

/**
 * find_file_by_glob
 */
int find_file_by_glob(const char* pattern, char* result, size_t size) {
    glob_t glob_result;
    int ret = -1;

    if (glob(pattern, 0, NULL, &glob_result) == 0) {
        if (glob_result.gl_pathc > 0) {
            strncpy(result, glob_result.gl_pathv[0], size - 1);
            result[size - 1] = '\0';
            ret = 0;
        }
    }
    globfree(&glob_result);
    return ret;
}

/**
 * 获取设备型号
 */
static char* get_device_model() {
    static char model[64] = {0};
    char model_file[32] = "/tmp/sysinfo/model";

    char buf[64] = { 0 };
    // 尝试从 /tmp/sysinfo/model 中获取设备型号
    if (read_file(model_file, buf, 0) == 0) {
        strncpy(model, buf, sizeof(model) - 1);
        model[sizeof(model) - 1] = '\0';
        return model;
    }

    // 如果失败，返回默认值
    strcpy(model, "Unknown");
    return model;
}

// 动态查找thermal_file路径
static void find_and_set_thermal_file(char *dst, size_t size) {
    char found[MAX_LENGTH];
    if (find_file_by_glob("/sys/class/thermal/thermal_zone*/temp", found, sizeof(found)) == 0) {
        strncpy(dst, found, size - 1);
        dst[size - 1] = '\0';
    } else {
        strncpy(dst, "/sys/class/thermal/thermal_zone0/temp", size - 1);
        dst[size - 1] = '\0';
    }
}
// 动态查找fan_file路径
static void find_and_set_fan_file(char *dst, size_t size) {
    char found[MAX_LENGTH];
    // 先查找第一个路径
    if (find_file_by_glob("/sys/class/hwmon/hwmon*/pwm1", found, sizeof(found)) == 0) {
        strncpy(dst, found, size - 1);
        dst[size - 1] = '\0';
    // 再查找第二个路径
    } else if (find_file_by_glob("/sys/devices/platform/pwm-fan/hwmon/hwmon*/pwm1", found, sizeof(found)) == 0) {
        strncpy(dst, found, size - 1);
        dst[size - 1] = '\0';
    // 都没找到用默认
    } else {
        strncpy(dst, "/sys/class/hwmon/hwmon0/pwm1", size - 1);
        dst[size - 1] = '\0';
    }
}
// 动态查找fan_rpm_file路径
static void find_and_set_fan_rpm_file(char *dst, size_t size) {
    char found[MAX_LENGTH];
    if (find_file_by_glob("/sys/class/hwmon/hwmon*/fan1_input", found, sizeof(found)) == 0) {
        strncpy(dst, found, size - 1);
        dst[size - 1] = '\0';
    } else {
        strncpy(dst, "/sys/class/hwmon/hwmon0/fan1_input", size - 1);
        dst[size - 1] = '\0';
    }
}

/**
 * 根据设备型号设置默认路径
 */
void set_default_paths(const char* model) {
    int found = 0;
    for (int i = 0; i < device_configs_count; ++i) {
        // 支持 device_configs[i].model 逗号分隔多型号
        int match = 0;
        char models_copy[128];
        strncpy(models_copy, device_configs[i].model, sizeof(models_copy) - 1);
        models_copy[sizeof(models_copy) - 1] = '\0';
        char *token = strtok(models_copy, ",");
        while (token) {
            if (strcmp(model, token) == 0) {
                match = 1;
                break;
            }
            token = strtok(NULL, ",");
        }
        if (match) {
            // thermal_file
            if (device_configs[i].thermal_file)
                strncpy(thermal_file, device_configs[i].thermal_file, sizeof(thermal_file) - 1);
            else
                find_and_set_thermal_file(thermal_file, sizeof(thermal_file));
            // fan_file
            if (device_configs[i].fan_file) {
                strncpy(fan_file, device_configs[i].fan_file, sizeof(fan_file) - 1);
                fan_file[sizeof(fan_file) - 1] = '\0';
                // 检查文件是否存在
                if (file_exist(fan_file) != 0) {
                    // 不存在则动态查找
                    find_and_set_fan_file(fan_file, sizeof(fan_file));
                }
            } else {
                find_and_set_fan_file(fan_file, sizeof(fan_file));
            }
            // fan_rpm_file
            if (device_configs[i].fan_rpm_file)
                strncpy(fan_rpm_file, device_configs[i].fan_rpm_file, sizeof(fan_rpm_file) - 1);
            else
                find_and_set_fan_rpm_file(fan_rpm_file, sizeof(fan_rpm_file));
            // wifi_temp_cmd
            if (device_configs[i].wifi_temp_cmd)
                strncpy(wifi_temp_cmd, device_configs[i].wifi_temp_cmd, sizeof(wifi_temp_cmd) - 1);
            else
                wifi_temp_cmd[0] = '\0';
            // support_max_speed
            if (device_configs[i].support_max_speed)
                support_max_speed = device_configs[i].support_max_speed;
            else
                support_max_speed = 255;
            found = 1;
            break;
        }
    }
    if (!found) {
        find_and_set_thermal_file(thermal_file, sizeof(thermal_file));
        find_and_set_fan_file(fan_file, sizeof(fan_file));
        find_and_set_fan_rpm_file(fan_rpm_file, sizeof(fan_rpm_file));
        wifi_temp_cmd[0] = '\0';
        support_max_speed = 255;
    }

    // 写回配置文件
    char cmd2[MAX_LENGTH + 64];
    char cmd3[MAX_LENGTH + 64];
    snprintf(cmd2, sizeof(cmd2), "uci set fancontrol.settings.fan_file='%s'", fan_file);
    snprintf(cmd3, sizeof(cmd3), "uci set fancontrol.settings.fan_rpm_file='%s'", fan_rpm_file);
    system(cmd2);
    system(cmd3);
    system("uci commit fancontrol");

    strncpy(cpu_thermal_file, thermal_file, sizeof(cpu_thermal_file) - 1);
}

/**
 * 读取温度
 */
int get_temperature(char* thermal_file ,int div) {
    char buf[8] = { 0 };
    if (read_file(thermal_file ,buf ,0) == 0) {
        return atoi(buf) / div;
    }
    return -1;
}

/**
 * 读取风扇速度
 */
int get_fanspeed(char* fan_file) {
    char buf[8] = { 0 };
    if (read_file(fan_file ,buf ,0) == 0) {
        return atoi(buf);
    }
    return -1;
}

/**
 * 设置风扇转速
 */
int set_fanspeed(int fan_speed ,char* fan_file) {
    char buf[8] = { 0 };
    sprintf(buf ,"%d\n" ,fan_speed);
    return write_file(fan_file ,buf ,strlen(buf));
}

/**
 * 计算风扇转速
 */
int calculate_speed(int current_temp ,int max_temp ,int min_temp ,int max_speed ,int min_speed) {
    if (current_temp < min_temp) {
        return 0;
    }
    double temp_coefficent = (double)support_max_speed / 255.0;
    int fan_speed = ( current_temp - min_temp ) * ( max_speed - min_speed ) / ( max_temp - min_temp ) + min_speed;
    if (fan_speed > max_speed) {
        fan_speed = max_speed;
    }
    fan_speed = (int)(fan_speed * temp_coefficent);
    return fan_speed;
}

/**
 *  信号处理函数
 */
void loop_set_fanspeed(int fan_speed, char* fan_file, int seconds);

void handle_termination(int signum) {
    // 设置风扇转速为 0
    loop_set_fanspeed(0 ,fan_file ,15);
    exit(EXIT_SUCCESS); // 优雅地退出程序
}

/**
 * 注册信号处理函数
 */
void register_signal_handlers( ) {
    struct sigaction sa;
    memset(&sa ,0 ,sizeof(sa));
    sa.sa_handler = handle_termination;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT ,&sa ,NULL);
    sigaction(SIGTERM ,&sa ,NULL);
}

// 根据模式调整参数
void adjust_params_by_mode() {
    if (mode == 0) { // 静音模式
        start_temp = 55;
        start_speed = 40;
        max_speed = 128;
    } else if (mode == 2) { // 极速模式
        start_temp = 30;
        start_speed = 200;
        max_speed = 255;
    } else { // 均衡模式
        start_temp = 45;
        start_speed = 70;
        max_speed = 255;
    }
}

int get_wifi_temperature() {
    if (wifi_temp_cmd[0] == '\0') return -100;
    FILE *fp;
    char buf[64] = {0};
    int temp = -100;
    fp = popen(wifi_temp_cmd, "r");
    if (fp) {
        if (fgets(buf, sizeof(buf), fp)) {
            temp = atoi(buf);
        }
        pclose(fp);
    }
    return temp;
}

// 解析curve字符串
void parse_curve_config(const char* curve_str) {
    curve_table_size = 0;
    if (!curve_str) return;
    char buf[MAX_LENGTH * 2];
    strncpy(buf, curve_str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    char* token = strtok(buf, ",");
    while (token && curve_table_size < CURVE_POINTS) {
        int t = 0, s = 0;
        if (sscanf(token, "%d:%d", &t, &s) == 2) {
            curve_table[curve_table_size].temp = t;
            curve_table[curve_table_size].speed = s;
            curve_table_size++;
        }
        token = strtok(NULL, ",");
    }
}

// 根据温度查表获取转速百分比（线性插值）
int get_speed_from_curve(int temp) {
    if (curve_table_size < 2) return -1;
    if (temp <= curve_table[0].temp) return curve_table[0].speed;
    if (temp >= curve_table[curve_table_size-1].temp) return curve_table[curve_table_size-1].speed;
    for (int i = 1; i < curve_table_size; ++i) {
        if (temp < curve_table[i].temp) {
            int t0 = curve_table[i-1].temp, t1 = curve_table[i].temp;
            int s0 = curve_table[i-1].speed, s1 = curve_table[i].speed;
            return s0 + (s1 - s0) * (temp - t0) / (t1 - t0);
        }
    }
    return curve_table[curve_table_size-1].speed;
}

// 读取curve配置
void load_curve_from_uci() {
    FILE* fp = popen("uci get fancontrol.settings.curve 2>/dev/null", "r");
    if (!fp) return;
    char buf[MAX_LENGTH * 2] = {0};
    if (fgets(buf, sizeof(buf), fp)) {
        char* nl = strchr(buf, '\n');
        if (nl) *nl = '\0';
        parse_curve_config(buf);
    }
    pclose(fp);
}

// 检测配置变更标志文件
void check_change_flag() {
    if (file_exist("/tmp/fancontrol.changed") == 0) {
        change = 1;
    }
}

// 清除配置变更标志文件
void clear_change_flag() {
    remove("/tmp/fancontrol.changed");
    change = 0;
}

// 读取模式
void load_mode_from_uci() {
    FILE* fp = popen("uci get fancontrol.settings.mode 2>/dev/null", "r");
    if (!fp) return;
    char buf[MAX_LENGTH * 2] = {0};
    if (fgets(buf, sizeof(buf), fp)) {
        char* nl = strchr(buf, '\n');
        if (nl) *nl = '\0';
        mode = atoi(buf);
    }
    pclose(fp);
}

// 读取enable
void check_enable_flag() {
    if (file_exist("/tmp/fancontrol.close") == 0) {
        remove("/tmp/fancontrol.close");
        enable = 0;
    }
}

/**
 * 循环设置风扇转速30秒
 */
 void loop_set_fanspeed(int fan_speed ,char* fan_file ,int seconds) {
    time_t end = time(NULL) + seconds;
    while (time(NULL) < end) {
        // 检查标志文件是否存在
        check_enable_flag();
        check_change_flag();
        if (enable == 1 && change == 1)
            return;
        set_fanspeed(fan_speed ,fan_file);
        usleep(100000); // 200ms
    }
}

void wait_sleep(int seconds)
{
    time_t end = time(NULL) + seconds;
    while (time(NULL) < end) {
        check_change_flag();
        if (change == 1)
            return;
        usleep(100000); // 100ms
    }
}

/**
 * 主函数
 */
int main(int argc ,char* argv[ ]) {
    // 获取设备型号
    char* model = get_device_model();

    // 设置默认路径
    set_default_paths(model);

    // 只解析-M参数
    int opt;
    while (( opt = getopt(argc ,argv ,"M:") ) != -1) {
        switch (opt) {
            case 'M':
                mode = atoi(optarg);
                adjust_params_by_mode();
                break;
            default:
                fprintf(stderr ,"Usage: %s [-M mode] (0=静音 1=均衡 2=极速)\n" ,argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    // 检测虚拟文件是否存在
    if (file_exist(fan_file) != 0 || file_exist(thermal_file) != 0) {
        fprintf(stderr ,"File: '%s' or '%s' not exist\n" ,fan_file ,thermal_file);
        exit(EXIT_FAILURE);
    }

    load_curve_from_uci();

    // 注册退出信号
    register_signal_handlers( );

    // 监控风扇
    static int last_is_wifi = -1; // -1:未初始化, 0:CPU, 1:WiFi
    // bool is_first = true;
    while (1) {
        int cpu_temp = get_temperature(cpu_thermal_file ,temp_div);
        int wifi_temp = -100;
        if (wifi_temp_cmd[0] != '\0') {
            wifi_temp = get_wifi_temperature();
            // 每次都写入wifi温度文件（单位：毫度）
            FILE *wfp = fopen("/tmp/wifi_temperature", "w");
            if (wfp) {
                fprintf(wfp, "%d\n", wifi_temp * 1000);
                fclose(wfp);
            }
        }
        // 取最大温度
        int max_temp_val = (cpu_temp > wifi_temp) ? cpu_temp : wifi_temp;
        int is_wifi = (max_temp_val == wifi_temp);
        // 只有切换参考温度时才写uci配置
        if (is_wifi != last_is_wifi) {
            char cmd[MAX_LENGTH + 64];
            if (is_wifi) {
                snprintf(cmd, sizeof(cmd), "uci set fancontrol.settings.thermal_file='/tmp/wifi_temperature'");
            } else {
                snprintf(cmd, sizeof(cmd), "uci set fancontrol.settings.thermal_file='%s'", cpu_thermal_file);
            }
            system(cmd);
            system("uci commit fancontrol");
            last_is_wifi = is_wifi;
        }
        // 有效温度时设置风扇速度
        if (max_temp_val > 0) {
            int fan_speed = -1;
            if (curve_table_size >= 2) {
                // 使用自定义曲线
                fan_speed = get_speed_from_curve(max_temp_val);
                fan_speed = fan_speed * support_max_speed / 100;
            } else {
                // 兼容老逻辑
                fan_speed = calculate_speed(max_temp_val, MAX_TEMP, start_temp, max_speed, start_speed);
            }
            if (fan_speed != 0 && fan_speed < 20)
                fan_speed = 20;
            loop_set_fanspeed(fan_speed, fan_file, 10);
        }
        if (debug_mode) {
            fprintf(stdout ,"CPU: %d°C, WiFi: %d°C, Use: %d°C, Fan Speed: %d, Mode: %d\n" ,cpu_temp, wifi_temp, max_temp_val, get_fanspeed(fan_file) ,mode);
        }
        if (change == 0) {
            check_change_flag();
            if (change == 0)
                wait_sleep(5);
        }

        if (change == 1)
        {
            load_mode_from_uci();
            adjust_params_by_mode();
            // 读取curve配置
            load_curve_from_uci();
            // 处理完后清除标志
            clear_change_flag();
        }
    }
    return 0;
}
