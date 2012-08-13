HostsMerge
==========

Merging tool for downloading and merging hosts files into a larger host file.

By default can download from the following hosts files and merge them into a single hosts file:

	http://www.jgilmore.com/AdBin/hosts.txt
	http://www.mvps.org/winhelp2002/hosts.txt
	http://someonewhocares.org/hosts/hosts
	http://pgl.yoyo.org/adservers/serverlist.php?showintro=0;hostformat=hosts


Uses wget to perform any download.

Usage:

HostsMergeCmd c:\windows\system32\drivers\etc\hosts x:\hosts\hosts http://www.mvps.org/winhelp2002/hosts.txt

Produces the following:

	wpad.dat		- a browser auto configuration file to exclude directly in the browser.  May be slow with large lists
	hosts.cache		- an updated hosts file that can be copied over an existing hosts file.

If the following files are present in the current folder these will be used:

	hosts.exclude	- list of hosts to exclude from blocking - defined as a normal hosts file - only the name portion is used
	hosts.cache		- A previous run of data
	hosts.bad		- a local hosts file of bad hosts or domains

The hosts file is optimized so that it will try and reduce the number of entries to block.  Given the following blocking host entries it will produce a single entry (example.com):

127.0.0.1	www.example.com
127.0.0.1	ftp.example.com
127.0.0.1	example.com

Any non blocking entries will be transferred without the optimization applied.

