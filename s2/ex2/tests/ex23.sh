#!/bin/bash
# Noam Koren 308192871


switch_host() {
	_static_hostname="0"
	_name="0"
	_machine_id="0"
	_boot_id="0"
	_virtualization="0"
	_kernel="0"
	_architecture="0"
	_any="0"
	for arg in "$@" ; do
    case $arg in
        --static_hostname)
		if [[ $_static_hostname == "0" ]] ; then
			echo $(awk 'NR==1' hostnamectl) | cut -d':' -f 2 | xargs
			_static_hostname="1"
			_any="1"
		fi
		shift # Remove --cache= from processing
        ;;
		--name|--icon_name)
		if [[ $_name == "0" ]] ; then
        	echo $(awk 'NR==2' hostnamectl) | cut -d':' -f 2 | xargs
			_name="1"
			_any="1"
		fi
		shift # Remove --initialize from processing
        ;;
        --machine_id)
		if [[ $_machine_id == "0" ]] ; then
			echo $(awk 'NR==4' hostnamectl) | cut -d':' -f 2 | xargs
			__machine_id="1"
			_any="1"
		fi
		shift # Remove argument value from processing
        ;;
		--boot_id)
		if [[ $_boot_id == "0" ]] ; then
			echo $(awk 'NR==5' hostnamectl) | cut -d':' -f 2 | xargs
			_boot_id="1"
			_any="1"
		fi
		shift
		;;
		--virtualization)
		if [[ $_virtualization == "0" ]] ; then
			echo $(awk 'NR==6' hostnamectl) | cut -d':' -f 2 | xargs
			__virtualization="1"
			_any="1"
		fi
		shift
		;;
		--kernel)
		if [[ $_kernel == "0" ]] ; then
			echo $(awk 'NR==8' hostnamectl) | cut -d':' -f 2 | xargs
			_kernel="1"
			_any="1"
		fi		
		shift
		;;
		--architecture)
		if [[ $_architecture == "0" ]] ; then
			echo $(awk 'NR==9' hostnamectl) | cut -d':' -f 2 | xargs
			_architecture="1"
			_any="1"
		fi
		shift
		;;
        *)
        shift # Remove generic argument from processing
        ;;
    esac
done
if [[ $_any == "0" ]] ; then
	cat hostnamectl
fi
}


switch_system() {
	_name="0"
	_version="0"
	_pretty_name="0"
	_home_url="0"
	_support_url="0"
	_any="0"
	for arg in "$@" ; do
    case $arg in
        --name)
		if [[ $_name == "0" ]] ; then
			echo $(awk 'NR==1' os-release) | cut -d'=' -f 2 | xargs
			_name="1"
			_any="1"
		fi
		shift # Remove --cache= from processing
        ;;
		--version)
		if [[ $_version == "0" ]] ; then
        	echo $(awk 'NR==2' os-release) | cut -d'=' -f 2 | xargs
			_version="1"
			_any="1"
		fi
		shift # Remove --initialize from processing
        ;;
        --pretty_name)
		if [[ $_pretty_name == "0" ]] ; then
			echo $(awk 'NR==5' os-release) | cut -d'=' -f 2 | xargs
			_pretty_name="1"
			_any="1"
		fi
		shift # Remove argument value from processing
        ;;
		--home_url)
		if [[ $_home_url == "0" ]] ; then
			echo $(awk 'NR==7' os-release) | cut -d'=' -f 2 | xargs
			_home_url="1"
			_any="1"
		fi
		shift
		;;
		--support_url)
		if [[ $_support_url == "0" ]] ; then
			echo $(awk 'NR==8' os-release) | cut -d'=' -f 2 | xargs
			_support_url="1"
			_any="1"
		fi
		shift
		;;
       	*)
        shift # Remove generic argument from processing
        ;;
    esac
done
if [[ $_any == "0" ]] ; then
	cat os-release
fi
}

if [ ! -z $1 ] ; then
	if [ $1 == "host" ] ; then
		if [ -z $2 ] ; then 
			cat hostnamectl
		else
			switch_host "$@"
		fi
	elif [ $1 == "system" ] ; then
		if [ -z $2 ] ; then
			cat os-release
		else 
			switch_system "$@"
		fi
	else
		echo "Invalid input"
	fi
else
  echo "Invalid input"
fi
