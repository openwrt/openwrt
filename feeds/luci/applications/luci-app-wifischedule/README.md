# wifischedule
Turns WiFi on and off according to a schedule on an openwrt router

## Components
* wifischedule: Shell script that creates cron jobs based on configuration provided in UCI and does all the other logic of enabling and disabling wifi with the use of `/sbin/wifi` and `/usr/bin/iwinfo`. Can be used standalone.
* luci-app-wifischedule: LUCI frontend for creating the UCI configuration and triggering the actions. Depends on wifischedule.


## Use cases
You can create user-defined events when to enable or disable WiFi.
There are various use cases why you would like to do so:

1. Reduce power consumption and therefore reduce CO2 emissions.
2. Reduce emitted electromagnatic radiation.
3. Force busincess hours when WiFi is available.

Regarding 1: Please note, that you need to unload the wireless driver modules in order to get the most effect of saving power.
In my test scenario only disabling WiFi saves about ~0.4 Watt, unloading the modules removes another ~0.4 Watt.

Regarding 2: Think of a wireless accesspoint e.g. in your bedrom, kids room where you want to remove the amount of radiation emitted.

Regarding 3: E.g. in a company, why would wireless need to be enabled weekends if no one is there working?
Or think of an accesspoint in your kids room when you want the youngsters to sleep after 10 pm instead of facebooking...

## Configuration
You can create an arbitrary number of schedule events. Please note that there is on sanity check done whether the start / stop times overlap or make sense.
If start and stop time are equal, this leads to disabling the WiFi at the given time.

Logging if enabled is done to the file `/var/log/wifi_schedule.log` and can be reviewed through the "View Logfile" tab.
The cron jobs created can be reviewed through the "View Cron Jobs" tab.

Please note that the "Unload Modules" function is currently considered as experimental. You can manually add / remove modules in the text field.
The button "Determine Modules Automatically" tries to make a best guess determining regarding the driver module and its dependencies.
When un-/loading the modules, there is a certain number of retries (`module_load`) performed.

The option "Force disabling wifi even if stations associated" does what it says - when activated it simply shuts down WiFi.
When unchecked, its checked every `recheck_interval` minutes if there are still stations associated. Once the stations disconnect, WiFi is disabled.

Please note, that the parameters `module_load` and `recheck_interval` are only accessible through uci.

## UCI Configuration `wifi_schedule`
UCI configuration file: `/etc/config/wifi_schedule`:

```
config global
        option logging '0'
        option enabled '0'
        option recheck_interval '10'
        option modules_retries '10'

config entry 'Businesshours'
        option enabled '0'
        option daysofweek 'Monday Tuesday Wednesday Thursday Friday'
        option starttime '06:00'
        option stoptime '22:00'
        option forcewifidown '0'

config entry 'Weekend'
        option enabled '0'
        option daysofweek 'Saturday Sunday'
        option starttime '00:00'
        option stoptime '00:00'
        option forcewifidown '1'
```

## Script: `wifi_schedule.sh`
This is the script that does the work. Make your changes to the UCI config file: `/etc/config/wifi_schedule`

Then call the script as follows in order to get the necessary cron jobs created:

`wifi_schedule.sh cron`

All commands:

```
wifi_schedule.sh cron|start|stop|forcestop|recheck|getmodules|savemodules|help

    cron: Create cronjob entries.
    start: Start wifi.
    stop: Stop wifi gracefully, i.e. check if there are stations associated and if so keep retrying.
    forcestop: Stop wifi immediately.
    recheck: Recheck if wifi can be disabled now.
    getmodules: Returns a list of modules used by the wireless driver(s)
    savemodules: Saves a list of automatic determined modules to UCI
    help: This description.
```
