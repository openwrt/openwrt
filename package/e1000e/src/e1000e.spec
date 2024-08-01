Name: e1000e
Summary: Intel(R) Gigabit Ethernet Connection
Version: 3.8.4
Release: 1
Source: %{name}-%{version}.tar.gz
Vendor: Intel Corporation
License: GPL
ExclusiveOS: linux
Group: System Environment/Kernel
Provides: %{name}
URL: http://support.intel.com/support/go/linux/e1000e.htm
BuildRoot: %{_tmppath}/%{name}-%{version}-root
# do not generate debugging packages by default - newer versions of rpmbuild
# may instead need:
#%define debug_package %{nil}
%debug_package %{nil}
# macros for finding system files to update at install time (pci.ids, pcitable)
%define find() %(for f in %*; do if [ -e $f ]; then echo $f; break; fi; done)
%define _pciids   /usr/share/pci.ids        /usr/share/hwdata/pci.ids
%define _pcitable /usr/share/kudzu/pcitable /usr/share/hwdata/pcitable /dev/null
%define pciids    %find %{_pciids}
%define pcitable  %find %{_pcitable}
Requires: kernel, fileutils, findutils, gawk, bash

%description
This package contains the Linux driver for the Intel(R) Gigabit Family of Server Adapters.

%prep
%setup

%build
make -C src clean
make -C src

%install
make -C src INSTALL_MOD_PATH=%{buildroot} MANDIR=%{_mandir} rpm
# Append .new to driver name to avoid conflict with kernel RPM
cd %{buildroot}
find lib -name "e1000e.*o" -exec mv {} {}.new \; \
         -fprintf %{_builddir}/%{name}-%{version}/file.list "/%p.new\n"


%clean
rm -rf %{buildroot}

%files -f %{_builddir}/%{name}-%{version}/file.list
%defattr(-,root,root)
%{_mandir}/man7/e1000e.7.gz
%doc COPYING
%doc README
%doc file.list
%doc pci.updates

%post
FL="%{_docdir}/%{name}-%{version}/file.list
    %{_docdir}/%{name}/file.list"
FL=$(for d in $FL ; do if [ -e $d ]; then echo $d; break; fi;  done)

if [ -d /usr/local/lib/%{name} ]; then
	rm -rf /usr/local/lib/%{name}
fi
if [ -d /usr/local/share/%{name} ]; then
	rm -rf /usr/local/share/%{name}
fi

# Save old drivers (aka .o and .o.gz)
echo "original pci.ids saved in /usr/local/share/%{name}";
if [ "%{pcitable}" != "/dev/null" ]; then
	echo "original pcitable saved in /usr/local/share/%{name}";
fi
for k in $(sed 's/\/lib\/modules\/\([0-9a-zA-Z_\.\-]*\).*/\1/' $FL) ; 
do
	d_drivers=/lib/modules/$k
	d_usr=/usr/local/share/%{name}/$k
	mkdir -p $d_usr
	cd $d_drivers; find . -name %{name}.*o -exec cp --parents {} $d_usr \; -exec rm -f {} \;
	cd $d_drivers; find . -name %{name}_*.*o -exec cp --parents {} $d_usr \; -exec rm -f {} \;
	cd $d_drivers; find . -name %{name}.*o.gz -exec cp --parents {} $d_usr \; -exec rm -f {} \;
	cd $d_drivers; find . -name %{name}_*.*o.gz -exec cp --parents {} $d_usr \; -exec rm -f {} \;
	cp --parents %{pciids} /usr/local/share/%{name}/
	if [ "%{pcitable}" != "/dev/null" ]; then
		cp --parents %{pcitable} /usr/local/share/%{name}/
	fi
done

# Add driver link
for f in $(sed 's/\.new$//' $FL) ; do
	ln -f $f.new $f 
done

# Check if kernel version rpm was built on IS the same as running kernel
BK_LIST=$(sed 's/\/lib\/modules\/\([0-9a-zA-Z_\.\-]*\).*/\1/' $FL)
MATCH=no
for i in $BK_LIST
do
	if [ $(uname -r) == $i ] ; then
		MATCH=yes
		break
	fi
done
if [ $MATCH == no ] ; then
	echo -n "WARNING: Running kernel is $(uname -r).  "
	echo -n "RPM supports kernels (  "
	for i in $BK_LIST
	do
		echo -n "$i  "
	done
	echo ")"
fi

LD="%{_docdir}/%{name}";
if [ -d %{_docdir}/%{name}-%{version} ]; then
	LD="%{_docdir}/%{name}-%{version}";
fi

#Yes, this really needs bash
bash -s %{pciids} \
	%{pcitable} \
	$LD/pci.updates \
	$LD/pci.ids.new \
	$LD/pcitable.new \
	%{name} \
<<"END"
#! /bin/bash
# $1 = system pci.ids file to update
# $2 = system pcitable file to update
# $3 = file with new entries in pci.ids file format
# $4 = pci.ids output file
# $5 = pcitable output file
# $6 = driver name for use in pcitable file

exec 3<$1
exec 4<$2
exec 5<$3
exec 6>$4
exec 7>$5
driver=$6
IFS=

# pattern matching strings
ID="[[:xdigit:]][[:xdigit:]][[:xdigit:]][[:xdigit:]]"
VEN="${ID}*"
DEV="	${ID}*"
SUB="		${ID}*"
TABLE_DEV="0x${ID}	0x${ID}	\"*"
TABLE_SUB="0x${ID}	0x${ID}	0x${ID}	0x${ID}	\"*"

line=
table_line=
ids_in=
table_in=
vendor=
device=
ids_device=
table_device=
subven=
ids_subven=
table_subven=
subdev=
ids_subdev=
table_subdev=
ven_str=
dev_str=
sub_str=

# force a sub-shell to fork with a new stdin
# this is needed if the shell is reading these instructions from stdin
while true
do
	# get the first line of each data file to jump start things
	exec 0<&3
	read -r ids_in
	if [ "$2" != "/dev/null" ];then
	exec 0<&4
	read -r table_in
	fi

	# outer loop reads lines from the updates file
	exec 0<&5
	while read -r line
	do
		# vendor entry
		if [[ $line == $VEN ]]
		then
			vendor=0x${line:0:4}
			ven_str=${line#${line:0:6}}
			# add entry to pci.ids
			exec 0<&3
			exec 1>&6
			while [[ $ids_in != $VEN ||
				 0x${ids_in:0:4} < $vendor ]]
			do
				echo "$ids_in"
				read -r ids_in
			done
			echo "$line"
			if [[ 0x${ids_in:0:4} == $vendor ]]
			then
				read -r ids_in
			fi

		# device entry
		elif [[ $line == $DEV ]]
		then
			device=`echo ${line:1:4} | tr "[:upper:]" "[:lower:]"`
			table_device=0x${line:1:4}
			dev_str=${line#${line:0:7}}
			ids_device=`echo ${ids_in:1:4} | tr "[:upper:]" "[:lower:]"`
			table_line="$vendor	$table_device	\"$driver\"	\"$ven_str|$dev_str\""
			# add entry to pci.ids
			exec 0<&3
			exec 1>&6
			while [[ $ids_in != $DEV ||
				 $ids_device < $device ]]
			do
				if [[ $ids_in == $VEN ]]
				then
					break
				fi
				if [[ $ids_device != ${ids_in:1:4} ]]
				then
					echo "${ids_in:0:1}$ids_device${ids_in#${ids_in:0:5}}"
				else
					echo "$ids_in"
				fi
				read -r ids_in
				ids_device=`echo ${ids_in:1:4} | tr "[:upper:]" "[:lower:]"`
			done
			if [[ $device != ${line:1:4} ]]
			then
				echo "${line:0:1}$device${line#${line:0:5}}"
			else
				echo "$line"
			fi
			if [[ $ids_device == $device ]]
			then
				read -r ids_in
			fi
			# add entry to pcitable
			if [ "$2" != "/dev/null" ];then
			exec 0<&4
			exec 1>&7
			while [[ $table_in != $TABLE_DEV ||
				 ${table_in:0:6} < $vendor ||
				 ( ${table_in:0:6} == $vendor &&
				   ${table_in:7:6} < $table_device ) ]]
			do
				echo "$table_in"
				read -r table_in
			done
			echo "$table_line"
			if [[ ${table_in:0:6} == $vendor &&
			      ${table_in:7:6} == $table_device ]]
			then
				read -r table_in
			fi
			fi
		# subsystem entry
		elif [[ $line == $SUB ]]
		then
			subven=`echo ${line:2:4} | tr "[:upper:]" "[:lower:]"`
			subdev=`echo ${line:7:4} | tr "[:upper:]" "[:lower:]"`
			table_subven=0x${line:2:4}
			table_subdev=0x${line:7:4}
			sub_str=${line#${line:0:13}}
			ids_subven=`echo ${ids_in:2:4} | tr "[:upper:]" "[:lower:]"`
			ids_subdev=`echo ${ids_in:7:4} | tr "[:upper:]" "[:lower:]"`
			table_line="$vendor	$table_device	$table_subven	$table_subdev	\"$driver\"	\"$ven_str|$sub_str\""
			# add entry to pci.ids
			exec 0<&3
			exec 1>&6
			while [[ $ids_in != $SUB ||
				 $ids_subven < $subven ||
				 ( $ids_subven == $subven && 
				   $ids_subdev < $subdev ) ]]
			do
				if [[ $ids_in == $VEN ||
				      $ids_in == $DEV ]]
				then
					break
				fi
				if [[ ! (${ids_in:2:4} == "1014" &&
					 ${ids_in:7:4} == "052C") ]]
				then
					if [[ $ids_subven != ${ids_in:2:4} || $ids_subdev != ${ids_in:7:4} ]]
					then
						echo "${ids_in:0:2}$ids_subven $ids_subdev${ids_in#${ids_in:0:11}}"
					else
						echo "$ids_in"
					fi
				fi
				read -r ids_in
				ids_subven=`echo ${ids_in:2:4} | tr "[:upper:]" "[:lower:]"`
				ids_subdev=`echo ${ids_in:7:4} | tr "[:upper:]" "[:lower:]"`
			done
			if [[ $subven != ${line:2:4} || $subdev != ${line:7:4} ]]
			then
				echo "${line:0:2}$subven $subdev${line#${line:0:11}}"
			else
				echo "$line"
			fi
			if [[ $ids_subven == $subven  &&
			      $ids_subdev == $subdev ]]
			then
				read -r ids_in
			fi
			# add entry to pcitable
			if [ "$2" != "/dev/null" ];then
			exec 0<&4
			exec 1>&7
			while [[ $table_in != $TABLE_SUB ||
				 ${table_in:14:6} < $table_subven ||
				 ( ${table_in:14:6} == $table_subven &&
				   ${table_in:21:6} < $table_subdev ) ]]
			do
				if [[ $table_in == $TABLE_DEV ]]
				then
					break
				fi
				if [[ ! (${table_in:14:6} == "0x1014" &&
					 ${table_in:21:6} == "0x052C") ]]
				then
					echo "$table_in"
				fi
				read -r table_in
			done
			echo "$table_line"
			if [[ ${table_in:14:6} == $table_subven &&
			      ${table_in:21:6} == $table_subdev ]]
			then
				read -r table_in
			fi
			fi
		fi

		exec 0<&5
	done

	# print the remainder of the original files
	exec 0<&3
	exec 1>&6
	echo "$ids_in"
	while read -r ids_in
	do
		echo "$ids_in"
	done

	if [ "$2" != "/dev/null" ];then
	exec 0>&4
	exec 1>&7
	echo "$table_in"
	while read -r table_in
	do
		echo "$table_in"
	done
	fi

	break
done <&5

exec 3<&-
exec 4<&-
exec 5<&-
exec 6>&-
exec 7>&-

END

mv -f $LD/pci.ids.new  %{pciids}
if [ "%{pcitable}" != "/dev/null" ]; then
mv -f $LD/pcitable.new %{pcitable}
fi

uname -r | grep BOOT || /sbin/depmod -a > /dev/null 2>&1 || true

%preun
# If doing RPM un-install
if [ $1 -eq 0 ] ; then
	FL="%{_docdir}/%{name}-%{version}/file.list
    		%{_docdir}/%{name}/file.list"
	FL=$(for d in $FL ; do if [ -e $d ]; then echo $d; break; fi;  done)

	# Remove driver link
	for f in $(sed 's/\.new$//' $FL) ; do
		rm -f $f
	done

	# Restore old drivers
	if [ -d /usr/local/share/%{name} ]; then
		cd /usr/local/share/%{name}; find . -name '%{name}.*o*' -exec cp --parents {} /lib/modules/ \;
		cd /usr/local/share/%{name}; find . -name '%{name}_*.*o*' -exec cp --parents {} /lib/modules/ \;
		rm -rf /usr/local/share/%{name}
	fi
fi

%postun
uname -r | grep BOOT || /sbin/depmod -a > /dev/null 2>&1 || true

