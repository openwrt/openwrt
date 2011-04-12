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
	qdisc[n] = $7
	filter[n] = $8
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
		printf "tc qdisc add dev "device" parent 1:"class[i]"0 handle "class[i]"00: "

		# RED parameters - also used to determine the queue length for sfq
		# calculate min value. for links <= 256 kbit, we use 1500 bytes
		# use 50 ms queue length as min threshold for faster links
		# max threshold is fixed to 3*min
		base_pkt=3000
		base_rate=256
		min_lat=50
		if (maxrate[i] <= base_rate) min = base_pkt
		else min = int(maxrate[i] * 1024 / 8 * 0.05)
		max = 3 * min
		limit = (min + max) * 3

		if (qdisc[i] != "") {
			# user specified qdisc
			print qdisc[i] " limit " limit
		} else if (rtm1[i] > 0) {
			# rt class - use sfq
			print "sfq perturb 2 limit "  limit
		} else {
			# non-rt class - use RED

			avpkt = pktsize[i]
			# don't use avpkt values less than 500 bytes
			if (avpkt < 500) avpkt = 500
			# if avpkt is too close to min, scale down avpkt to allow proper bursting
			if (avpkt > min * 0.70) avpkt *= 0.70


			# according to http://www.cs.unc.edu/~jeffay/papers/IEEE-ToN-01.pdf a drop
			# probability somewhere between 0.1 and 0.2 should be a good tradeoff
			# between link utilization and response time (0.1: response; 0.2: utilization)
			prob="0.12"
		
			rburst=int((2*min + max) / (3 * avpkt))
			if (rburst < 2) rburst = 2
			print "red min " min " max " max " burst " rburst " avpkt " avpkt " limit " limit " probability " prob " ecn"
		}
	}
	
	# filter rule
	for (i = 1; i <= n; i++) {
		print "tc filter add dev "device" parent 1: prio "class[i]" protocol ip handle "class[i]" fw flowid 1:"class[i] "0" 
		filterc=1
		if (filter[i] != "") {
			print " tc filter add dev "device" parent "class[i]"00: handle "filterc"0 "filter[i]
			filterc=filterc+1
		}
	}
}

