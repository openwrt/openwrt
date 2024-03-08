#!/usr/bin/env python3

from pathlib import Path
from sys import argv
import json

if len(argv) != 3:
    print("Usage: checksec_mitigation_report.py build_dir output")
    exit(1)

builddir_path = argv[1]
output_path = Path(argv[2])

print(builddir_path)
print(output_path)


output = {
}

with open(output_path, "w") as write_file:
    output["packagelist"]=[]

    builddir = Path(builddir_path)
    for report in list(builddir.rglob("checksec_report_formatted.json")):
        with open(report, "r") as read_file:
            data = json.load(read_file)
            output["packagelist"].append(data)
    json.dump(output, write_file)

print("#############################################")
print("############# HARDENING RESULTS #############")
print("#############################################")

# elf list
number_of_elf=0
for package in output["packagelist"]:
    number_of_elf += len(package["elflist"])

print("NB_ELF = " + str(number_of_elf))

#relro result
number_of_relro=0
relro_positive_result=["n/a", "full", "partial"]
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["relro"] in relro_positive_result:
            number_of_relro+=1

# now result
number_of_now=0
now_positive_result=["n/a", "full"]
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["relro"] in now_positive_result :
            number_of_now+=1;

# stack protector result
number_of_ssp=0
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["canary"] == "yes":
            number_of_ssp+=1;

# nx result
number_of_nx=0
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["nx"] == "yes":
            number_of_nx+=1;

# PIE result 
number_of_pie=0
pie_positive_result=["yes", "dso"]
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["pie"] in pie_positive_result:
            number_of_pie+=1;

# rpath
number_of_rpath=0
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["rpath"] == "yes":
            number_of_rpath+=1;

# fortify
number_of_fortify=0
for package in output["packagelist"]:
    for k in list(package["elflist"]):
        elf=package["elflist"][k]
        if elf["fortify_source"] != "no" :
            number_of_fortify+=1;


print('RELRO =  {:2.2%}'.format(number_of_relro / number_of_elf))
print('NOW =  {:2.2%}'.format(number_of_now / number_of_elf))
print('NX =  {:2.2%}'.format(number_of_nx / number_of_elf))
print('PIE =  {:2.2%}'.format(number_of_pie / number_of_elf))
print('SSP =  {:2.2%}'.format(number_of_ssp / number_of_elf))
print('FORTIFY =  {:2.2%}'.format(number_of_fortify / number_of_elf))
print('RPATH =  {:2.2%}'.format(number_of_rpath / number_of_elf))

