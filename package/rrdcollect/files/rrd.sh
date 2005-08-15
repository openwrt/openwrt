#!/bin/sh
#
# Author:   Christian Rost (chr@baltic-online.de)
# Purpose:  Shell-Interface to rrdtool
#
RRDCNF=/etc/rrd.conf
. $RRDCNF

######################################
# init() function to create .rrd-file
# $1=VAR-PRAEFIX $2=VAR-POSTFIX 
######################################
init()
{
	# Calculate optimal step by assuming highest precision on the last-xhour plot
	OPTSTEP=`expr $HOURS \* 3600 \/ $PIXWIDTH \* $H_RES`
	[ $OPTSTEP != $STEP ] && echo "WARNING: Your optimal step is: $OPTSTEP. But you are using: $STEP"
	
	ARH_AVG=`expr $HOURS \* 3600   \/ $PIXWIDTH \* $H_RES \/ $STEP`; ARH_CNT=`expr $HOURS \* 3600   \/ $ARH_AVG \/ $STEP`
	ARD_AVG=`expr $DAYS  \* 86400  \/ $PIXWIDTH \* $D_RES \/ $STEP`; ARD_CNT=`expr $DAYS  \* 86400  \/ $ARD_AVG \/ $STEP`
	ARW_AVG=`expr $WEEKS \* 604800 \/ $PIXWIDTH \* $W_RES \/ $STEP`; ARW_CNT=`expr $WEEKS \* 604800 \/ $ARW_AVG \/ $STEP`
	
	PRAEFIX=$1; POSTFIX=$2
	DSOURCE=""
	
	echo "Init storage for $HOURS hours on averages: $ARH_AVG x $STEP seconds ($ARH_CNT total values)"
	echo "Init storage for $DAYS days on averages: $ARD_AVG x $STEP seconds ($ARD_CNT total values)"
	echo "Init storage for $WEEKS weeks on averages: $ARW_AVG x $STEP seconds ($ARW_CNT total values)"

	eval  SOURCE=\$${PRAEFIX}_${POSTFIX}
	for i in $SOURCE
	do
		eval var=\$${PRAEFIX}_${i}
		ifs="$IFS"; IFS="|"; set -- $var
		if  [ "`echo ${1} | cut -b1`" = "-" ]
		then
			VAR="`echo ${1} | cut -b2-`"
		else
			VAR=`echo $1 | sed "s/ //g"`
		fi
                DSOURCE="$DSOURCE DS:${VAR}:${3}:$MAXSTEP:0:${4}"
		IFS="$ifs"
        done
        $DEBUG $RRDTOOL create $RRDDIR/$PRAEFIX.rrd       \
                --step ${STEP}                            \
		$DSOURCE                                  \
                        RRA:AVERAGE:0.5:$ARH_AVG:$ARH_CNT \
                        RRA:AVERAGE:0.5:$ARD_AVG:$ARD_CNT \
                        RRA:AVERAGE:0.5:$ARW_AVG:$ARW_CNT \
                        RRA:MAX:0.5:$ARH_AVG:$ARH_CNT     \
                        RRA:MAX:0.5:$ARD_AVG:$ARD_CNT     \
                        RRA:MAX:0.5:$ARW_AVG:$ARW_CNT
	return $?
}

#################################################################################################################
# graph() function to create png-graphics
# $1=IMG-DIR $2=VAR-PRAEFIX, $3=VAR-POSTFIX, $4=Headline, $5=end, $6=start, $7=base, 
# $8=DEFS/ENTRIES $9=other rrdgraph options (Vertlabel,...)
#################################################################################################################
# COMMENT:"$DATE\c"
do_rrdgraph()
{	
        $DEBUG $NICE $RRDTOOL graph "$1/$2_$3.png" \
          --title          "$4"             \
          --imgformat      $PIXFORMAT       \
          --width          $PIXWIDTH        \
          --height         $PIXHEIGHT       \
          --end            "$5"             \
          --start          "$6"             \
	  --base "$7"   	            \
	  $8 $9
}

#################################################################################################################
# graph() function to create png-graphics
# $1=IMG-DIR $2=VAR-PRAEFIX, $3=VAR-POSTFIX, $4=Headline, $5=end, $6=start, $7=base, 
# $8=DEFS/ENTRIES $9=other rrdgraph options (Vertlabel,...)
#################################################################################################################
# COMMENT:"<RRD::TIME::NOW %c>\c"
do_rrdcgi()
{
	cat << EOF > $1/$2_$3.def
              
<RRD::GRAPH $1/<RRD::CV START>$2_$3.png \
--title          "$4"              \
--imgformat      $PIXFORMAT        \
--width          $PIXWIDTH         \
--height         $PIXHEIGHT        \
--end            "$5"              \
--start          <RRD::CV START>   \
--base 	         $7   	           \
--imginfo        "<IMG NAME=$2_$3 SRC=$RRDCGIIMGPATH/%s OnClick=zoomToggle('${SCGIPIXWIDTH}px','${NCGIPIXWIDTH}px',this); WIDTH=${SCGIPIXWIDTH}px>" \
--lazy                             \
$8 $9 >
EOF

	cat << EOF >> $RRDCGISCRIPT
$NEWLINE <TD valign=top>$4<br>
  <RRD::INCLUDE $1/$2_$3.def>
</TD>
EOF
}



gen_cgi_header()
{
	cat << EOF > $RRDCGISCRIPT
#!$RRDCGI
<HEAD><TITLE>RRDCGI Router Statistics</TITLE></HEAD>
<SCRIPT TYPE="text/javascript" LANGUAGE="JavaScript">
function update_radio_check(robj,value) { for (var i=0; i<robj.length; i++) if (robj[i].value==value) robj[i].checked=1; }
var nW,oW;
function zoomToggle(iWideSmall,iWideLarge,whichImage){
oW=whichImage.style.width;
if(oW==iWideLarge){
nW=iWideSmall;}else{
nW=iWideLarge;}
whichImage.style.width=nW;
}
</SCRIPT>
<BODY onload="update_radio_check(document.RangeForm.START,'<RRD::CV START>')">
<H1>RRDCGI Router Statistics <font size-=2><em>(<RRD::TIME::NOW %c>)</em></font></H1>
<FORM NAME=RangeForm>
      <INPUT NAME=START TYPE=RADIO VALUE=end-${HOURS}h checked>${HOURS} hours ago
      <INPUT NAME=START TYPE=RADIO VALUE=end-${DAYS}d>${DAYS} days ago
      <INPUT NAME=START TYPE=RADIO VALUE=end-${WEEKS}w>${WEEKS} weeks ago
      <INPUT TYPE=SUBMIT>
</FORM>
<P>
<TABLE>
EOF
}

gen_cgi_footer()
{
	cat << EOF >> $RRDCGISCRIPT
</TABLE>
</P>
</BODY>
</HTML>
EOF
	chmod +x $RRDCGISCRIPT
}

#################################################################################################################
# $1=mode $2=VAR-PRAEFIX, $3=VAR-POSTFIX, $4=Headline, $5=Vertlabel, $6=start, $7=end, $8=base, $9=other rrdgraph options
#################################################################################################################
do_graph()
{
        MODE=$1; PRAEFIX=$2; POSTFIX=$3; HEADLINE=$4; VERTLABEL=$5; START=$6; END=$7; BASE=$8; SRC=${RRDDIR}/${PRAEFIX}.rrd
	if [ ! -r "$SRC" ] 
	then
		echo "$SRC not found. Aborting all operations now..."
		exit 1
	fi

	# For Graphing Memory BASE should be set to 1024, default is 1000
	[ -n "$8" ] && BASE=$8 || BASE=1000
	[ -n "$9" ] && OPTS=$9 || OPTS="--lower-limit 0"
	DEFS=""; ENTRIES=""
	eval  SOURCE=\$${PRAEFIX}_${POSTFIX}
	for i in $SOURCE
	do
		eval var=\$${PRAEFIX}_${i}
		ifs="$IFS"; IFS="|"; set -- $var
		if  [ "`echo ${1} | cut -b1`" = "-" ]
		then
			VAR="`echo ${1} | cut -b2-`"
			NEG=1
		else
			VAR=`echo $1 | sed "s/ //g"`
			NEG=0
		fi
		
		if [ "$NEG" = 1 ]	
		then
			[ -n "${8}" ] && DEFS="$DEFS DEF:${VAR}=$SRC:${VAR}:AVERAGE  DEF:MAX${VAR}=$SRC:${VAR}:MAX ${8} CDEF:N${VAR}C=${VAR}C,-1,* " \
			              || DEFS="$DEFS DEF:${VAR}=$SRC:${VAR}:AVERAGE  DEF:MAX${VAR}=$SRC:${VAR}:MAX CDEF:N${VAR}=${VAR},-1,*"
		else
			[ -n "${8}" ] && DEFS="$DEFS DEF:${VAR}=$SRC:${VAR}:AVERAGE  DEF:MAX${VAR}=$SRC:${VAR}:MAX ${8}" \
				      || DEFS="$DEFS DEF:${VAR}=$SRC:${VAR}:AVERAGE  DEF:MAX${VAR}=$SRC:${VAR}:MAX"
		fi
		# Now this is a bit ugly, but i don't know how to escape the blanks in a proper way.
		# If you know any better way to keep the blanks in the legend of the graphics please inform me.
		#L=`echo ${2} | tr -s "[= =]" '\014'`
		#L=`echo ${2} | tr -s "[= =]" '_'`
		L=`echo ${2} | sed "s/ /_/g"`
		if [ -n "${7}" ]
		then
			if [ -n "${8}" ]                     
			then
				[ "$NEG" = 1 ] &&  ENTRIES="$ENTRIES ${6}:N${VAR}C${5}:${L}: GPRINT:${VAR}C:AVERAGE:Average\:${7} GPRINT:MAX${VAR}C:MAX:Max\:${7} GPRINT:${VAR}C:LAST:Last\:${7}\\j" \
					       ||  ENTRIES="$ENTRIES ${6}:${VAR}C${5}:${L}:  GPRINT:${VAR}C:AVERAGE:Average\:${7} GPRINT:MAX${VAR}C:MAX:Max\:${7} GPRINT:${VAR}C:LAST:Last\:${7}\\j"
			else
				[ "$NEG" = 1 ] &&  ENTRIES="$ENTRIES ${6}:N${VAR}${5}:${L}: GPRINT:${VAR}:AVERAGE:Average\:${7} GPRINT:MAX${VAR}:MAX:Max\:${7} GPRINT:${VAR}:LAST:Last\:${7}\\j" \
					       ||  ENTRIES="$ENTRIES ${6}:${VAR}${5}:${L}:  GPRINT:${VAR}:AVERAGE:Average\:${7} GPRINT:MAX${VAR}:MAX:Max\:${7} GPRINT:${VAR}:LAST:Last\:${7}\\j"
			fi
		else
			[ "$NEG" = 1 ] && ENTRIES="$ENTRIES ${6}:N${VAR}${5}:${L}: GPRINT:${VAR}:AVERAGE:Average\:%6.2lf%s GPRINT:MAX${VAR}:MAX:Max\:%6.0lf%s GPRINT:${VAR}:LAST:Last\:%6.2lf%s\\j" \
       				       || ENTRIES="$ENTRIES ${6}:${VAR}${5}:${L}:  GPRINT:${VAR}:AVERAGE:Average\:%6.2lf%s GPRINT:MAX${VAR}:MAX:Max\:%6.0lf%s GPRINT:${VAR}:LAST:Last\:%6.2lf%s\\j"
		fi
		IFS="$ifs"
	done

	#[ "$POSTFIX" = "CPU_STATS" ] && (echo $OPTS; echo $DEFS ; echo $ENTRIES; echo $OPTS)

	$MODE "$IMGDIR" "$PRAEFIX" "$POSTFIX" "$HEADLINE" "$END" "$START" "$BASE" "$DEFS $ENTRIES"  "--vertical-label $VERTLABEL $OPTS"
}

# Check if all rrd-files are present 
# and create them if not
init_loop()
{
	for j in $ALL_PRAEFIXES
	do
		if [ ! -f $RRDDIR/$j.rrd ]  
		then
		        echo "Creating $RRDDIR/$j.rrd ..."
                        init $j ALL
                        [ ! -f $RRDDIR/$j.rrd ] && EXIT=1
		fi
	done
	[ "$EXIT" = 1 ] && return 1 || return 0
}


graph_loop()
{
	mode=$1; start=$2; end=$3; c=1 
	while [ $c -le $GRAPH_MAX ]
	do
		eval var=\$GRAPH_${c}
        	# $1=VAR-PRAEFIX $2=VAR-POSTFIX $3=Headline $4=Vertlabel $5=base $6=other rrdgraph options
		ifs="$IFS"; IFS="|"; set -- $var
		IFS="$ifs"
		[ "$1" != "$OLD" -o "`expr \( $c - 1 \) % 2`" = 0 ] && NEWLINE="</TR><TR>" || NEWLINE=""
		do_graph $mode $1 $2 "$3" "$4" $start $end $5 "$6"
		c=`expr $c + 1`; OLD=$1
	done
}


case "$1" in
	init)
			if init_loop 
			then
				exit 0
			else
			 	echo "Initialisation of rrd-database-files failed"
				exit 1
			fi
			;;
	cgi)
			echo "Updating $RRDCGISCRIPT and $IMGDIR/*.def"
			gen_cgi_header
			# The start-time will be not used
			graph_loop do_rrdcgi end now
			gen_cgi_footer
			;;
	images)
			# Create the graphs
			[ -n "$2" ] && STH=$2 || STH=24h
			echo "Updating Graphs ($STH ago -> now) for $RRDDIR/*.rrd"
			graph_loop do_rrdgraph "end-${STH}" now
			;;
	fetch)
			# Fetch data from rrdcollect daemon
			if [ ! -n "$3" ]
			then
				echo "Usage: $0 fetch <host> <port>"
				exit 1
			else
        			echo "Fetching ascii-logs from rrdcollect daemon"
        			(cd $RRDDIR && $NETCAT $2 $3 | sed -n "s:^\(update .*\):$RRDTOOL \1:p" | sh -x ) 2>&1 | wc -l #>/dev/null
				exit 0
			fi
			;;
	*)		echo "Usage: $0 <init|cgi|fetch|images>"
			echo "	init:   Initialize rrd-datafiles if neeeded"
			echo "	cgi:    Creates cgi-script for use with rrdcgi"
			echo "	fetch:  Fetching data from rrdcollect via netcat"
			echo "	images: Generates images"
			exit 1
			;;
esac
exit 0
