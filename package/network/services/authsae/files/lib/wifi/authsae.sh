authsae_start_interface() {
	local device="$1" # to use the correct channel
	local vif="$2"
	local band

	cfgfile="/var/run/authsae-$vif.cfg"
	config_get channel "$device" channel
	config_get hwmode "$device" hwmode
	config_get htmode "$device" htmode
	config_get ifname "$vif" ifname
	config_get key "$vif" key
	config_get mesh_id "$vif" mesh_id
	config_get mcast_rate "$vif" mcast_rate "12"

	case "$htmode" in
		HT20|HT40+|HT40-) htmode="$htmode";;
		NOHT|none|*) htmode="none";;
	esac

	case "$hwmode" in
		*g*) band=11g;;
		*a*) band=11a;;
	esac

	cat > "$cfgfile" <<EOF
authsae:
{
 sae:
  {
    debug = 0;
    password = "$key";
    group = [19, 26, 21, 25, 20];
    blacklist = 5;
    thresh = 5;
    lifetime = 3600;
  };
 meshd:
  {
    meshid = "$mesh_id";
    interface = "$ifname";
    passive = 0;
    debug = 0;
    mediaopt = 1;
    band = "$band";
    channel = $channel;
    htmode = "$htmode";
    mcast-rate = $mcast_rate;
  };
};
EOF
	ifconfig "$ifname" up
	meshd-nl80211 -i "$ifname" -s "$mesh_id" -c "$cfgfile" -B
}
