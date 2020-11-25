LuCI provides some of its libraries to external applications through a JSON-RPC API.
This Howto shows how to use it and provides information about available functions.


# Basics
LuCI comes with an efficient JSON De-/Encoder together with a JSON-RPC-Server which implements the *JSON-RPC 1.0_' and 2.0 (partly) specifications. The LuCI JSON-RPC server offers several independent APIs. Therefore you have to use '_different URLs for every exported library*.
Assuming your LuCI-Installation can be reached through */cgi-bin/luci_' any exported library can be reached via '''/cgi-bin/luci/rpc/''LIBRARY_*.


# Authentication
Most exported libraries will require a valid authentication to be called with. If you get an *HTTP 403 Forbidden_' status code you are probably missing a valid authentication token. To get such a token you have to call the function '''login''' of the RPC-Library '''auth'''. Following our example from above this login function would be provided at '_/cgi-bin/luci/rpc/auth*. The function accepts 2 parameters: username and password (of a valid user account on the host system) and returns an authentication token.

If you want to call any exported library which requires an authentication token you have to *append it as an URL parameter _auth''''' to the RPC-Server URL. So instead of calling '''/cgi-bin/luci/rpc/''LIBRARY''''' you have to call '''/cgi-bin/luci/rpc/''LIBRARY''?auth=''TOKEN_*.

If your JSON-RPC client is Cookie-aware (like most browsers are) you will receive the authentication token also with a session cookie and probably don't have to append it to the RPC-Server URL.


# Exported Libraries
## uci
The UCI-Library */rpc/uci* offers functionality to interact with the Universal Configuration Interface.
*Exported Functions:*
* [(string) add(config, type)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.add)
* [(integer) apply(config)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.apply)
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.changes (object) changes([config])]
* [(boolean) commit(config)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.commit)
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.delete (boolean) delete(config, section[, option])]
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.delete_all (boolean) delete_all(config[, type])]
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.foreach (array) foreach(config[, type])]
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.get (mixed) get(config, section[, option])]
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.get_all (object) get_all(config[, section])]
* [http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.get (mixed) get_state(config, section[, option])]
* [(boolean) revert(config)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.revert)
* [(name) section(config, type, name, values)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.section)
* [(boolean) set(config, section, option, value)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.set)
* [(boolean) tset(config, section, values)](http://luci.subsignal.org/api/luci/modules/luci.model.uci.html#Cursor.tset)

## uvl
The UVL-Library */rpc/uvl* offers functionality to validate UCI files and get schemes describing UCI files.
*Exported Functions:*
* [(array) get_scheme(scheme)](http://luci.subsignal.org/api/luci/modules/luci.uvl.html#UVL.get_scheme)
* [(array) validate(config, section, option)](http://luci.subsignal.org/api/luci/modules/luci.uvl.html#UVL.validate)
* [(array) validate_config(config)](http://luci.subsignal.org/api/luci/modules/luci.uvl.html#UVL.validate_config)
* [(array) validate_section(config, section)](http://luci.subsignal.org/api/luci/modules/luci.uvl.html#UVL.validate_section)
* [(array) validate(config, section, option)](http://luci.subsignal.org/api/luci/modules/luci.uvl.html#UVL.validate_option)

## fs
The Filesystem library */rpc/fs* offers functionality to interact with the filesystem on the host machine.
*Exported Functions:*

* [Complete luci.fs library](http://luci.subsignal.org/api/luci/modules/luci.fs.html)
*Note:* All functions are exported as they are except for _readfile'' which encodes its return value in base64 and ''writefile'' which only accepts base64 encoded data as second argument. Note that both functions will only be available when the ''luasocket_ packet is installed on the hostsystem.

## sys
The System library */rpc/sys* offers functionality to interact with the operating system on the host machine.
*Exported Functions:*
* [Complete luci.sys library](http://luci.subsignal.org/api/luci/modules/luci.sys.html)
* [Complete luci.sys.group library](http://luci.subsignal.org/api/luci/modules/luci.sys.group.html) with prefix *group.*
* [Complete luci.sys.net library](http://luci.subsignal.org/api/luci/modules/luci.sys.net.html) with prefix *net.*
* [Complete luci.sys.process library](http://luci.subsignal.org/api/luci/modules/luci.sys.process.html) with prefix *process.*
* [Complete luci.sys.user library](http://luci.subsignal.org/api/luci/modules/luci.sys.user.html) with prefix *user.*
* [Complete luci.sys.wifi library](http://luci.subsignal.org/api/luci/modules/luci.sys.wifi.html) with prefix *wifi.*

## ipkg
The IPKG library */rpc/ipkg* offers functionality to interact with the package manager (IPKG or OPKG) on the host machine.
*Exported Functions:*
* [Complete luci.model.ipkg library](http://luci.subsignal.org/api/luci/modules/luci.model.ipkg.html)
