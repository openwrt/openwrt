BEGIN {
	dmax=100
	if (!(linespeed > 0)) linespeed = 128
	FS=":"
	n = 0
}

($1 != "") {
	n++
	class[n] = $1
	prio[n] = $2
	avgrate[n] = ($3 * linespeed / 100)
	pktsize[n] = $4
	delay[n] = $5
	maxrate[n] = ($6 * linespeed / 100)
}

END {
	allocated = 0
	maxdelay = 0

	for (i = 1; i <= n; i++) {
		# set defaults
		if (!(pktsize[i] > 0)) pktsize[i] = 1500
		if (!(prio[i] > 0)) prio[i] = 1

		allocated += avgrate[i]
		sum_prio += prio[i]
		if ((avgrate[i] > 0) && !(delay[i] > 0)) {
			sum_rtprio += prio[i]
		}
	}
	
	# allocation of m1 in rt classes:
	# sum(d * m1) must not exceed dmax * (linespeed - allocated)
	dmax = 0
	for (i = 1; i <= n; i++) {
		if (avgrate[i] > 0) {
			rtm2[i] = avgrate[i]
			if (delay[i] > 0) {
				d[i] = delay[i]
			} else {
				d[i] = 2 * pktsize[i] * 1000 / (linespeed * 1024)
				if (d[i] > dmax) dmax = d[i]
			}
		}
	}	

	ds_avail = dmax * (linespeed - allocated)
	for (i = 1; i <= n; i++) {
		lsm1[i] = 0
		rtm1[i] = 0
		lsm2[i] = linespeed * prio[i] / sum_prio
		if ((avgrate[i] > 0) && (d[i] > 0)) {
			if (!(delay[i] > 0)) {
				ds = ds_avail * prio[i] / sum_rtprio
				ds_avail -= ds
				rtm1[i] = rtm2[i] + ds/d[i]
			}
			lsm1[i] = rtm1[i]
		}
		else {
			d[i] = 0
		}
	}

	# main qdisc
	for (i = 1; i <= n; i++) {
		printf "tc class add dev "device" parent 1:1 classid 1:"class[i]"0 hfsc"
		if (rtm1[i] > 0) {
			printf " rt m1 " int(rtm1[i]) "kbit d " int(d[i] * 1000) "us m2 " int(rtm2[i])"kbit"
		}
		printf " ls m1 " int(lsm1[i]) "kbit d " int(d[i] * 1000) "us m2 " int(lsm2[i]) "kbit"
		print " ul rate " int(maxrate[i]) "kbit"
	}

	# leaf qdisc
	avpkt = 1200
	for (i = 1; i <= n; i++) {
		ql = int((avgrate[i] + linespeed) * 1024 / (8 * pktsize[i]))
		printf "tc qdisc add dev "device" parent 1:"class[i]"0 handle "class[i]"00: "
		if (rtm1[i] > 0) {
			# rt class - use sfq
			print "sfq perturb 2 limit " ql
		} else {
			# non-rt class - use red
			min = int(maxrate[i] * 1024 / 8 * 0.05)
			if (min < avpkt) min = avpkt
			dqb = 8 * min;
			max = int(2.1*min)
			rburst = int((2*min + max) / (3 * avpkt))
			if (rburst < 2) rburst = 2
			print "red min " min " max " max " burst " rburst " avpkt " avpkt " limit " dqb " probability 0.04 ecn"
		}
	}
	
	# filter rule
	for (i = 1; i <= n; i++) {
		print "tc filter add dev "device" parent 1: prio "class[i]" protocol ip handle "class[i]" fw flowid 1:"class[i] "0" 
	}
}

