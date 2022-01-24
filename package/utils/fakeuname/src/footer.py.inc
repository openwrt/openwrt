
args_dict = {
    "-a" : "%s %s %s %s %s %s %s %s" % 
            (kernel_name, nodename, kernel_release, kernel_version,
            machine, processor, hardware_platform, operating_system),
    "-m" : machine,
    "-n" : nodename,
    "-r" : kernel_release,
    "-s" : kernel_name,
    "-p" : processor,
    "-v" : kernel_version,
    "-i" : hardware_platform,
    "-o" : operating_system,
}

input_args = sys.argv
if len(input_args) == 1:
    input_args = ["-s"]

print(" ".join([args_dict[arg] for arg in input_args if arg in args_dict]))
